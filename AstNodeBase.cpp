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
#include <llvm/IR/Verifier.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>

#include "AstNode.h"
#include "IRGenInfo.h"


using namespace std;
using namespace llvm;
using namespace expr;



/*
 * nameという変数の領域を探して返す
 *
 * retval = getVariable(igi);
 * if (retval) {} で偽の場合、変数がなかった
 *
 * 1. 引数に対象の変数があるか
 * 2. 現在の関数のスコープに対象の変数があるか
 * 3. グローバルに対象の変数があるか
 */
Value *expr::getVariable(IRGenInfo &igi, const string *name)
{
	auto &m = igi.getModule();
	auto curFunc = igi.getCurFunc();

	auto vs_table = curFunc->getValueSymbolTable();
	auto alloca = vs_table->lookup(*name);

	if (!alloca) {
		auto &global_vs_table = m.getValueSymbolTable();
		alloca = global_vs_table.lookup(*name);
	}

	return alloca;
}


// AstNode

Value *AstNode::getValue(IRGenInfo &igi)
{
	return nullptr;
}


Type *AstNode::getType(IRGenInfo &igi)
{
	return nullptr;
}


void AstNode::print_ast(std::ostream &dout, int indent)
{
	// インデント
	for (int i = 0; i < indent; ++i)
		dout << "  ";

	// クラス名かメッセージ表示
	if (!dbg_msg.empty())
		dout << dbg_msg << " ";
	else
		dout << typeid(*this).name();
	dout << std::endl;
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
Value *AstList::getValue(IRGenInfo &igi)
{
	Value *lastVal = nullptr;

	for (auto &child : children)
		lastVal = child->getValue(igi);

	return lastVal;
}


void AstList::print_ast(std::ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);

	// 子要素の表示
	int next_indent = indent + 1;
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
Value *AstUnit::getValue(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &m = igi.getModule();
	auto &builder = igi.getBuilder();

	// 文法には関数定義はないが、
	// LLVM IRでは関数を定義する必要があるため
	// 便宜上ここで関数mainを定義する
	// (全ての処理は関数mainの処理とする)

	auto func = Function::Create(
			FunctionType::get(Type::getInt32Ty(c), false),
			Function::ExternalLinkage, "main", &m);

	auto *bb = BasicBlock::Create(c, "entry", func);
	builder.SetInsertPoint(bb);

	igi.pushCurFunc(func);

	// 子要素の実行
	auto *v = AstList::getValue(igi);

	if(!v)
		v = builder.getInt32(0);

	builder.CreateRet(v);

	return nullptr;
}


