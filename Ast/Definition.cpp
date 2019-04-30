//
// ノードの処理
//
// 変数、関数の定義
//
#include <iostream>
#include <string>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>

#include "Definition.h"
#include "IRState.h"


using namespace std;
using namespace llvm;
using namespace expr;



// DefinitionVar

DefinitionVar::DefinitionVar(Identifier *decl, Node *init)
	: decl(decl), init(init)
{}


void DefinitionVar::print_ast(ostream &dout, int indent)
{
	Node::print_ast(dout, indent);
	// 子要素の表示
	const int next_indent = indent + 1;
	if (decl != nullptr)
		decl->print_ast(dout, next_indent);
	if (init != nullptr)
		init->print_ast(dout, next_indent);
}


// IR 生成
//
// 変数定義
//
// TODO エラー処理
Value *DefinitionVar::getValue(IRState &irs)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	const auto name = decl->getName();
	const auto alloca = builder.CreateAlloca(Type::getInt32Ty(c), 0, *name);

	if (this->init.get() != nullptr) {
		const auto value = init->getValue(irs);
		builder.CreateStore(value, alloca);
	}
	// TODO 初期値無しの場合、型ごとの初期値

	return alloca;
}


// DefinitionFunc

DefinitionFunc::DefinitionFunc(Identifier *decl, IdentifierList *argumentList, Node *body)
	: decl(decl), argumentList(argumentList), body(body)
{}


void DefinitionFunc::print_ast(std::ostream &dout, int indent)
{
	Node::print_ast(dout, indent);
	// 子要素の表示
	const int next_indent = indent + 1;
	if (decl != nullptr)
		decl->print_ast(dout, next_indent);
	if (argumentList != nullptr)
		argumentList->print_ast(dout, next_indent);
	if (body != nullptr)
		body->print_ast(dout, next_indent);
}


// IR 生成
//
// 関数定義
//
// TODO エラー処理
Value *DefinitionFunc::getValue(IRState &irs)
{
	auto &c = irs.getContext();
	auto &m = irs.getModule();
	auto &builder = irs.getBuilder();

	FunctionType *funcType;

	const auto retType = decl->getType(irs);
	if (argumentList) {
		// 引数有り関数
		const auto argTypes = argumentList->getTypes(irs);
		funcType = FunctionType::get(retType, *argTypes, false);
	} else {
		// 引数無し関数
		funcType = FunctionType::get(retType, false);
	}

	const auto name = decl->getName();
	const auto func = Function::Create(funcType, Function::ExternalLinkage, *name, &m);

	// 現在の関数を更新
	irs.pushCurFunc(func);

	// 命令挿入位置の更新
	BasicBlock *bb = BasicBlock::Create(c, "entry", func);
	const auto oldBb = builder.GetInsertBlock();
	builder.SetInsertPoint(bb);

	// 引数有り関数の場合、各引数の名前、値の変数を作成
	if (argumentList) {
		const auto argNames = argumentList->getNames();
		const auto argTypes = argumentList->getTypes(irs);  // FIXME funcType生成時にも呼び出している
		auto ni = argNames->cbegin();
		auto ti = argTypes->cbegin();
		for (auto ai = func->arg_begin(); ai != func->arg_end(); ++ai, ++ni, ++ti) {
			const auto alloca = builder.CreateAlloca(*ti, 0, **ni);
			builder.CreateStore(ai, alloca);
		}
	}

	const auto bodyValue = this->body->getValue(irs);

	// body内でreturn していた場合、ここでreturnを追加しない
	if (!isa<ReturnInst>(bodyValue)) {
		if ((bodyValue == nullptr) || (retType == Type::getVoidTy(c)))
			builder.CreateRetVoid();
		else
			builder.CreateRet(bodyValue);
	}

	// 命令挿入位置を戻す
	builder.SetInsertPoint(oldBb);
	// 現在の関数を戻す
	irs.popCurFunc();

	// 最後にverifyModule()を実行するのでここで関数のチェックをしない

	return nullptr;
}


