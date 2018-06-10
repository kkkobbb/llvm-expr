# expr
簡易言語

* コンパイラ名 `exprc`
* LLVM 確認用
* c++でのflex bison 確認用


## コマンド使い方

* `./exprc srcfile.expr` で`a.ll`ファイルが生成される
    * 中身はLLVM IRのビットコード

### JIT
* `lli a.ll` で実行可能
    * 出力はしないので、`echo $?` とかで戻り値を確認する

### アセンブリ出力 -> コンパイル
* `llc a.ll` で `a.s` が生成される
    * 中身はネイティブのアセンブリコード
* `gcc a.s` とかで実行ファイル生成して実行可能


## 準備
* Ubuntu 16.04
    * `sudo apt install llvm`
    * `sudo apt install llvm-5.0` (llvm 5.0を使う場合)
    * `sudo apt intall zlib1g-dev` (llvm 3.8のビルドオプションで`-lz`が指定されるため)
* Ubuntu 18.04
    * `sudo apt install llvm` (llvm 6.0)


## 文法
* 式だけ
* 演算子 `<-` `+` `-` `*` `/` `%`


