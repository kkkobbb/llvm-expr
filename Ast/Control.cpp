//
// ノードの処理
//
// 制御文
//
#include "Control.h"
#include "IRState.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>

using namespace std;
using namespace llvm;
using namespace expr;


// ControlIf

ControlIf::ControlIf(Node *cond, Node *proc, Node *elseProc)
	: cond(cond), proc(proc), elseProc(elseProc)
{}

void ControlIf::print_ast(ostream &dout, int indent)
{
	Node::print_ast(dout, indent);
	// 子要素の表示
	int next_indent = indent + 1;
	if (cond != nullptr)
		cond->print_ast(dout, next_indent);
	if (proc != nullptr)
		proc->print_ast(dout, next_indent);
	else
		print_ast_string("null", dout, next_indent);
	if (elseProc != nullptr)
		elseProc->print_ast(dout, next_indent);
}

// IR 生成
//
// if文
//
// TODO エラー処理
Value *ControlIf::getValue(IRState &irs)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	auto condV = irs.getValueInBlock(cond.get());
	auto cmp = builder.CreateIsNotNull(condV);

	auto curFunc = builder.GetInsertBlock()->getParent();
	auto thenBB = BasicBlock::Create(c, "then", curFunc);
	auto elseBB = BasicBlock::Create(c, "else");
	auto retBB = BasicBlock::Create(c, "ifret");

	// 分岐の数
	int brnum = 0;

	Value *thenV = nullptr;
	Value *elseV = nullptr;

	// 比較
	builder.CreateCondBr(cmp, thenBB, elseBB);

	// then
	builder.SetInsertPoint(thenBB);
	if (proc) {
		thenV = irs.getValueInBlock(proc.get(), true);
		++brnum;
	}
	builder.CreateBr(retBB);  // 分岐終了地点へジャンプ
	// blockを更新
	thenBB = builder.GetInsertBlock();

	// else
	curFunc->getBasicBlockList().push_back(elseBB);
	builder.SetInsertPoint(elseBB);
	if (elseProc) {
		elseV = irs.getValueInBlock(elseProc.get(), true);
		++brnum;
	}
	builder.CreateBr(retBB);  // 分岐終了地点へジャンプ
	// blockを更新
	elseBB = builder.GetInsertBlock();

	// 分岐終了地点
	curFunc->getBasicBlockList().push_back(retBB);
	builder.SetInsertPoint(retBB);

	// phi関数
	if (brnum > 1) {
		auto type = Type::getInt32Ty(c);
		auto phi = builder.CreatePHI(type, 2);
		phi->addIncoming(thenV, thenBB);
		phi->addIncoming(elseV, elseBB);
		return phi;
	}

	auto constZero = builder.getInt32(0);
	return constZero;
}

// ControlWhile

ControlWhile::ControlWhile(Node *cond, Node *proc)
	: cond(cond), proc(proc)
{}


void ControlWhile::print_ast(ostream &dout, int indent)
{
	Node::print_ast(dout, indent);
	// 子要素の表示
	int next_indent = indent + 1;
	if (cond != nullptr)
		cond->print_ast(dout, next_indent);
	if (proc != nullptr)
		proc->print_ast(dout, next_indent);
	else
		print_ast_string("null", dout, next_indent);
}

// IR 生成
//
// while文
//
// TODO エラー処理
Value *ControlWhile::getValue(IRState &irs)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	auto curFunc = builder.GetInsertBlock()->getParent();
	auto loopBB = BasicBlock::Create(c, "loop");
	auto bodyBB = BasicBlock::Create(c, "loopbody");
	auto endBB = BasicBlock::Create(c, "loopend");

	builder.CreateBr(loopBB);  // ループ始点へジャンプ

	// ループ始点
	curFunc->getBasicBlockList().push_back(loopBB);
	builder.SetInsertPoint(loopBB);

	auto condV = irs.getValueInBlock(cond.get(), true);
	auto constZero = builder.getInt32(0);
	auto cmp = builder.CreateICmpNE(condV, constZero);

	// 比較
	builder.CreateCondBr(cmp, bodyBB, endBB);

	// ループ内処理
	curFunc->getBasicBlockList().push_back(bodyBB);
	builder.SetInsertPoint(bodyBB);
	irs.getValueInBlock(proc.get(), true);
	builder.CreateBr(loopBB);  // ループ始点へジャンプ

	// ループ終端
	curFunc->getBasicBlockList().push_back(endBB);
	builder.SetInsertPoint(endBB);

	// TODO ループ内の値を返す
	return constZero;
}

