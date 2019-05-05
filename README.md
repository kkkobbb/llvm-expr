# exparr
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


## 文法 概要
* 式
    * 代入 `<-` `->`
        * 代入は __数値のみ__ 可能
    * 比較 `==` `!=` `<` `>` `<=` `>=`
    * 加減乗除余 `+` `-` `*` `/` `%`
    * 分岐 `if` `else`
    * 繰り返し `while`
    * 型 `void` `int` `string`
    * 関数宣言 `decl fnc`
    * 関数定義 `fnc`
    * 変数定義 `var` `local var` `global var`
    * エラー強制発生 `compileerr`
* string型
    * string型は関数宣言時の戻り値と引数の型指定用としてのみ利用可能
* 変数名前空間
    * 名前空間の分類は以下の2つのみ
        * `global var` __グローバル__
            * (定義した場所以降の)全ての場所からアクセス可能
            * 関数内でもグローバルとして定義した場合、以降アクセス可能
        * `var` or `local var` __ローカル__
            * 定義した関数内からのみアクセス可能
            * 関数外でローカルとして定義した場合、他の関数内からアクセスできない
            * 新規の変数は指定がなければローカルとなる
* 外部関数呼び出し
    * `int` `char *` `...`の引数をとる関数なら呼び出し可能
        * 正確には、この条件でのみ外部関数宣言を書ける
        * `unsigned` も`int`で一応代用できる
    * 呼び出せる標準ライブラリの関数の例
        * `printf`
        * `atoi`

