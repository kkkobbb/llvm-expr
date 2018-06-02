/*
 * LLVM確認用言語 Expr コンパイラ
 */
#include <fstream>

#include <llvm/Support/CommandLine.h>

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
	llvm::cl::opt<bool> EmitAst("emit-ast",
			llvm::cl::desc("emit AST"),
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<bool> EmitLlvm("emit-llvm",
			llvm::cl::desc("emit llvm IR"),
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

	AstGenerator astGen;
	if (!astGen.genarate(fin))
		return 1;

	IRGenerator irGen;
	if(!irGen.genarate(astGen.get()))
		return 1;

	// TODO Module受け取り

	// TODO 目的ファイル生成

	return 0;
}

