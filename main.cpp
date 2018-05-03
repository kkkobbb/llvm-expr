/*
 * flex c++
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
	llvm::cl::ParseCommandLineOptions(argc, argv);



	ifstream fin;
	fin.open("sample.expr", ios::binary);
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

