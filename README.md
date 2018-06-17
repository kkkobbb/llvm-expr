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
* 演算子 `<-` `+` `-` `*` `/` `%` `if` `while`
* 特殊な定数 `through`
    * `through`を代入する処理はなかったことになる(エラーにはならない)
    * `through`を処理する代入以外の演算は`through`を返す
    * `through`が演算対象になった時点で評価はしなくなる
    * __注意__ 現在の`through`の処理は、厳密に上記の処理を行っていない
        * 単に`through`の式を無視している
        * `if`とかで正しく処理できないはず

