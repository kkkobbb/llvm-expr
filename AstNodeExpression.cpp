/*
 * ノードの処理
 *
 * 単純な式
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
#include "IRState.h"


using namespace std;
using namespace llvm;
using namespace expr;



// AstExpression

AstExpression::AstExpression(AstNode *l, AstNode *r)
{
	this->l.reset(l);
	this->r.reset(r);
}


/*
 * IR 生成
 * 式
 */
Value *AstExpression::getValue(IRState &irs)
{
	Value *lv = nullptr;
	if (l != nullptr)
		lv = l->getValue(irs);
	Value *rv = nullptr;
	if (r != nullptr)
		rv = r->getValue(irs);

	return generate_exp(irs, lv, rv);
}


/*
 * 演算処理
 *
 * 演算子毎に変更すること
 */
Value *AstExpression::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	return nullptr;
}


void AstExpression::print_ast(ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);
	// 子要素の表示
	int next_indent = indent + 1;
	if (l != nullptr)
		l->print_ast(dout, next_indent);
	if (r != nullptr)
		r->print_ast(dout, next_indent);
}


// AstExpressionFunc

AstExpressionFunc::AstExpressionFunc(AstIdentifier *identifier, AstList *argumentList)
	: AstExpression(identifier, argumentList) {
		this->identifier = identifier;
		this->argumentList = argumentList;
}


/*
 * IR 生成
 * 関数呼び出し
 */
Value *AstExpressionFunc::getValue(IRState &irs)
{
	auto &builder = irs.getBuilder();
	auto &m = irs.getModule();

	auto name = this->identifier->getName();
	auto argList = this->argumentList->getList();

	auto callee = m.getFunction(*name);
	if (callee->arg_size() > argList->size())
		return nullptr;  // TODO エラー処理 引数が少ない

	vector<Value*> args;
	for (auto &arg : *argList)
		args.push_back(arg->getValue(irs));

	return builder.CreateCall(callee, args);
}


// AstExpressionAS

AstExpressionAS::AstExpressionAS(AstIdentifier *identifier, AstNode *value)
	: AstExpression(identifier, value) {
		this->identifier = identifier;
}


/*
 * IR 生成
 * 代入
 */
Value *AstExpressionAS::getValue(IRState &irs)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();
	auto rhs = r->getValue(irs);

	auto name = this->identifier->getName();

	auto alloca = irs.getVariable(name);
	// FIXME 変数がない場合、領域を確保している
	//       ない場合はエラーにする？
	if (!alloca)
		alloca = builder.CreateAlloca(Type::getInt32Ty(c), 0, *name);

	builder.CreateStore(rhs, alloca);

	return rhs;
}



// 以降、各演算用ノード


/*
 * IR 生成
 * <
 */
Value *AstExpressionLT::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	auto cmp = builder.CreateICmpSLT(lv, rv, "lt_tmp");

	// signed i32へ型変換
	return builder.CreateIntCast(cmp, Type::getInt32Ty(c), false);
}


/*
 * IR 生成
 * 加算
 */
Value *AstExpressionADD::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateAdd(lv, rv, "add_tmp");
}


/*
 * IR 生成
 * 減算
 */
Value *AstExpressionSUB::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateSub(lv, rv, "sub_tmp");
}


/*
 * IR 生成
 * 乗算
 */
Value *AstExpressionMUL::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateMul(lv, rv, "mul_tmp");
}


/*
 * IR 生成
 * 除算
 */
Value *AstExpressionDIV::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateSDiv(lv, rv, "div_tmp");
}


/*
 * IR 生成
 * 余算
 */
Value *AstExpressionMOD::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateSRem(lv, rv, "mod_tmp");
}

