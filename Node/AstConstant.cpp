/*
 * ノードの処理
 *
 * 定数、識別子
 */
#include <iostream>
#include <string>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>

#include "AstConstant.h"
#include "IRState.h"


using namespace std;
using namespace llvm;
using namespace expr;



// AstConstantInt

AstConstantInt::AstConstantInt(int num)
{
	this->num = num;
	dbg_msg = "(" + to_string(num) + ")";
}


/*
 * IR 生成
 * 定数 整数
 */
Value *AstConstantInt::getValue(IRState &irs)
{
	auto &builder = irs.getBuilder();

	return builder.getInt32(num);
}


// AstConstantString

AstConstantString::AstConstantString(string *str)
{
	if (str == nullptr)
		this->str = llvm::make_unique<string>("");
	else
		this->str = unique_ptr<string>(str);

	dbg_msg = "\"" + *str + "\"";
}


/*
 * IR 生成
 * 定数 文字列
 */
Value *AstConstantString::getValue(IRState &irs)
{
	auto &builder = irs.getBuilder();

	auto gstr = irs.getGlobalString(this->str->c_str());

	// グローバル変数のポインタの取得
	auto constZero = builder.getInt32(0);
	vector<Constant *> index(2, constZero);
	auto strType = gstr->getInitializer()->getType();
	auto strPtr = ConstantExpr::getGetElementPtr(strType, gstr, index);

	return strPtr;
}


// AstIdentifier

AstIdentifier::AstIdentifier(string *name, AstNode *type)
{
	if (name == nullptr)
		this->name = llvm::make_unique<string>("");
	else
		this->name = unique_ptr<string>(name);
	dbg_msg = "\"" + *this->name + "\"";
	this->type = unique_ptr<AstNode>(type);
}


void AstIdentifier::print_ast(ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);
	// 子要素の表示
	int next_indent = indent + 1;
	if (type != nullptr)
		type->print_ast(dout, next_indent);
}


const string *AstIdentifier::getName()
{
	return name.get();
}


Type *AstIdentifier::getType(IRState &irs)
{
	return this->type->getType(irs);
}


/*
 * IR 生成
 *
 * 変数の値を返す
 *
 * 値の参照以外の用途(代入先、関数名など)の場合、getName()呼び出しで
 * 親ノードが処理すること
 */
Value *AstIdentifier::getValue(IRState &irs)
{
	auto &builder = irs.getBuilder();
	auto name = getName();
	auto alloca = irs.getVariable(name);

	return builder.CreateLoad(alloca, "var");
}


// AstIdentifierList

AstIdentifierList::AstIdentifierList(AstIdentifier *n)
{
	add(n);
}


void AstIdentifierList::add(AstIdentifier *n)
{
	if (n == nullptr)
		return;
	children.push_back(unique_ptr<AstIdentifier>(n));
}


void AstIdentifierList::print_ast(ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);

	// 子要素の表示
	int next_indent = indent + 1;
	for (auto &child : children)
		child->print_ast(dout, next_indent);
}


/*
 * リスト中の識別子から型を全て取り出す
 *
 * 型が設定されていない場合、nullptrが要素となる
 * Type自体のメモリはLLVMで管理されているためunique_ptrにしない
 */
unique_ptr<vector<Type*>> AstIdentifierList::getTypes(IRState &irs)
{
	auto typelist = new vector<Type*>();

	for (auto &child : children)
		typelist->push_back(child->getType(irs));

	unique_ptr<vector<Type*>> ptr(typelist);

	return move(ptr);
}


/*
 * リスト中の識別子から識別子名を全て取り出す
 *
 * 識別子名はAstIdentifierで管理されているためunique_ptrにしない
 */
unique_ptr<vector<const string*>> AstIdentifierList::getNames()
{
	auto namelist = new vector<const string*>();

	for (auto &child : children)
		namelist->push_back(child->getName());

	unique_ptr<vector<const string*>> ptr(namelist);

	return move(ptr);
}


