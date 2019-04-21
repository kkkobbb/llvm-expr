# exparr
簡易言語

* LLVM 確認用
* c++でのflex bison 確認用
* コンパイラ名 `exparrc`
* ソースコード拡張子 `ea` 


## 準備
* 以下のコマンドをビルド時に使用するのでなければインストール
    * `make`
    * `g++`
    * `flex`
    * `bison`
* Ubuntu 16.04
    * llvm 3.8を使う場合 `sudo apt install llvm zlib1g-dev`
        * (ビルドオプションで`-lz`が指定されるためzlibも必要)
    * llvm 5.0を使う場合 `sudo apt install llvm-5.0`
* Ubuntu 18.04
    * `sudo apt install llvm` (llvm 6.0)


## ビルド
* `make`
* 成功すると`exparrc`が生成される


## コマンド使い方
* `./exparrc srcfile.ea` で`a.s`ファイルが生成される
    * 中身はアセンブリコード
    * ubuntu 18.04 では`gcc` `clang`で実行ファイル生成に成功

* `./exparrc -filetype=bc srcfile.ea`で`a.bc`ファイルが生成される
    * 中身はllvmビットコード
    * インタプリタ
        * `lli a.bc` で実行可能
    * コンパイル (picじゃないとgccでエラーになる)
        * `llc -relocation-model=pic a.bc` で `a.s` が生成される


## 文法
* 式
    * 代入 `<-` `->`
    * 比較 `==` `!=` `<` `>` `<=` `>=`
    * 加減乗除余 `+` `-` `*` `/` `%`
    * 分岐 `if` `else`
    * 繰り返し `while`
    * 型 `void` `int` `string`
    * 関数定義 `fnc`
    * 変数定義 `var`
    * エラー強制発生 `compileerr`
* 外部関数呼び出し
    * `int` `char *` `...`の引数をとる関数なら呼び出し可能
        * 正確には、この条件でのみ外部関数宣言を書ける
    * 呼び出せる標準ライブラリの関数の例
        * `putchar`
        * `printf`

