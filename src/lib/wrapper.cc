#include <math.h>
#include <fenv.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <queue>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "wrapper.h"

// 模型是否加载
Wrapper::Wrapper() : 
	isLoaded(false),
	isPrecomputed_(false) {}

// 文件是否存在
bool Wrapper::fileExist(const std::string& filename){
    if (FILE *file = fopen(filename.c_str(), "r")) {
    	fclose(file);
    	return true;
    } else {
    	return false;
    }
}

// 验证模型
bool Wrapper::checkModel( std::istream& in ) {
	int32_t magic;
	int32_t version;
	in.read((char*)&(magic), sizeof(int32_t));
	if (magic != FASTTEXT_FILEFORMAT_MAGIC_INT32) {
		return false;
	}
	in.read((char*)&(version), sizeof(int32_t));

	if (version != FASTTEXT_VERSION) {
		return false;
	}

	return true;
}

// 获取向量
void Wrapper::getVector(Vector& vec, const std::string& word) {
	const std::vector<int32_t>& ngrams = dict_->getNgrams(word);
    vec.zero();
    for (auto it = ngrams.begin(); it != ngrams.end(); ++it) {
        vec.addRow(*input_, *it);
    }
    if (ngrams.size() > 0) {
        vec.mul(1.0 / ngrams.size());
    }
}

// 训练数据模型
std::map<std::string, std::string> Wrapper::train(const std::vector<std::string> args) {
	std::shared_ptr<Args> a = std::make_shared<Args>();
	a->parseArgs(args);
	std::string inputFilename = a->input;
	if ( !fileExist(inputFilename) ) {
		throw "Input file is not exist.";
	}
	fasttext::FastText fasttext;
	fasttext.train(a);
	return loadModel(a->output + ".bin");
}

// 获取模型信息
std::map<std::string, std::string> Wrapper::getModelInfo() {
	std::map<std::string, std::string> response;
	// dictionary
    response["word_count"] = std::to_string( dict_->nwords() );
    response["label_count"] = std::to_string( dict_->nlabels() );
    response["token_count"] = std::to_string( dict_->ntokens() );
    // arguments
    response["lr"] = std::to_string( args_->lr );
    response["dim"] = std::to_string( args_->dim );
    response["ws"] = std::to_string( args_->ws );
    response["epoch"] = std::to_string( args_->epoch );
    response["minCount"] = std::to_string( args_->minCount );
    response["minCountLabel"] = std::to_string( args_->minCountLabel );
    response["neg"] = std::to_string( args_->neg );
    response["wordNgrams"] = std::to_string( args_->wordNgrams );

	std::string loss_name = "";
	if (args_->loss == fasttext::loss_name::hs ) {
		loss_name = "hs";
	} else if (args_->loss == fasttext::loss_name::ns ) {
		loss_name = "ns";
	} else if (args_->loss == fasttext::loss_name::softmax ) {
		loss_name = "softmax";
	}

	std::string model_name = "";
	if (args_->model == fasttext::model_name::cbow ) {
		model_name = "cbow";
	} else if (args_->model == fasttext::model_name::sup ) {
		model_name = "supervised";
	} else if (args_->model == fasttext::model_name::sg ) {
		model_name = "skipgram";
	}

	response["loss"] = loss_name;
	response["model"] = model_name;
	response["bucket"] = std::to_string( args_->bucket );
	response["minn"] = std::to_string( args_->minn );
	response["maxn"] = std::to_string( args_->maxn );
	response["thread"] = std::to_string( args_->thread );
	response["lrUpdateRate"] = std::to_string( args_->lrUpdateRate );
	response["t"] = std::to_string( args_->t );
	response["label"] = args_->label;
	response["verbose"] = std::to_string( args_->verbose );
	response["pretrainedVectors"] = args_->pretrainedVectors;

	return response;
}

// 加载模型
std::map<std::string, std::string> Wrapper::loadModel(std::string filename) {
	// 判断模型是否已加载
	if (isModelLoaded()) {
		return getModelInfo();
	}

	// 读取模型文件
	std::ifstream ifs(filename, std::ifstream::binary);
	if (!ifs.is_open()) {
		throw "Model file cannot be opened for loading!";
	}
	// 验证模型
	if (!checkModel(ifs)) {
        throw "Model file has wrong file format!";
    }

    args_->load(ifs);
    dict_->load(ifs);
    bool quant_input;

    ifs.read((char*) &quant_input, sizeof(bool));
    if (quant_input) {
		qinput_->load(ifs);
	} else {
		input_->load(ifs);
	}

	ifs.read((char*) &args_->qout, sizeof(bool));
	if (quant_input && args_->qout) {
		qoutput_->load(ifs);
	} else {
		output_->load(ifs);
	}

	model_ = std::make_shared<Model>(input_, output_, args_, 0);
	model_->quant_ = quant_input;
	model_->setQuantizePointer(qinput_, qoutput_, args_->qout);

	if (args_->model == fasttext::model_name::sup) {
		// 分类模型
		model_->setTargetCounts(dict_->getCounts(fasttext::entry_type::label));
	} else {
		// 词向量模型
		model_->setTargetCounts(dict_->getCounts(fasttext::entry_type::word));
        precomputeWordVectors();
	}
	isLoaded = true;
	ifs.close();
	return getModelInfo();
}

// 预计算词向量
void Wrapper::precomputeWordVectors() {
    if (isPrecomputed_) {
        return;
    }
    precomputeMtx_.lock();
    if (isPrecomputed_) {
        precomputeMtx_.unlock();
        return;
    }
    Matrix wordVectors(dict_->nwords(), args_->dim);
    wordVectors_ = wordVectors;
    Vector vec(args_->dim);
    wordVectors_.zero();
    for (int32_t i = 0; i < dict_->nwords(); i++) {
        std::string word = dict_->getWord(i);
        getVector(vec, word);
        real norm = vec.norm();
        wordVectors_.addRow(vec, i, 1.0 / norm);
    }
    isPrecomputed_ = true;
    precomputeMtx_.unlock();
}

// 分类预测
std::vector<PredictResult> Wrapper::predict (std::string sentence , int32_t k ) {
	std::vector<PredictResult> arr;
	std::vector<int32_t> words, labels;
	std::istringstream in(sentence);
	dict_->getLine(in, words, labels, model_->rng);

	if (words.empty()) {
        return arr;
    }

    Vector hidden(args_->dim);
    Vector output(dict_->nlabels());
    std::vector<std::pair<real,int32_t>> modelPredictions;
    model_->predict(words, k, modelPredictions, hidden, output);

    PredictResult response;
    for (auto it = modelPredictions.cbegin(); it != modelPredictions.cend(); it++) {
        response = { dict_->getLabel(it->second), exp(it->first) };
        arr.push_back(response);
    }

    return arr;
}

// 邻近词查询
std::vector<PredictResult> Wrapper::findNN(const Vector& queryVec, int32_t k, const std::set<std::string>& banSet ) {
	real queryNorm = queryVec.norm();

	if (std::abs(queryNorm) < 1e-8) {
        queryNorm = 1;
    }

    std::priority_queue<std::pair<real, std::string>> heap;
    Vector vec(args_->dim);

    for (int32_t i = 0; i < dict_->nwords(); i++) {
        std::string word = dict_->getWord(i);
        real dp = wordVectors_.dotRow(queryVec, i);
        heap.push(std::make_pair(dp / queryNorm, word));
    }

    PredictResult response;
    std::vector<PredictResult> arr;
    int32_t i = 0;

    while (i < k && heap.size() > 0) {
        auto it = banSet.find(heap.top().second);
        if (it == banSet.end()) {
            response = { heap.top().second, exp(heap.top().first) };
            arr.push_back(response);
            i++;
        }
        heap.pop();
    }
    
    return arr;
}

// 邻近词查询
std::vector<PredictResult> Wrapper::nn(std::string query, int32_t k) {
	Vector queryVec(args_->dim);
    std::set<std::string> banSet;
    banSet.clear();
    banSet.insert(query);
    getVector(queryVec, query);
    return findNN(queryVec, k, banSet);
}











