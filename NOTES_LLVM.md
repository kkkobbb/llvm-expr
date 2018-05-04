# LLVMメモ

## 全体

* コマンドライン引数解析に`llvm/Support/CommandLine.h`が使える

* `opt<>`の`cl::init()`での指定がない場合の初期値
    * `opt<string>` `""`
    * `opt<bool>` `false`

* clangでも使っている？
    * `llvm::cl::ParseCommandLineOptions()`を使っているコードもある
      (e.g. `clang/tools/driver/cc1as_main.cpp`)
    * `llvm::cl::readConfigFile()`を使っている？
      (e.g. `clang/lib/Driver/Driver.cpp`)


## LLVM 3.8

* `llvm-config --system-libs`の内容をビルドオプションに追加する必要がある
  (最低限 -lpthread -ltinfo)

* `llvm-config --system-libs`では-lzが付き、zlib開発版が必要となる
  (インストールは `sudo apt intall zlib1g-dev`)


## LLVM 5.0

* zlib開発版は不要

* `-help`するとデフォルトでたくさんのオプションが表示されるため、それらを非表示にする必要がある


---


## 参考
* <https://llvm.org/docs/CommandLine.html>
* <https://stackoverflow.com/questions/34147464/how-to-avoid-llvms-support-commandline-leaking-library-arguments>


