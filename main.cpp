/*
 * LLVM確認用言語 Expr コンパイラ
 */
#include <fstream>

#include <llvm/Support/CommandLine.h>

#include "Lexer.h"
#include "Parser.hh"

#include "Ast.h"
#include "IRGenerator.h"

using namespace std;
using namespace expr;


int main(int argc, char *argv[])
{
	llvm::cl::OptionCategory CompilerCategory(
			"Compiler Options");
	llvm::cl::opt<string> InputFilename(llvm::cl::Positional,
			llvm::cl::desc("<input file>"),
			llvm::cl::Required,
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<string> OutputFilename("o",
			llvm::cl::desc("specify output filename"),
			llvm::cl::value_desc("filename"),
			llvm::cl::cat(CompilerCategory));
	llvm::cl::opt<bool> Force("f",
			llvm::cl::desc("Enable binary output on terminals"),
			llvm::cl::cat(CompilerCategory));
	// CompilerCategory以外は非表示にする
	llvm::cl::HideUnrelatedOptions(CompilerCategory);
	llvm::cl::ParseCommandLineOptions(argc, argv, "tiny LLVM compiler\n");

	ifstream fin;
	fin.open(InputFilename, ios::binary);
	if (!fin) {
		cerr << "Error: open \"" << InputFilename << "\"" << endl;
		return 1;
	}
	Lexer lexer(&fin);

	Parser parser(lexer);
	parser.parse();

	// 構文解析でエラーが発生した場合、終了
	// エラーメッセージは構文解析時に出力される
	if (is_ast_err())
		return 1;

	unique_ptr<AstNode> root(get_ast());
	root->print_debug(cout);  // 各ノードの表示

	return 0;
}

