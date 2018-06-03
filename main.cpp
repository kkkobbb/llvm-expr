/*
 * LLVM確認用言語 Expr コンパイラ
 */
#include <fstream>

#include <llvm/IR/Module.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

#include "AstNode.h"
#include "AstGenerator.h"
#include "IRGenerator.h"

using namespace std;
using namespace expr;



int main(int argc, char *argv[])
{
	// コマンドライン引数の設定
	llvm::cl::OptionCategory CompilerCategory("Compiler Options");
	llvm::cl::opt<string> InputFilename(llvm::cl::Positional,
			llvm::cl::desc("<input file>"),
			llvm::cl::Required,
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<string> OutputFilename("o",
			llvm::cl::desc("specify output filename"),
			llvm::cl::value_desc("filename"),
			llvm::cl::init("a.out"),
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<bool> Force("f",
			llvm::cl::desc("Enable binary output on terminals"),
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<bool> PrintAst("print-ast",
			llvm::cl::desc("print AST"),
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<bool> PrintLlvm("print-llvm",
			llvm::cl::desc("print llvm IR"),
			llvm::cl::cat(CompilerCategory));
	// CompilerCategory以外は非表示
	llvm::cl::HideUnrelatedOptions({&CompilerCategory});
	llvm::cl::ParseCommandLineOptions(argc, argv, "tiny LLVM compiler\n");

	ifstream fin;
	fin.open(InputFilename, ios::binary);
	if (!fin) {
		cerr << "Error: open \"" << InputFilename << "\"" << endl;
		return 1;
	}

	// AST生成
	AstGenerator astGen;
	if (!astGen.genarate(fin))
		return 1;
	auto ast = astGen.get();

	if (PrintAst) {
		ast->print_debug(cout);
		cout << endl;
	}

	// IR生成
	IRGenerator irGen;
	if (!irGen.genarate(move(ast)))
		return 1;
	auto m = irGen.get();
	// FIXME ファイル名の設定 必要？
	m->setSourceFileName(InputFilename);

	if (PrintLlvm) {
		m->print(llvm::errs(), nullptr);
		llvm::errs() << "\n";
	}

	// TODO 目的ファイル生成

	return 0;
}

