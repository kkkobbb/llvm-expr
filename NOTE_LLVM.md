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

* `unique_ptr<llvm::Module>` `llvm::LLVMContext`の順番でメンバ変数を宣言すると
  `LLVMContext`内で実行時エラー(`Segmentation fault`)となる
    * 自動変数は宣言の逆順で破棄される
        * <http://cimadai.hateblo.jp/entry/20080422/1208878033>
        * <https://detail.chiebukuro.yahoo.co.jp/qa/question_detail/q1043000740>
        * TODO:  規格ではどこに記載されている？
    * 最初に`LLVMContext`を破棄、次に`Module`を破棄、
      `Module`破棄中に`LLVMContext`内の関数(`LLVMContext::removeModule()`)を呼び出すためエラーとなる?
    * `llvm::LLVMContext` `unique_ptr<llvm::Module>`の順番で宣言するとエラーにならない
        * `Module(LLVMContext, ...)`なコンストラクタなので、この順番が自然ではある?

* cygwinで実行し、生成されたアセンブリをgccでコンパイルすると以下のエラーがでる
    * `Error: invalid register for .seh_pushreg`
        * seh... はintel製品の擬似命令っぽい
        * 例外に関する処理っぽい
            * <http://d.hatena.ne.jp/a4lg/20110111/1294720293>
            * <http://sile.hatenablog.jp/entry/20100206/1265454978>
    * `llc` では`-exception-model=dwarf`を付けると回避できた
        * llvmのライブラリでこれ相当の設定がある？
        * `TargetOptions` クラス
            * `llvm/include/llvm/Target/TargetOptions.h`
        * `ExceptionHandling` enum
            * `llvm/include/llvm/MC/MCTargetOptions.h`

* 出力するファイルの種類の指定方法をllcに合わせる？
    * llc では`-filetype=asm`のように指定する


## LLVM ツール

* `cat a.ll | opt -mem2reg | llvm-dis` で最適化したLLVMコードを出力


## LLVM 3.8

* `llvm-config --system-libs`の内容をビルドオプションに追加する必要がある
  (最低限 -lpthread -ltinfo)

* `llvm-config --system-libs`では-lzが付き、zlib開発版が必要となる
  (インストールは `sudo apt intall zlib1g-dev`)


## LLVM 5.0

* zlib開発版は不要

* `-help`するとデフォルトでたくさんのオプションが表示されるため、それらを非表示にする必要がある


## LLVM 6.0

* `llvm::getGlobalContext()`はなくなっている
    * `llvm::LLVMContext TheContext`とか宣言して、共通のLLVMContextを使用すれば良い
    * <http://llvm.org/docs/tutorial/LangImpl03.html#full-code-listing>


---


## 参考
* <https://llvm.org/docs/CommandLine.html>
* <https://stackoverflow.com/questions/34147464/how-to-avoid-llvms-support-commandline-leaking-library-arguments>


