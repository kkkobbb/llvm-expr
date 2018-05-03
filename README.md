# expr
簡易言語

* LLVM 確認用
* c++でのflex bison 確認用


## 準備
* Ubuntu 16.04
* `sudo apt install llvm`


## 文法
* 式だけ
* 演算子 `<-` `+` `-` `*` `/` `%`


---


## LLVMメモ
* コマンドライン引数解析に`llvm/Support/CommandLine.h`が使える

* ただし、`llvm-config --system-libs`の内容をビルドオプションに追加する必要がある
  (最低限 -lpthread -ltinfo)

* `llvm-config --system-libs`の-lzには、zlibの開発版が必要となる
  (インストールは `sudo apt intall zlib1g-dev`)


### 参考
* <https://llvm.org/docs/CommandLine.html>


