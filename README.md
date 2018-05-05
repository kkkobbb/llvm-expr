# expr
簡易言語

* コンパイラ名 `exprc`
* LLVM 確認用
* c++でのflex bison 確認用


## 準備
* Ubuntu 16.04
* `sudo apt install llvm`
* `sudo apt install llvm-5.0` (llvm 5.0を使う場合)
* `sudo apt intall zlib1g-dev` (llvm 3.8のビルドオプションで`-lz`が指定されるため)


## 文法
* 式だけ
* 演算子 `<-` `+` `-` `*` `/` `%`


