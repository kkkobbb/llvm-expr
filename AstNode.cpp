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

#include "AstNode.h"


using namespace std;
using namespace expr;
using namespace llvm;



/*
 * IR 生成
 */
llvm::Value *AstList::generate(llvm::Module *m, llvm::IRBuilder<> &builder)
{
	Value *lastVal = nullptr;

	// 子要素の実行
	for (auto itr = children.cbegin(); itr != children.cend(); ++itr)
		lastVal = (*itr)->generate(m, builder);

	return lastVal;
}


/*
 * IR 生成
 */
Value *AstUnit::generate(Module *m, IRBuilder<> &builder)
{
	LLVMContext &c = builder.getContext();

	// 文法には関数定義はないが、
	// LLVM IRでは関数を定義する必要があるため
	// 便宜上ここで関数mainを定義する
	// (全ての処理は関数mainの処理とする)

	Function *func = Function::Create(
			FunctionType::get(Type::getInt32Ty(c), false),
			Function::ExternalLinkage, "main", m);

	BasicBlock *bb = BasicBlock::Create(c, "entry", func);
	builder.SetInsertPoint(bb);

	// 子要素の実行
	Value *v = AstList::generate(m, builder);

	builder.CreateRet(v);

	return nullptr;
}


/*
 * IR 生成
 */
Value *AstStatement::generate(Module *m, IRBuilder<> &builder)
{
	Value *v = n->generate(m, builder);

	return v;
}


/*
 * IR 生成
 */
Value *AstExpressionADD::generate(Module *m, IRBuilder<> &builder)
{
	Value *lhs = l->generate(m, builder);
	Value *rhs = r->generate(m, builder);

	return builder.CreateAdd(lhs, rhs, "add_tmp");
}


/*
 * IR 生成
 */
Value *AstConstantInt::generate(Module *m, IRBuilder<> &builder)
{
	LLVMContext &c = builder.getContext();

	// Builderからの定数取得
	//return builder.getInt32(value);

	return ConstantInt::get(c, APInt(sizeof(int) * 8, value, true));
}


/*
 * IR 生成
 */
Value *AstIdentifier::generate(Module *m, IRBuilder<> &builder)
{
	// TODO 名前管理

	return nullptr;
}

