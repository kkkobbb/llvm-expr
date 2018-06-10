/*
 * 各ノードの処理
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
#include "IRGenInfo.h"


using namespace std;
using namespace llvm;
using namespace expr;



/*
 * IR 生成
 */
llvm::Value *AstList::generate(IRGenInfo &igi)
{
	Value *lastVal = nullptr;

	// 子要素の実行
	for (auto itr = children.cbegin(); itr != children.cend(); ++itr)
		lastVal = (*itr)->generate(igi);

	return lastVal;
}


/*
 * IR 生成
 */
Value *AstUnit::generate(IRGenInfo &igi)
{
	LLVMContext &c = igi.getContext();
	Module &m = igi.getModule();
	IRBuilder<> &builder = igi.getBuilder();

	// 文法には関数定義はないが、
	// LLVM IRでは関数を定義する必要があるため
	// 便宜上ここで関数mainを定義する
	// (全ての処理は関数mainの処理とする)

	Function *func = Function::Create(
			FunctionType::get(Type::getInt32Ty(c), false),
			Function::ExternalLinkage, "main", &m);

	BasicBlock *bb = BasicBlock::Create(c, "entry", func);
	builder.SetInsertPoint(bb);

	igi.curFunc = func;

	// 子要素の実行
	Value *v = AstList::generate(igi);

	builder.CreateRet(v);

	return nullptr;
}


/*
 * IR 生成
 */
Value *AstStatement::generate(IRGenInfo &igi)
{
	Value *v = n->generate(igi);

	return v;
}


/*
 * IR 生成
 */
Value *AstExpressionAS::generate(IRGenInfo &igi)
{
	LLVMContext &c = igi.getContext();
	IRBuilder<> &builder = igi.getBuilder();
	Value *rhs = r->generate(igi);

	AstIdentifier *id = dynamic_cast<AstIdentifier*>(l.get());
	if (!id) {
		// TODO エラー処理
		return nullptr;
	}
	const string &name = id->getName();

	IRBuilder<> fBuilder(&igi.curFunc->getEntryBlock(),
			igi.curFunc->getEntryBlock().begin());
	AllocaInst *alloca = fBuilder.CreateAlloca(Type::getInt32Ty(c), 0, name);

	builder.CreateStore(rhs, alloca);

	return alloca;
}


/*
 * IR 生成
 */
Value *AstExpressionADD::generate(IRGenInfo &igi)
{
	IRBuilder<> &builder = igi.getBuilder();
	Value *lhs = l->generate(igi);
	Value *rhs = r->generate(igi);

	return builder.CreateAdd(lhs, rhs, "add_tmp");
}


/*
 * IR 生成
 */
Value *AstConstantInt::generate(IRGenInfo &igi)
{
	IRBuilder<> &builder = igi.getBuilder();

	return builder.getInt32(num);
}


/*
 * IR 生成
 *
 * 変数の値を返す
 * 現在の関数のスコープを確認して変数が存在しない場合、
 * モジュールに登録された変数(global)を確認する
 *
 * 値の参照以外の用途(代入先、関数名など)の場合、getName()呼び出しで
 * 親ノードが処理すること
 */
Value *AstIdentifier::generate(IRGenInfo &igi)
{
	Module &m = igi.getModule();
	IRBuilder<> &builder = igi.getBuilder();

	ValueSymbolTable *vs_table = igi.curFunc->getValueSymbolTable();
	Value *alloca = vs_table->lookup(*name);

	if(!alloca) {
		ValueSymbolTable &global_vs_table = m.getValueSymbolTable();
		alloca = global_vs_table.lookup(*name);
	}

	return builder.CreateLoad(alloca, "var");
}

