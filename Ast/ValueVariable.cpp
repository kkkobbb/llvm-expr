//
// ノードの処理
// 定数、識別子
//
#include "ValueVariable.h"
#include "IRState.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <iostream>
#include <string>
#include <memory>

using namespace std;
using namespace llvm;
using namespace expr;


// Identifier

Identifier::Identifier(string *name, Node *type)
{
	if (name == nullptr)
		this->name = llvm::make_unique<string>("");
	else
		this->name = unique_ptr<string>(name);
	dbg_msg = "\"" + *this->name + "\"";
	this->type = unique_ptr<Node>(type);
}

void Identifier::print_ast(ostream &dout, int indent)
{
	Node::print_ast(dout, indent);
	// 子要素の表示
	const int next_indent = indent + 1;
	if (type != nullptr)
		type->print_ast(dout, next_indent);
}

const string *Identifier::getName()
{
	return name.get();
}

Type *Identifier::getType(IRState &irs)
{
	auto &c = irs.getContext();

	// 型の指定がない場合、void型とみなす
	if (type == nullptr)
		return Type::getVoidTy(c);

	return type->getType(irs);
}

// IR 生成
//
// 変数の値を返す
//
// 値の参照以外の用途(代入先、関数名など)の場合、getName()呼び出しで
// 親ノードが処理すること
Value *Identifier::getValue(IRState &irs)
{
	auto &builder = irs.getBuilder();
	const auto name = getName();
	const auto alloca = irs.getVariable(name);

	if (!alloca) {
		auto msg = make_unique<string>("Not found variable '" + *name + "'");
		irs.setError(move(msg));
		// 検査時にエラーとなる要素を返す
		return builder.CreateUnreachable();
	}

	return builder.CreateLoad(alloca, "var");
}

// IdentifierList

IdentifierList::IdentifierList(Identifier *n)
{
	add(n);
}

void IdentifierList::add(Identifier *n)
{
	if (n == nullptr)
		return;
	children.push_back(unique_ptr<Identifier>(n));
}

void IdentifierList::print_ast(ostream &dout, int indent)
{
	Node::print_ast(dout, indent);

	// 子要素の表示
	const int next_indent = indent + 1;
	for (auto &child : children)
		child->print_ast(dout, next_indent);
}

// リスト中の識別子から型を全て取り出す
//
// 型が設定されていない場合、nullptrが要素となる
// Type自体のメモリはLLVMで管理されているためunique_ptrにしない
unique_ptr<vector<Type*>> IdentifierList::getTypes(IRState &irs)
{
	const auto typelist = new vector<Type*>();

	for (auto &child : children)
		typelist->push_back(child->getType(irs));

	unique_ptr<vector<Type*>> ptr(typelist);

	return move(ptr);
}

// リスト中の識別子から識別子名を全て取り出す
//
// 識別子名はIdentifierで管理されているためunique_ptrにしない
unique_ptr<vector<const string*>> IdentifierList::getNames()
{
	const auto namelist = new vector<const string*>();

	for (auto &child : children)
		namelist->push_back(child->getName());

	unique_ptr<vector<const string*>> ptr(namelist);

	return move(ptr);
}

