
#include <iostream>
#include <string>
#include <memory>

#include "Ast.h"


using namespace std;
using namespace expr;



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

