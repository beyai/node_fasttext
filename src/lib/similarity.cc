#include <iostream>
#include "similarity.h"

void Similarity::Execute () {
	try {
        result_ = wrapper_->similarity( what_ , with_ );
    } catch (std::string errorMessage) {
        SetErrorMessage(errorMessage.c_str());
    }
}

// 错误回调
void Similarity::HandleErrorCallback () {
    Nan::HandleScope scope;
    auto res =  GetFromPersistent("key").As<v8::Promise::Resolver>();
    res->Reject( Nan::GetCurrentContext() , Nan::Error(ErrorMessage()));
    v8::Isolate::GetCurrent()->RunMicrotasks();
}

// 成功回调
void Similarity::HandleOKCallback () {
    Nan::HandleScope scope;
    v8::Local<v8::Number> result = Nan::New<v8::Number>( result_ );
    auto res =  GetFromPersistent("key").As<v8::Promise::Resolver>();
    res->Resolve( Nan::GetCurrentContext() , result);
    v8::Isolate::GetCurrent()->RunMicrotasks();
}