// query.h
#ifndef FASTTEXT_H
#define FASTTEXT_H

#include "wrapper.h"
#include "node-argument.h"

#include "loadModel.h"
#include "predict.h"
#include "train.h"
#include "test_classifier.h"
#include "quantize.h"
#include "nn.h"

class FastText : public Nan::ObjectWrap {
    public: 
    	static NAN_MODULE_INIT(Init) {
            v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
            tpl->SetClassName(Nan::New("FastText").ToLocalChecked());
            tpl->InstanceTemplate()->SetInternalFieldCount(1);

            Nan::SetPrototypeMethod(tpl, "train", train);
            Nan::SetPrototypeMethod(tpl, "quantize", quantize);
            Nan::SetPrototypeMethod(tpl, "test", testClassifier );
            Nan::SetPrototypeMethod(tpl, "loadModel", loadModel);
            Nan::SetPrototypeMethod(tpl, "predict", predict);
            Nan::SetPrototypeMethod(tpl, "nn", Nn);


            constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
            Nan::Set(target, Nan::New("FastText").ToLocalChecked(),
            Nan::GetFunction(tpl).ToLocalChecked());
        }

    private:

    	explicit FastText() :
            wrapper_(new Wrapper())
            {};

        ~FastText() {}

        // 实例化
        static NAN_METHOD(New) {
            if (info.IsConstructCall()) {
	        	FastText *obj = new FastText();
	        	obj->Wrap(info.This());
	        	info.GetReturnValue().Set(info.This());
	        } else {
	        	const int argc = 1;
                v8::Local<v8::Value> argv[argc] = {info[0]};
                v8::Local<v8::Function> cons = Nan::New(constructor());
                info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
	        }
        }

        // 训练模型
        static NAN_METHOD( train ) {

           if (!info[0]->IsString()) {
                Nan::ThrowError("First argument must be a command string.");
                return;
            }

            if (!info[1]->IsObject()) {
                Nan::ThrowError("Second argument must be an object.");
                return;
            }
            
            v8::String::Utf8Value commandArg(info[0]->ToString());
            v8::String::Utf8Value commandConf(info[1]->ToString());
            std::string command = std::string(*commandArg);
            std::string conf = std::string(*commandConf);


            if (command == "skipgram" || command == "cbow" || command == "supervised") {

                v8::Local<v8::Object> confObj = v8::Local<v8::Object>::Cast( info[1] );

                NodeArgument::NodeArgument nodeArg;
                NodeArgument::CArgument c_argument;

                try {
                    c_argument = nodeArg.ObjectToCArgument( confObj );
                } catch (std::string errorMessage) {
                    Nan::ThrowError(errorMessage.c_str());
                    return;
                }

                int count = c_argument.argc;
                char** argument = c_argument.argv;

                char *emptyString = (char *)"-command";
                char *argv[count + 2];
                int argc = count + 2; // increment the argc

                for(int j = 0; j < count; j++) {
                    argv[j + 2] = argument[j];
                }

                argv[0] = emptyString;
                argv[1] = (char *) command.c_str();
                FastText* obj = Nan::ObjectWrap::Unwrap<FastText>(info.Holder());
                std::vector<std::string> args(argv, argv + argc);

                auto worker = new Train(args, obj->wrapper_);
                auto resolver = v8::Promise::Resolver::New( info.GetIsolate());
                worker->SaveToPersistent("key",resolver);
                info.GetReturnValue().Set(resolver->GetPromise());
                Nan::AsyncQueueWorker( worker );
            } else {
                Nan::ThrowError("Permitted command type is ['skipgram', 'cbow', 'supervised'].");
                return;
            }
        }

        // 测试分类
        static NAN_METHOD( testClassifier ) {
            if (!info[0]->IsString()) {
                Nan::ThrowError("test file path must be a string");
                return;
            }
            if (!info[1]->IsUint32()) {
                Nan::ThrowError("k must be a number");
                return;
            }

            v8::String::Utf8Value modelArg(info[0]->ToString());
            std::string filename = std::string(*modelArg);
            int32_t k = info[1]->Uint32Value();

            FastText* obj = Nan::ObjectWrap::Unwrap<FastText>( info.Holder() );

            auto worker = new TestClassifier( filename, k, obj->wrapper_);
            auto resolver = v8::Promise::Resolver::New( info.GetIsolate());

            worker->SaveToPersistent("key",resolver);
            info.GetReturnValue().Set( resolver->GetPromise() );
            Nan::AsyncQueueWorker( worker );
        }

        // 模型转换
        static NAN_METHOD( quantize ) {

            if (!info[0]->IsObject()) {
                Nan::ThrowError("First argument must be an object.");
                return;
            }
            
            v8::Local<v8::Object> confObj = v8::Local<v8::Object>::Cast( info[0] );
            NodeArgument::NodeArgument nodeArg;
            NodeArgument::CArgument c_argument;

            try {
                c_argument = nodeArg.ObjectToCArgument( confObj );
            } catch (std::string errorMessage) {
                Nan::ThrowError(errorMessage.c_str());
                return;
            }

            int count = c_argument.argc;
            char** argument = c_argument.argv;

            char *argv[count + 2];
            int argc = count + 2; 
            for(int j = 0; j < count; j++) {
                argv[j + 2] = argument[j];
            }

            FastText* obj = Nan::ObjectWrap::Unwrap<FastText>(info.Holder());
            std::vector<std::string> args(argv, argv + argc);

            auto worker = new Quantize(args, obj->wrapper_);
            auto resolver = v8::Promise::Resolver::New( info.GetIsolate() );
            worker->SaveToPersistent("key",resolver);
            info.GetReturnValue().Set(resolver->GetPromise());
            Nan::AsyncQueueWorker( worker );
        }

        // 加载模型
        static NAN_METHOD( loadModel ) {

        	if (!info[0]->IsString()) {
                Nan::ThrowError("model file path must be a string");
                return;
            } 

        	v8::String::Utf8Value modelArg(info[0]->ToString());
        	std::string filename = std::string(*modelArg);
        	FastText* obj = Nan::ObjectWrap::Unwrap<FastText>( info.Holder() );

            auto worker = new LoadModel(filename, obj->wrapper_);
            auto resolver = v8::Promise::Resolver::New( info.GetIsolate());
            worker->SaveToPersistent("key",resolver);
            info.GetReturnValue().Set(resolver->GetPromise());
            Nan::AsyncQueueWorker( worker );
        }

        // 分类预测
        static NAN_METHOD( predict ) {

            if (!info[0]->IsString()) {
                Nan::ThrowError("sentence must be a string");
                return;
            }
            if (!info[1]->IsUint32()) {
                Nan::ThrowError("k must be a number");
                return;
            }

            int32_t k = info[1]->Uint32Value();
            v8::String::Utf8Value sentenceArg(info[0]->ToString());
            std::string sentence = std::string(*sentenceArg);
            FastText* obj = Nan::ObjectWrap::Unwrap<FastText>(info.Holder());

            auto resolver = v8::Promise::Resolver::New( info.GetIsolate());
            auto worker = new Predict( sentence, k, obj->wrapper_ );
            worker->SaveToPersistent("key",resolver);
            info.GetReturnValue().Set(resolver->GetPromise());
            Nan::AsyncQueueWorker(worker);
        }

        // 邻近词
        static NAN_METHOD( Nn ) {
            if (!info[0]->IsString()) {
                Nan::ThrowError("sentence must be a string");
                return;
            }
            if (!info[1]->IsUint32()) {
                Nan::ThrowError("k must be a number");
                return;
            }
           
            int32_t k = info[1]->Uint32Value();
            v8::String::Utf8Value sentenceArg(info[0]->ToString());
            std::string sentence = std::string(*sentenceArg);
            FastText* obj = Nan::ObjectWrap::Unwrap<FastText>(info.Holder());
            
            auto resolver = v8::Promise::Resolver::New( info.GetIsolate());
            auto worker = new NN( sentence, k, obj->wrapper_ );
            worker->SaveToPersistent("key",resolver);
            info.GetReturnValue().Set(resolver->GetPromise());
            Nan::AsyncQueueWorker(worker);
        }

        static inline Nan::Persistent<v8::Function> & constructor() {
            static Nan::Persistent<v8::Function> my_constructor;
            return my_constructor;
        }


        Wrapper* wrapper_;
};

#endif