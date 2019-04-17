/*
 * ノードの処理
 *
 * 関数の宣言
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

#include "AstDeclaration.h"
#include "IRState.h"


using namespace std;
using namespace llvm;
using namespace expr;



// AstDeclarationFunc

AstDeclarationFunc::AstDeclarationFunc(AstIdentifier *decl, AstIdentifierList *argumentList, bool vararg)
	: decl(decl), argumentList(argumentList), vararg(vararg)
{}


void AstDeclarationFunc::print_ast(std::ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);
	// 子要素の表示
	int next_indent = indent + 1;
	if (decl != nullptr)
		decl->print_ast(dout, next_indent);
	if (argumentList != nullptr)
		argumentList->print_ast(dout, next_indent);
}


/*
 * IR 生成
 *
 * 関数宣言
 *
 * TODO エラー処理
 */
Value *AstDeclarationFunc::getValue(IRState &irs)
{
	auto &m = irs.getModule();
	auto &builder = irs.getBuilder();

	auto argTypes = argumentList->getTypes(irs);
	auto argNames = argumentList->getNames();
	auto retType = decl->getType(irs);
	auto funcType = FunctionType::get(retType, *argTypes, vararg);

	auto name = decl->getName();
	Function::Create(funcType, Function::ExternalLinkage, *name, &m);

	// 最後にverifyModule()を実行するのでここで関数のチェックをしない

	return builder.getInt32(0);
}


