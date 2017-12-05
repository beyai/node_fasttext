#ifndef ANALOGIES_H
#define ANALOGIES_H

#include <nan.h>
#include "wrapper.h"

class Analogies : public Nan::AsyncWorker {
	public:
		Analogies( std::vector<std::string> words , int32_t k, Wrapper *wrapper) : 
			Nan::AsyncWorker(new Nan::Callback()) , 
			words_(words),
			wrapper_(wrapper),
			result_(),
			k_(k){};

		~Analogies() {};

		void Execute ();
        void HandleOKCallback ();
        void HandleErrorCallback ();

    private : 
    	std::vector<std::string> words_;
    	Wrapper *wrapper_;
    	std::vector<PredictResult> result_;
        int32_t k_;
};

#endif