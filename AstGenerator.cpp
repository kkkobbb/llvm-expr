/*
 * AstGenerator
 * 構文木を生成するためのクラス
 */
#include <fstream>
#include <memory>

#include "Lexer.h"
#include "Parser.hh"

#include "AstNode.h"
#include "AstGenerator.h"


using namespace std;
using namespace expr;



/*
 * 入力ファイルから構文木を生成する
 *
 * 生成に成功した場合、真を返す
 */
bool AstGenerator::generate(ifstream &fin)
{
	Lexer lexer(&fin);
	Parser parser(lexer);

	parser.parse();

	// 構文解析でエラーが発生した場合、終了
	// エラーメッセージは構文解析時に出力される
	if (is_parse_err())
		return false;

	ast_root = get_ast();

	return true;
}


/*
 * 生成した構文木を返す
 */
unique_ptr<AstNode> AstGenerator::get()
{
	return move(ast_root);
}

