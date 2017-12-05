#include "test_classifier.h"
#include "node-argument.h"


void TestClassifier::Execute () {
	try {
        result_ = wrapper_->test( filename , k_ );
    } catch (std::string errorMessage) {
        SetErrorMessage(errorMessage.c_str());
    }
}

// 错误回调
void TestClassifier::HandleErrorCallback () {
    Nan::HandleScope scope;
    auto res =  GetFromPersistent("key").As<v8::Promise::Resolver>();
    res->Reject( Nan::GetCurrentContext() , Nan::Error(ErrorMessage()));
    v8::Isolate::GetCurrent()->RunMicrotasks();
}

// 成功回调
void TestClassifier::HandleOKCallback () {
	
    Nan::HandleScope scope;
    NodeArgument::NodeArgument nodeArg;
    v8::Local<v8::Object> result = nodeArg.mapToObject( result_ );

    auto res =  GetFromPersistent("key").As<v8::Promise::Resolver>();
    res->Resolve( Nan::GetCurrentContext() , result);
    v8::Isolate::GetCurrent()->RunMicrotasks();
}