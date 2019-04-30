//
// ノードの処理
//
// 関数の宣言
//
#include "Declaration.h"
#include "IRState.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <iostream>
#include <string>
#include <memory>

using namespace std;
using namespace llvm;
using namespace expr;


// DeclarationFunc

DeclarationFunc::DeclarationFunc(Identifier *decl, IdentifierList *argumentList, bool vararg)
	: decl(decl), argumentList(argumentList), vararg(vararg)
{}

void DeclarationFunc::print_ast(std::ostream &dout, int indent)
{
	Node::print_ast(dout, indent);
	// 子要素の表示
	const int next_indent = indent + 1;
	if (decl != nullptr)
		decl->print_ast(dout, next_indent);
	if (argumentList != nullptr)
		argumentList->print_ast(dout, next_indent);
}

// IR 生成
//
// 関数宣言
//
// TODO エラー処理
Value *DeclarationFunc::getValue(IRState &irs)
{
	auto &m = irs.getModule();
	auto &builder = irs.getBuilder();

	FunctionType *funcType;

	const auto retType = decl->getType(irs);
	if (argumentList) {
		// 引数あり関数
		auto argTypes = argumentList->getTypes(irs);
		funcType = FunctionType::get(retType, *argTypes, vararg);
	} else {
		// 引数なし関数
		funcType = FunctionType::get(retType, vararg);
	}

	const auto name = decl->getName();
	Function::Create(funcType, Function::ExternalLinkage, *name, &m);

	// 最後にverifyModule()を実行するのでここで関数のチェックをしない

	return builder.getInt32(0);
}

