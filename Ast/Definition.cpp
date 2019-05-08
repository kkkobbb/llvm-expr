//
// ノードの処理
//
// 変数、関数の定義
//
#include "Definition.h"
#include "IRState.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <iostream>
#include <string>
#include <memory>

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

bool DefinitionVar::verifyType(IRState &irs)
{
	auto &c = irs.getContext();
	const auto typeInt32 = Type::getInt32Ty(c);
	const auto name = decl->getName();

	// TODO 他の型でも定義できるようにする
	// 変数定義はint型のみ
	const auto typeVar = decl->getType(irs);
	if (typeVar != typeInt32) {
		auto msg = make_unique<string>("No int type '" + *name + "'");
		irs.setError(move(msg));
		return false;
	}

	return true;
}

// DefinitionVarLocal

// IR 生成
//
// 変数定義
//
Value *DefinitionVarLocal::getValue(IRState &irs)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	if (!verifyType(irs))
		return builder.CreateUnreachable();

	// TODO 他の型でも定義できるようにする

	const auto name = decl->getName();
	const auto alloca = builder.CreateAlloca(Type::getInt32Ty(c), 0, *name);

	// 初期値の指定があれば、設定する
	if (init.get() != nullptr) {
		const auto value = irs.getValueInBlock(init.get());
		builder.CreateStore(value, alloca);
	}

	return alloca;
}

// DefinitionVarGlobal

// IR 生成
//
// 変数定義
//
Value *DefinitionVarGlobal::getValue(IRState &irs)
{
	auto &m = irs.getModule();
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	if (!verifyType(irs))
		return builder.CreateUnreachable();

	// TODO 他の型でも定義できるようにする

	const auto name = decl->getName();
	auto gvar = new GlobalVariable(
			m,
			Type::getInt32Ty(c),
			false,  // isConstant
			GlobalValue::PrivateLinkage,
			builder.getInt32(0),  // Initializer
			name->c_str()
			);

	// 初期値の指定があれば、設定する
	if (init.get() != nullptr) {
		const auto value = irs.getValueInBlock(init.get());
		const auto constVal = dyn_cast_or_null<Constant>(value);
		if (constVal != nullptr)
			gvar->setInitializer(constVal);
	}

	return gvar;
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

	const auto bodyValue = irs.getValueInBlock(body.get(), true);

	// body内でreturn していた場合、ここでreturnを追加しない
	if (!isa<ReturnInst>(bodyValue)) {
		if ((bodyValue == nullptr) || (retType == Type::getVoidTy(c)))
			builder.CreateRetVoid();
		else
			builder.CreateRet(bodyValue);
	}

	// 命令挿入位置を戻す
	builder.SetInsertPoint(oldBb);

	// 最後にverifyModule()を実行するのでここで関数のチェックをしない

	return nullptr;
}

