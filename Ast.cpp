
#include <iostream>
#include <string>
#include <memory>

#include "Ast.h"


using namespace std;
using namespace expr;


static std::unique_ptr<AstNode> ast_root;
static bool error_flag = false;
static int error_count = 0;



void AstList::print_debug(ostream &dout, int indent)
{
	// AstList自体の表示の場合、自身の情報を表示
	if (typeid(*this) == typeid(AstList)) {
		AstNode::print_debug(dout, indent);
		dout << "AstList" << endl;
	}

	// 子要素の表示
	int next_indent = indent + 1;
	for (auto itr = children.cbegin(); itr != children.cend(); ++itr)
		(*itr)->print_debug(dout, next_indent);
}


void AstUnit::print_debug(ostream &dout, int indent)
{
	AstNode::print_debug(dout, indent);
	dout << "AstUnit" << endl;

	AstList::print_debug(dout, indent);
}


void AstStatementList::print_debug(ostream &dout, int indent)
{
	AstNode::print_debug(dout, indent);
	dout << "AstStatementList" << endl;

	AstList::print_debug(dout, indent);
}


void AstStatement::print_debug(ostream &dout, int indent)
{
	AstNode::print_debug(dout, indent);
	dout << "AstStatement" << endl;

	n->print_debug(dout, indent + 1);
}


void AstExpression::print_debug(ostream &dout, int indent)
{
	// AstExpression自体の表示の場合、自身の情報を表示
	if (typeid(*this) == typeid(AstExpression)) {
		AstNode::print_debug(dout, indent);
		dout << "expr(" << ope << ")" << endl;
	}

	// 子要素の表示
	int next_indent = indent + 1;
	if (l != nullptr)
		l->print_debug(dout, next_indent);
	if (r != nullptr)
		r->print_debug(dout, next_indent);
}


void AstPrefixExpression::print_debug(ostream &dout, int indent)
{
	AstNode::print_debug(dout, indent);
	dout << "prefix expr(" << ope << ")" << endl;

	AstExpression::print_debug(dout, indent);
}


void AstPostfixExpression::print_debug(ostream &dout, int indent)
{
	AstNode::print_debug(dout, indent);
	dout << "postfix expr(" << ope << ")" << endl;

	AstExpression::print_debug(dout, indent);
}


void AstConstant::print_debug(ostream &dout, int indent)
{
	AstNode::print_debug(dout, indent);
	dout << number << endl;
}


void AstIdentifier::print_debug(ostream &dout, int indent)
{
	AstNode::print_debug(dout, indent);
	dout << "\"" << *name << "\"" << endl;
}




// global関数

/*
 * エラー発生時に印を付ける
 */
void expr::add_err()
{
	error_flag = true;
	++error_count;
}

/*
 * 構文木のルートを設定する
 *
 * 構文解析時に1度だけ使用する
 */
void expr::set_ast(AstNode *root)
{
	ast_root.reset(root);
}

/*
 * 構文木のルートを取得する
 */
std::unique_ptr<AstNode> expr::get_ast()
{
	return std::move(ast_root);
}

/*
 * 構文木でエラーがあったか確認する
 *
 * エラーがあった場合、真を返す
 */
bool expr::is_ast_err()
{
	return error_flag;
}

