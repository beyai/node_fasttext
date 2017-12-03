{
    "targets": [
        {
            "target_name": "fasttext",
            "sources": [

				"src/lib/fasttext/src/args.cc",
				"src/lib/fasttext/src/dictionary.cc",
				"src/lib/fasttext/src/matrix.cc",
				"src/lib/fasttext/src/qmatrix.cc",
				"src/lib/fasttext/src/vector.cc",
				"src/lib/fasttext/src/model.cc",
				"src/lib/fasttext/src/utils.cc",
				"src/lib/fasttext/src/fasttext.cc",

                "src/lib/node-argument.cc",
                "src/lib/wrapper.cc",

                "src/lib/loadModel.cc",
                "src/lib/predict.cc",
                "src/lib/train.cc",
                "src/lib/nn.cc",

                "src/fasttext.cc"
            ],
            "include_dirs": ["<!(node -e \"require('nan')\")"],
            "cflags": [
                "-std=c++0x",
                "-pthread",
                "-Wsign-compare",
                "-fexceptions",
                "-O0"
            ],
            "conditions": [
                [ 'OS!="win"', {
                    "cflags+": [ "-std=c++11", "-fexceptions" ],
                    "cflags_c+": [ "-std=c++11", "-fexceptions" ],
                    "cflags_cc+": [ "-std=c++11", "-fexceptions" ],
                }],
                [ 'OS=="mac"', {
                    "cflags+": [ "-stdlib=libc++" ],
                    "xcode_settings": {
                        "OTHER_CPLUSPLUSFLAGS" : [ "-std=c++11", "-stdlib=libc++", "-pthread" ],
                        "OTHER_LDFLAGS": [ "-stdlib=libc++" ],
                        "MACOSX_DEPLOYMENT_TARGET": "10.7",
                        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                        "CLANG_CXX_LANGUAGE_STANDARD":"c++11",
                        "CLANG_CXX_LIBRARY": "libc++"
                    },
                }]
            ]
        }
    ]
}