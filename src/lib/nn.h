#ifndef NN_H
#define NN_H

#include <nan.h>
#include "wrapper.h"

class NN : public Nan::AsyncWorker {
	public:
		NN(std::string sentence , int32_t k, Wrapper *wrapper) : 
			Nan::AsyncWorker(new Nan::Callback()) , 
			sentence_(sentence),
			wrapper_(wrapper),
			result_(),
			k_(k){};

		~NN() {};

		void Execute ();
        void HandleOKCallback ();
        void HandleErrorCallback ();

    private : 
    	std::string sentence_;
    	Wrapper *wrapper_;
    	std::vector<PredictResult> result_;
        int32_t k_;
};

#endif