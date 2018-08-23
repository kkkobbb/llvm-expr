/*
 * ノードの処理
 *
 * 変数、関数の定義
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



// AstDefinitionVar

AstDefinitionVar::AstDefinitionVar(AstIdentifier *decl, AstNode *init)
{
	this->decl.reset(decl);
	this->init.reset(init);
}


void AstDefinitionVar::print_ast(ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);
	// 子要素の表示
	int next_indent = indent + 1;
	if (decl != nullptr)
		decl->print_ast(dout, next_indent);
	if (init != nullptr)
		init->print_ast(dout, next_indent);
}


/*
 * IR 生成
 *
 * 変数定義
 *
 * TODO エラー処理
 */
Value *AstDefinitionVar::getValue(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &builder = igi.getBuilder();

	auto name = decl->getName();
	auto alloca = builder.CreateAlloca(Type::getInt32Ty(c), 0, *name);

	if (this->init.get() != nullptr) {
		auto value = init->getValue(igi);
		builder.CreateStore(value, alloca);
	}
	// TODO 初期値無しの場合、型ごとの初期値

	return alloca;
}


// AstDefinitionFunc

AstDefinitionFunc::AstDefinitionFunc(AstIdentifier *decl, AstIdentifierList *argumentList, AstNode *body)
{
	this->decl.reset(decl);
	this->argumentList.reset(argumentList);
	this->body.reset(body);
}


void AstDefinitionFunc::print_ast(std::ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);
	// 子要素の表示
	int next_indent = indent + 1;
	if (decl != nullptr)
		decl->print_ast(dout, next_indent);
	if (argumentList != nullptr)
		argumentList->print_ast(dout, next_indent);
	if (body != nullptr)
		body->print_ast(dout, next_indent);
}


/*
 * IR 生成
 *
 * 関数定義
 *
 * TODO エラー処理
 */
Value *AstDefinitionFunc::getValue(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &m = igi.getModule();
	auto &builder = igi.getBuilder();

	auto argTypes = argumentList->getTypes(igi);
	auto argNames = argumentList->getNames();
	auto retType = decl->getType(igi);
	auto funcType = FunctionType::get(retType, *argTypes, false);
	auto name = decl->getName();

	auto func = Function::Create(funcType, Function::ExternalLinkage, *name, &m);

	// 現在の関数を更新
	igi.pushCurFunc(func);

	// 命令挿入位置の更新
	BasicBlock *bb = BasicBlock::Create(c, "entry", func);
	auto oldBb = builder.GetInsertBlock();
	builder.SetInsertPoint(bb);

	// 各引数の名前、値の変数を作成
	auto ni = argNames->cbegin();
	auto ti = argTypes->cbegin();
	for (auto ai = func->arg_begin(); ai != func->arg_end(); ++ai, ++ni, ++ti) {
		auto alloca = builder.CreateAlloca(*ti, 0, **ni);
		builder.CreateStore(ai, alloca);
	}

	auto bodyValue = this->body->getValue(igi);
	if (bodyValue == nullptr)
		return nullptr;

	if (retType == Type::getVoidTy(c))
		builder.CreateRetVoid();
	else
		builder.CreateRet(bodyValue);

	// 命令挿入位置を戻す
	builder.SetInsertPoint(oldBb);
	// 現在の関数を戻す
	igi.popCurFunc();

	verifyFunction(*func, &errs());

	return nullptr;
}


