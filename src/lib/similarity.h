#ifndef SIMILARITY_H
#define SIMILARITY_H

#include <nan.h>
#include "wrapper.h"

class Similarity : public Nan::AsyncWorker {
	public:
		Similarity( std::string what ,std::string with, Wrapper *wrapper) : 
			Nan::AsyncWorker(new Nan::Callback()) , 
			what_(what),
			with_(with),
			wrapper_(wrapper),
			result_() {};

		~Similarity() {};

		void Execute ();
        void HandleOKCallback ();
        void HandleErrorCallback ();

    private : 
    	std::string what_;
    	std::string with_;
    	Wrapper *wrapper_;
    	float result_;
};

#endif