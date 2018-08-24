/*
 * ノードの処理
 *
 * 制御文
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



// AstControlIf

AstControlIf::AstControlIf(AstNode *cond, AstNode *proc, AstNode *elseProc)
{
	this->cond.reset(cond);
	this->proc.reset(proc);
	this->elseProc.reset(elseProc);
}


void AstControlIf::print_ast(ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);
	// 子要素の表示
	int next_indent = indent + 1;
	if (cond != nullptr)
		cond->print_ast(dout, next_indent);
	if (proc != nullptr)
		proc->print_ast(dout, next_indent);
	else
		dout << endl;
	if (elseProc != nullptr)
		elseProc->print_ast(dout, next_indent);
}


/*
 * IR 生成
 *
 * if文
 *
 * TODO エラー処理
 */
Value *AstControlIf::getValue(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &builder = igi.getBuilder();

	auto cond = this->cond->getValue(igi);
	auto constZero = builder.getInt32(0);
	auto cmp = builder.CreateICmpNE(cond, constZero);

	auto curFunc = builder.GetInsertBlock()->getParent();
	auto thenBB = BasicBlock::Create(c, "then", curFunc);
	auto elseBB = BasicBlock::Create(c, "else");
	auto retBB = BasicBlock::Create(c, "ifret");

	// 比較
	builder.CreateCondBr(cmp, thenBB, elseBB);
	builder.SetInsertPoint(thenBB);

	// then
	auto thenV = this->proc->getValue(igi);
	builder.CreateBr(retBB);  // 分岐終了地点へジャンプ
	// 現在のblockを更新
	thenBB = builder.GetInsertBlock();

	// else
	curFunc->getBasicBlockList().push_back(elseBB);
	builder.SetInsertPoint(elseBB);
	auto elseV = this->elseProc->getValue(igi);
	builder.CreateBr(retBB);  // 分岐終了地点へジャンプ
	// 現在のblockを更新
	elseBB = builder.GetInsertBlock();

	// 分岐終了地点
	curFunc->getBasicBlockList().push_back(retBB);
	builder.SetInsertPoint(retBB);
	auto type = Type::getInt32Ty(c);

	// phi関数
	auto phi = builder.CreatePHI(type, 2);
	phi->addIncoming(thenV, thenBB);
	phi->addIncoming(elseV, elseBB);

	return phi;
}


// AstControlWhile

AstControlWhile::AstControlWhile(AstNode *cond, AstNode *proc)
{
	this->cond.reset(cond);
	this->proc.reset(proc);
}


void AstControlWhile::print_ast(ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);
	// 子要素の表示
	int next_indent = indent + 1;
	if (cond != nullptr)
		cond->print_ast(dout, next_indent);
	if (proc != nullptr)
		proc->print_ast(dout, next_indent);
	else
		dout << endl;
}


/*
 * IR 生成
 *
 * while文
 *
 * TODO エラー処理
 */
Value *AstControlWhile::getValue(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &builder = igi.getBuilder();




	return nullptr;
}

