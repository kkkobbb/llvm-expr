# The Exparr Programming Language
簡易言語

* LLVM 確認用
* c++でのflex bison 確認用
* コンパイラ名 `exparrc`
* ソースコード拡張子 `ea` 


## 準備
* ビルドに使用するコマンドのインストール
    * `sudo apt install make g++ flex bison`
* Ubuntu 16.04
    * llvm 3.8を使う場合 `sudo apt install llvm zlib1g-dev`
        * (ビルドオプションで`-lz`が指定されるためzlibも必要)
    * llvm 5.0を使う場合 `sudo apt install llvm-5.0`
* Ubuntu 18.04
    * `sudo apt install llvm` (llvm 6.0)


## ビルド
* `make`
    * 成功すると`exparrc`が生成される


## コマンド使い方 概要
* `./exparrc srcfile.ea` で`a.s`ファイルが生成される
    * 中身はアセンブリコード
    * `gcc a.s` `clang a.s`で実行ファイル生成

* `./exparrc -filetype=bc srcfile.ea`で`a.bc`ファイルが生成される
    * 中身はllvmビットコード
    * インタプリタ
        * `lli a.bc` で実行可能
    * コンパイル (picじゃないとgccでエラーになる)
        * `llc -relocation-model=pic a.bc` で `a.s` が生成される

