#include <nan.h>
#include "./lib/fasttext.h"

NAN_MODULE_INIT(Init) {
	FastText::Init(target);
}

NODE_MODULE(fastText, Init)
