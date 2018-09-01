/*
 * ノードの処理
 *
 * ジャンプ文
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



// AstJumpReturn

AstJumpReturn::AstJumpReturn(AstNode *ret)
{
	this->ret.reset(ret);
}


void AstJumpReturn::print_ast(ostream &dout, int indent)
{
	AstNode::print_ast(dout, indent);
	// 子要素の表示
	int next_indent = indent + 1;
	if (ret != nullptr)
		ret->print_ast(dout, next_indent);
	else
		this->print_ast_string("null", dout, next_indent);
}


/*
 * IR 生成
 *
 * return文
 */
Value *AstJumpReturn::getValue(IRState &irs)
{
	auto &builder = irs.getBuilder();

	Value *retV = nullptr;
	if(this->ret == nullptr) {
		retV = builder.CreateRetVoid();
	} else {
		auto retval = this->ret->getValue(irs);
		retV = builder.CreateRet(retval);
	}

	return retV;
}


