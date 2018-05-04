# LLVMメモ

* コマンドライン引数解析に`llvm/Support/CommandLine.h`が使える

* ただし、`llvm-config --system-libs`の内容をビルドオプションに追加する必要がある
  (最低限 -lpthread -ltinfo)

* `llvm-config --system-libs`の-lzには、zlibの開発版が必要となる
  (インストールは `sudo apt intall zlib1g-dev`)

* `opt<>`の`cl::init()`での指定がない場合の初期値
    * `opt<string>` `""`
    * `opt<bool>` `false`

* clangでも使っている？
    * `llvm::cl::ParseCommandLineOptions()`を使っているコードもある
      (e.g. `clang/tools/driver/cc1as_main.cpp`)
    * `llvm::cl::readConfigFile()`を使っている？
      (e.g. `clang/lib/Driver/Driver.cpp`)


## 参考
* <https://llvm.org/docs/CommandLine.html>


