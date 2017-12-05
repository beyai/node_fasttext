#include <iostream>
#include "predict.h"

void Predict::Execute () {
	try {
        result_ = wrapper_->predict( sentence_ , k_ );
    } catch (std::string errorMessage) {
        SetErrorMessage(errorMessage.c_str());
    }
}

// 错误回调
void Predict::HandleErrorCallback () {
    Nan::HandleScope scope;
    auto res =  GetFromPersistent("key").As<v8::Promise::Resolver>();
    res->Reject( Nan::GetCurrentContext() , Nan::Error(ErrorMessage()));
    v8::Isolate::GetCurrent()->RunMicrotasks();
}

// 成功回调
void Predict::HandleOKCallback () {
	
    Nan::HandleScope scope;
    v8::Local<v8::Array> result = Nan::New<v8::Array>( result_.size() );

    for(uint32_t i = 0; i < result_.size(); i++) {
        v8::Local<v8::Object> returnObject = Nan::New<v8::Object>();
        returnObject->Set(
            Nan::New<v8::String>("label").ToLocalChecked(),
            Nan::New<v8::String>(result_[i].label.c_str()).ToLocalChecked()
        );
        returnObject->Set(
            Nan::New<v8::String>("value").ToLocalChecked(),
            Nan::New<v8::Number>(result_[i].value)
        );
        result->Set(i, returnObject);
    }
    auto res =  GetFromPersistent("key").As<v8::Promise::Resolver>();
    res->Resolve( Nan::GetCurrentContext() , result);
    v8::Isolate::GetCurrent()->RunMicrotasks();
}