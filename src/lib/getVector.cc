#include "getVector.h"
#include "node-argument.h"


void GetVector::Execute () {
	try {
        result_ = wrapper_->getVector( word_ );
    } catch (std::string errorMessage) {
        SetErrorMessage(errorMessage.c_str());
    }
}

// 错误回调
void GetVector::HandleErrorCallback () {
    Nan::HandleScope scope;
    auto res =  GetFromPersistent("key").As<v8::Promise::Resolver>();
    res->Reject( Nan::GetCurrentContext() , Nan::Error(ErrorMessage()));
    v8::Isolate::GetCurrent()->RunMicrotasks();
}

// 成功回调
void GetVector::HandleOKCallback () {
	
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Object> returnObject = v8::Object::New(isolate);

    for (auto const& iterator : result_ ) {
        uint size = iterator.second.size();
        v8::Local<v8::Array> value = v8::Array::New(isolate, size); 

        for (uint i = 0; i < size; ++i) {
            v8::Handle<v8::Number> k = v8::Number::New(isolate, i ); 
            v8::Handle<v8::Number> v = v8::Number::New(isolate, iterator.second[i]);
            value->Set(k, v);
        }

        returnObject->Set(
            v8::String::NewFromUtf8(isolate, "text"), 
            v8::String::NewFromUtf8(isolate, iterator.first.c_str())
        );

        returnObject->Set(
            v8::String::NewFromUtf8(isolate, "value"), 
            value
        );
    }
    
    auto res =  GetFromPersistent("key").As<v8::Promise::Resolver>();
    res->Resolve( Nan::GetCurrentContext() , returnObject );
    v8::Isolate::GetCurrent()->RunMicrotasks();
}