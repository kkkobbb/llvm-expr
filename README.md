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
    * `llvm-config`
    * `flex`
    * `bison`
* Ubuntu 16.04
    * `sudo apt install llvm zlib1g-dev` (llvm 3.8のビルドオプションで`-lz`が指定されるためzlibも必要)
    * `sudo apt install llvm-5.0` (llvm 5.0を使う場合)
* Ubuntu 18.04
    * `sudo apt install llvm` (llvm 6.0)


## ビルド
* `make`
* 成功すると`exparrc`が生成される


## コマンド使い方
* `./exparrc srcfile.ea` で`a.s`ファイルが生成される
    * 中身はアセンブリコード
    * ubuntu 18.04 ではgcc clangで実行ファイル生成に成功

* `./exparrc -output-bc srcfile.ea`で`a.bc`ファイルが生成される
    * 中身はllvmビットコード

### インタプリタ
* `lli a.bc` で実行可能
    * 出力はしないので、`echo $?` とかで戻り値を確認する

### アセンブリ出力 -> コンパイル
* `llc -relocation-model=pic a.bc` で `a.s` が生成される
    * 中身はネイティブのアセンブリコード
    * picじゃないとgccでエラーになる(clangではエラーにならない)
* `gcc a.s` とかで実行ファイル生成して実行可能


## 文法
* 式
    * 代入 `<-` `->`
    * 比較 `==` `!=` `<` `>` `<=` `>=`
    * 加減乗除余 `+` `-` `*` `/` `%`
    * 分岐 `if` `else`
    * 繰り返し `while`
    * 関数定義 `fnc`
    * 変数定義 `var`
    * エラー強制発生 `compileerr`
* 外部関数呼び出し
    * `int` `char *` `...`の引数をとる関数なら呼び出し可能
        * 上は正確には、外部関数を呼び出すための宣言を書ける条件となる
    * 呼び出せる標準ライブラリの関数の例
        * `putchar`
        * `printf`

