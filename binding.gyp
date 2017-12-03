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
            'cflags!': [ '-fno-exceptions', '-Wformat=2', '-Werror', '-Wsign-compare'],
            'cflags_cc!': [ '-fno-exceptions', '-Wformat=2', '-Werror', '-Wsign-compare'],
            'conditions': [
                ['OS=="mac"', {
                    'xcode_settings': {
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                        'MACOSX_DEPLOYMENT_TARGET': '10.9'
                    }
                }]
            ]
        }
    ]
}