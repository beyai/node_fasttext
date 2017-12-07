#ifndef WRAPPER_H
#define WRAPPER_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <queue>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <mutex>
#include <array>

#include "./fasttext/src/fasttext.h"

using fasttext::Args;
using fasttext::Dictionary;
using fasttext::Matrix;
using fasttext::QMatrix;
using fasttext::Model;
using fasttext::Vector;
using fasttext::real;

// 数据返回的结构体
struct PredictResult {
    std::string label;
    double value;
};


class Wrapper {
	// 私有
    private:
        fasttext::FastText          fastText;
        std::shared_ptr<Args>       args_   = std::make_shared<Args>();
        std::shared_ptr<Dictionary> dict_   = std::make_shared<Dictionary>(args_);
        std::shared_ptr<Matrix>     input_  = std::make_shared<Matrix>();
        std::shared_ptr<Matrix>     output_ = std::make_shared<Matrix>();
        std::shared_ptr<QMatrix>    qinput_ = std::make_shared<QMatrix>();
        std::shared_ptr<QMatrix>    qoutput_ = std::make_shared<QMatrix>();
        std::shared_ptr<Model>      model_ = NULL;
        Matrix  wordVectors_;

        std::mutex precomputeMtx_;

        // 模型是否加载
        bool isLoaded;
        bool quant_; //是不是为转换后的模型
        bool isPrecomputed_;
        bool isModelLoaded(){ return isLoaded; }
        // 模型文件是否存在
        bool fileExist(const std::string& filename);
    	// 验证模型
    	bool checkModel( std::istream& );
    	// 获取向量
    	void getVector(Vector& , const std::string&);
        // 预计算词向量
        void precomputeWordVectors();
        // 获取模型信息
        std::map<std::string, std::string> getModelInfo();
    	// 邻近词查询
    	std::vector<PredictResult> findNN(const Vector&, int32_t, const std::set<std::string>&);
        // 词向量
        std::map<std::string, std::vector<double>> wordVectors( std::string query );
        // 文本向量
        std::map<std::string, std::vector<double>> textVectors( std::string query );


    // 公共
    public:
        Wrapper();
    	// 训练数据模型
        std::map<std::string, std::string> train(const std::vector<std::string> args);
        // 转换模型
    	std::map<std::string, std::string> quantize(const std::vector<std::string> args);
        // 测试分类
        std::map<std::string, std::string> test( std::string testFile, int32_t k );
    	// 加载模型
    	std::map<std::string, std::string> loadModel(std::string filename); 
    	// 分类预测
		std::vector<PredictResult> predict(std::string sentence , int32_t k );
		// 邻近词查询
		std::vector<PredictResult> nn(std::string query, int32_t k);
        // 词语类比
        std::vector<PredictResult> analogies( std::vector<std::string> words, int32_t k );
        // 打印向量
       std::map<std::string, std::vector<double>>  getVector(  std::string query );
       // 计算两个词语的语义距离
       float similarity(std::string word1, std::string word2 );
};
#endif