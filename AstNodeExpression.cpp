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
#include "IRGenInfo.h"


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
Value *AstExpression::getValue(IRGenInfo &igi)
{
	Value *lv = nullptr;
	if (l != nullptr)
		lv = l->getValue(igi);
	Value *rv = nullptr;
	if (r != nullptr)
		rv = r->getValue(igi);

	return generate_exp(igi, lv, rv);
}


/*
 * 演算処理
 *
 * 演算子毎に変更すること
 */
Value *AstExpression::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
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
Value *AstExpressionFunc::getValue(IRGenInfo &igi)
{
	auto &builder = igi.getBuilder();
	auto &m = igi.getModule();

	auto name = this->identifier->getName();
	auto argList = this->argumentList->getList();

	auto callee = m.getFunction(*name);
	if (callee->arg_size() > argList->size())
		return nullptr;  // TODO エラー処理 引数が少ない

	vector<Value*> args;
	for (auto itr = argList->cbegin(); itr != argList->cend(); ++itr)
		args.push_back((*itr)->getValue(igi));

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
Value *AstExpressionAS::getValue(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &builder = igi.getBuilder();
	auto rhs = r->getValue(igi);

	auto name = this->identifier->getName();

	auto alloca = getVariable(igi, name);
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
 * 加算
 */
Value *AstExpressionADD::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
{
	auto &builder = igi.getBuilder();

	return builder.CreateAdd(lv, rv, "add_tmp");
}


/*
 * IR 生成
 * 減算
 */
Value *AstExpressionSUB::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
{
	auto &builder = igi.getBuilder();

	return builder.CreateSub(lv, rv, "sub_tmp");
}


/*
 * IR 生成
 * 乗算
 */
Value *AstExpressionMUL::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
{
	auto &builder = igi.getBuilder();

	return builder.CreateMul(lv, rv, "mul_tmp");
}


/*
 * IR 生成
 * 除算
 */
Value *AstExpressionDIV::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
{
	auto &builder = igi.getBuilder();

	return builder.CreateSDiv(lv, rv, "div_tmp");
}


/*
 * IR 生成
 * 余算
 */
Value *AstExpressionMOD::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
{
	auto &builder = igi.getBuilder();

	return builder.CreateSRem(lv, rv, "mod_tmp");
}

