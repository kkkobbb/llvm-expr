# expr(仮)
簡易言語
* LLVM 確認用
* c++でのflex bison 確認用

* 言語名 候補
    * expra (expression with arrow)
    * expar (expression with arrow)
    * arrow
    * exprAllow
* コンパイラ名(仮) `exprc`


## コマンド使い方

* `./exprc srcfile.expr` で`a.bc`ファイルが生成される
    * 中身はLLVM IRのビットコード

### インタプリタ
* `lli a.bc` で実行可能
    * 出力はしないので、`echo $?` とかで戻り値を確認する

### アセンブリ出力 -> コンパイル
* `llc -relocation-model=pic a.bc` で `a.s` が生成される
    * 中身はネイティブのアセンブリコード
    * picじゃないとgccでエラーになる(clangではエラーにならない)
* `gcc a.s` とかで実行ファイル生成して実行可能


## 準備
* Ubuntu 16.04
    * `sudo apt install llvm`
    * `sudo apt install llvm-5.0` (llvm 5.0を使う場合)
    * `sudo apt intall zlib1g-dev` (llvm 3.8のビルドオプションで`-lz`が指定されるため)
* Ubuntu 18.04
    * `sudo apt install llvm` (llvm 6.0)


## 文法
* 式
    * 代入 `<-` `->`
    * 比較 `==` `!=` `<` `>` `<=` `>=`
    * 加減乗除余 `+` `-` `*` `/` `%`
    * 分岐 `if` `else`
    * 繰り返し `while`
    * 関数定義 `fnc`
    * 変数定義 `var`
    * エラー強制発生 `compileerr` `runtimeerr`
* 外部関数呼び出し
    * `int` `char *` `...`の引数をとる関数なら呼び出し可能
        * 上は正確には、外部関数を呼び出すための宣言を書ける条件となる
    * 呼び出せる標準ライブラリの関数の例
        * `putchar`
        * `printf`

