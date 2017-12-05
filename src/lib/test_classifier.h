#ifndef TESTCLASSIFIER_H
#define TESTCLASSIFIER_H

#include <nan.h>
#include "wrapper.h"

class TestClassifier : public Nan::AsyncWorker {
	public:
		TestClassifier(std::string filename , int32_t k, Wrapper *wrapper) : 
			Nan::AsyncWorker(new Nan::Callback()) , 
			filename(filename),
			k_(k),
			result_(),
			wrapper_(wrapper) {};

		~TestClassifier() {};

		void Execute ();
        void HandleOKCallback ();
        void HandleErrorCallback ();

    private : 
    	std::string filename;
    	int32_t k_;
    	std::map<std::string, std::string> result_;
    	Wrapper *wrapper_;
};

#endif