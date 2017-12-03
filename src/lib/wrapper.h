#ifndef WRAPPER_H
#define WRAPPER_H

#include <string>
#include <vector>
#include <map>

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
        std::shared_ptr<Args>       args_ = std::make_shared<Args>();
        std::shared_ptr<Dictionary> dict_ = std::make_shared<Dictionary>(args_);
        std::shared_ptr<Matrix>     input_ = std::make_shared<Matrix>();
        std::shared_ptr<Matrix>     output_ = std::make_shared<Matrix>();
        std::shared_ptr<QMatrix>    qinput_ = std::make_shared<QMatrix>();
        std::shared_ptr<QMatrix>    qoutput_ = std::make_shared<QMatrix>();
        std::shared_ptr<Model>      model_ = NULL;
        Matrix  wordVectors_;

        // 模型是否加载
        bool isLoaded;
        bool isModelLoaded(){ return isLoaded; }

        
        // 模型文件是否存在
        bool fileExist(const std::string& filename);
    	// 验证模型
    	bool checkModel( std::istream& );

    	// 获取向量
    	void getVector(Vector& , const std::string&);
        // 预计算词向量
        void precomputeWordVectors();
        std::mutex precomputeMtx_;
        bool isPrecomputed_;

    	// 邻近词查询
    	std::vector<PredictResult> findNN(const Vector&, int32_t, const std::set<std::string>&);

    // 公共
    public:
        Wrapper();

    	// 训练数据模型
    	std::map<std::string, std::string> train(const std::vector<std::string> args);
        // 获取模型信息
        std::map<std::string, std::string> getModelInfo();
    	// 加载模型
    	std::map<std::string, std::string> loadModel(std::string filename); 

    	// 分类预测
		std::vector<PredictResult> predict(std::string sentence , int32_t k );
		// 邻近词查询
		std::vector<PredictResult> nn(std::string query, int32_t k);
};
#endif