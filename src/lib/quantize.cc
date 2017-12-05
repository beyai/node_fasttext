
#include "node-argument.h"
#include "quantize.h"


void Quantize::Execute () {
	try {
        result_ = wrapper_->quantize( args_ );
    } catch (std::string errorMessage) {
        SetErrorMessage(errorMessage.c_str());
    }
}


// 错误回调
void Quantize::HandleErrorCallback () {
    Nan::HandleScope scope;
    auto res =  GetFromPersistent("key").As<v8::Promise::Resolver>();
    res->Reject( Nan::Error(ErrorMessage()) );
    v8::Isolate::GetCurrent()->RunMicrotasks();
}

// 成功回调
void Quantize::HandleOKCallback () {
    Nan::HandleScope scope;
    NodeArgument::NodeArgument nodeArg;
    v8::Local<v8::Object> result = nodeArg.mapToObject( result_ );
    auto res = GetFromPersistent("key").As<v8::Promise::Resolver>();
    res->Resolve( result );
    v8::Isolate::GetCurrent()->RunMicrotasks();
}