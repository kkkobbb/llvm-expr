/*
 * ノードの処理
 *
 * 基本のノード
 */
#include <iostream>
#include <string>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>

#include "AstNode.h"
#include "IRState.h"


using namespace std;
using namespace llvm;
using namespace expr;



// AstNode

Value *AstNode::getValue(IRState &irs)
{
	return nullptr;
}


Type *AstNode::getType(IRState &irs)
{
	return nullptr;
}


void AstNode::print_ast_string(const char *msg, ostream &dout, int indent)
{
	// インデント
	for (int i = 0; i < indent; ++i)
		dout << "  ";

	dout << msg << endl;
}


void AstNode::print_ast(ostream &dout, int indent)
{
	string msg = "";

	// クラス名かメッセージ表示
	if (!dbg_msg.empty())
		msg.assign(dbg_msg + " ");
	else
		msg.assign(typeid(*this).name());

	this->print_ast_string(msg.c_str(), dout, indent);
}


// AstList

AstList::AstList(AstNode *n)
{
	add(n);
}


void AstList::add(AstNode *n)
{
	if (n == nullptr)
		return;
	children.push_back(unique_ptr<AstNode>(n));
}


/*
 * IR 生成
 * List
 *
 * 子要素の実行のみ
 */
Value *AstList::getValue(IRState &irs)
{
	Value *lastVal = nullptr;

	for (auto &child : children)
		lastVal = child->getValue(irs);

	return lastVal;
}


void AstList::print_ast(std::ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);

	// 子要素の表示
	const int next_indent = indent + 1;
	for (auto &child : children)
		child->print_ast(dout, next_indent);
}


vector<unique_ptr<AstNode>> *AstList::getList()
{
	return &children;
}


// AstUnit

/*
 * IR 生成
 * 翻訳単位 (1ファイル分)
 */
Value *AstUnit::getValue(IRState &irs)
{
	auto &c = irs.getContext();
	auto &m = irs.getModule();
	auto &builder = irs.getBuilder();

	// 文法には関数定義はないが、
	// LLVM IRではmain関数を定義する必要があるため
	// 便宜上ここで関数mainを定義する
	// (全ての処理は関数mainの処理とする)

	const auto func = Function::Create(
			FunctionType::get(Type::getInt32Ty(c), false),
			Function::ExternalLinkage, "main", &m);

	auto *bb = BasicBlock::Create(c, "entry", func);
	builder.SetInsertPoint(bb);

	irs.pushCurFunc(func);

	// 子要素の実行
	auto *v = AstList::getValue(irs);

	// returnしていない場合、最後の式を戻り値とする
	if (!isa<ReturnInst>(v)) {
		// 値がない、またはvoidを返している場合、0を返す
		if (!v || v->getType() == Type::getVoidTy(c))
			v = builder.getInt32(0);
		builder.CreateRet(v);
	}

	// 最後にverifyModule()を実行するのでここで関数のチェックをしない

	return nullptr;
}


