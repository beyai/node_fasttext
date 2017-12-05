#ifndef GETVECTOR_H
#define GETVECTOR_H

#include <nan.h>
#include "wrapper.h"

class GetVector : public Nan::AsyncWorker {
	public:
		GetVector(std::string word , Wrapper *wrapper) : 
			Nan::AsyncWorker(new Nan::Callback()) , 
			word_(word),
			result_(),
			wrapper_(wrapper) {};

		~GetVector() {};

		void Execute ();
        void HandleOKCallback ();
        void HandleErrorCallback ();

    private : 
    	std::string word_;
    	std::map<std::string, std::vector<double>> result_;
    	Wrapper *wrapper_;
};

#endif