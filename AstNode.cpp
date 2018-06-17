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
 * List
 *
 * 子要素の実行のみ
 */
llvm::Value *AstList::generate(IRGenInfo &igi)
{
	Value *lastVal = nullptr;

	for (auto itr = children.cbegin(); itr != children.cend(); ++itr)
		lastVal = (*itr)->generate(igi);

	return lastVal;
}


/*
 * IR 生成
 * 翻訳単位 (1ファイル分)
 */
Value *AstUnit::generate(IRGenInfo &igi)
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

	igi.curFunc = func;

	// 子要素の実行
	auto *v = AstList::generate(igi);

	builder.CreateRet(v);

	return nullptr;
}


/*
 * IR 生成
 * 文
 *
 * 子要素の実行のみ
 */
Value *AstExpressionUnit::generate(IRGenInfo &igi)
{
	// 子要素の実行
	auto *v = l->generate(igi);

	return v;
}


/*
 * IR 生成
 * 代入
 */
Value *AstExpressionAS::generate(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &builder = igi.getBuilder();
	auto rhs = r->generate(igi);

	auto id = dynamic_cast<AstIdentifier*>(l.get());
	if (!id) {
		// TODO エラー処理
		return nullptr;
	}
	auto &name = id->getName();

	IRBuilder<> fBuilder(&igi.curFunc->getEntryBlock(),
			igi.curFunc->getEntryBlock().begin());
	auto alloca = fBuilder.CreateAlloca(Type::getInt32Ty(c), 0, name);

	builder.CreateStore(rhs, alloca);

	return alloca;
}


/*
 * IR 生成
 * 加算
 */
Value *AstExpressionADD::generate(IRGenInfo &igi)
{
	auto &builder = igi.getBuilder();
	auto lhs = l->generate(igi);
	auto rhs = r->generate(igi);

	return builder.CreateAdd(lhs, rhs, "add_tmp");
}


/*
 * IR 生成
 * 減算
 */
Value *AstExpressionSUB::generate(IRGenInfo &igi)
{
	auto &builder = igi.getBuilder();
	auto lhs = l->generate(igi);
	auto rhs = r->generate(igi);

	return builder.CreateSub(lhs, rhs, "sub_tmp");
}


/*
 * IR 生成
 * 乗算
 */
Value *AstExpressionMUL::generate(IRGenInfo &igi)
{
	auto &builder = igi.getBuilder();
	auto lhs = l->generate(igi);
	auto rhs = r->generate(igi);

	return builder.CreateMul(lhs, rhs, "mul_tmp");
}


/*
 * IR 生成
 * 除算
 */
Value *AstExpressionDIV::generate(IRGenInfo &igi)
{
	auto &builder = igi.getBuilder();
	auto lhs = l->generate(igi);
	auto rhs = r->generate(igi);

	return builder.CreateSDiv(lhs, rhs, "div_tmp");
}


/*
 * IR 生成
 * 余算
 */
Value *AstExpressionMOD::generate(IRGenInfo &igi)
{
	auto &builder = igi.getBuilder();
	auto lhs = l->generate(igi);
	auto rhs = r->generate(igi);

	return builder.CreateSRem(lhs, rhs, "mod_tmp");
}


/*
 * IR 生成
 * 定数 整数
 */
Value *AstConstantInt::generate(IRGenInfo &igi)
{
	auto &builder = igi.getBuilder();

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
	auto &m = igi.getModule();
	auto &builder = igi.getBuilder();

	auto vs_table = igi.curFunc->getValueSymbolTable();
	auto alloca = vs_table->lookup(*name);

	if(!alloca) {
		auto &global_vs_table = m.getValueSymbolTable();
		alloca = global_vs_table.lookup(*name);
	}

	return builder.CreateLoad(alloca, "var");
}

