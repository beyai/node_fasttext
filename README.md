# Node_FastText
> 带监督的文本分类算法FastText

#### 安装
```js
npm install node_fasttext
```

#### 使用
```js
const fastText = require("node_fasttext");
const FastText = new fastText();

const options = {
    "input" : "./train.txt",
    "output" : "./model",
    "loss" : "softmax",
    "dim" : 200,
    "bucket" : 2000000
}

FastText.train("supervised" ,  options ).then( res => {
    console.log(res);
})
```

# API

### 模型训练 FastText.train( type , options )
> 详细配置参数查看官方 [配置文档](https://github.com/facebookresearch/fastText#full-documentation)

```js
/**
* loadModel
* @param String type 模型类型 [supervised | cbow | skipgram]
* @param Object options 配置参数
* @return Promise 返回模型信息
*/

FastText.train("supervised" ,{
    "input" : "./train.txt",
    "output" : "./model",
}).then( res => {
    console.log(res);
    // { bucket: 2000000,
    //   dim: 100,
    //   epoch: 5,
    //   label: '__label__',
    //   label_count: 2,
    //   loss: 'ns',
    //   lr: 0.05,
    //   lrUpdateRate: 100,
    //   maxn: 6,
    //   minCount: 5,
    //   minCountLabel: 0,
    //   minn: 3,
    //   model: 'cbow',
    //   neg: 5,
    //   pretrainedVectors: '',
    //   t: 0.0001,
    //   thread: 12,
    //   token_count: 1209,
    //   verbose: 2,
    //   wordNgrams: 1,
    //   word_count: 27,
    //   ws: 5 }
})
``` 

### 测试分类 FastText.test( testFile , k )
> 必须先加载 supervised 模型，详细请查看官方 [文档](https://github.com/facebookresearch/fastText#text-classification)

```js
/**
* nn
* @param String testFile 待测试分类的文件
* @param Number k 精度和查全率，默认值为 1
* @return Promise
*/

FastText.test( "./test.txt" , 2 ).then( res => {
    console.log(res)
    // { 'Number of examples': 68, 'P@3': 0.333333, 'R@3': 1 }
})
```


### 压缩模型 FastText.quantize( options )
> 必须先训练模型，output 文件名必须与原模型名相同， 详细请查看官方 [配置文档](https://github.com/facebookresearch/fastText#full-documentation)

```js
/**
* quantize
* @param Object options 配置参数
* @return Promise 返回模型信息
*/
FastText.quantize({
    "input" : "./train.txt",
    "output" : "./model",
}).then( res => {
    console.log(res)
})
```


### 加载模型 FastText.loadModel( file )

```js
/**
* loadModel
* @param String file 模型文件路径
* @return Promise 返回模型信息
*/

FastText.loadModel("./model.bin").then( res => {
    console.log(res);
})
```

### 文本分类 FastText.predict( text , k )
> 模型必须是 supervised，详细请查看官方 [文档](https://github.com/facebookresearch/fastText#text-classification)

```js
/**
* predict
* @param String text 文本内容
* @param Number k 按相似度降序排列，返回 n 个分类，默认值为 1
* @return Promise
*/

FastText.predict("This is Band of Outsiders on Twitter" , 2 ).then( res => {
    console.log(res);
    // [ { label: '__label__BAND', value: 0.49999999904767284 },
    // { label: '__label__ORGANIZATION', value: 0.49804691366168485 } ]
})
```

### 相似词查询 FastText.nn( word , k )
> 模型必须是 cbow 或 skipgram，详细请查看官方 [文档](https://github.com/facebookresearch/fastText/blob/master/tutorials/unsupervised-learning.md#nearest-neighbor-queries)

```js
/**
* nn
* @param String word 词
* @param Number k 按相似度降序排列，返回 n 个相似的词，默认值为 1
* @return Promise
*/

FastText.nn( "word" , 2 ).then( res => {
    console.log(res)
    // [ { word: 'The', value: 1.3640304837694894 },
    // { word: 'you', value: 1.2910933389370642 } ]
})
```
### 计算两个词语的语义距离 FastText.similarity( what , with )
> 模型必须是 cbow 或 skipgram

```js
/**
* similarity
* @param String what 一个词
* @param String with 另一个词
* @return Promise 余弦距离
*/

FastText.analogies( "山东", "江苏" ).then( res => {
    console.log(res)
    // 0.81871825
})
```

### 词语类比查询 FastText.analogies( words , k )
> 模型必须是 cbow 或 skipgram，详细请查看官方 [文档](https://github.com/facebookresearch/fastText/blob/master/tutorials/unsupervised-learning.md#word-analogies)

```js
/**
* analogies
* @param Array words 词语，必须为3个词语
* @param Number k 按相似度降序排列，语义距离最近 n 个相似的词，默认值为 1
* @return Promise 
*/

FastText.analogies( ["berlin","germany","france"] , 2 ).then( res => {
    console.log(res)
    // [ { word: 'paris', value: 0.768954 },
    // { word: 'louveciennes', value: 0.765569 } ]
})
```


### 获取向量 FastText.getVector( text )

```js
/**
* getVector
* @param String text 词语或句子
* @return Promise 
*/

FastText.getVector( "paris").then( res => {
    console.log(res)
    // { text: 'paris', value: [
    //     -0.49160531163215637,
    //     0.08759436011314392,
    //     0.12446501106023788,
    //     -0.21982385218143463,
    //     ...
    //     -0.2687559723854065
    // ]},
})
```



