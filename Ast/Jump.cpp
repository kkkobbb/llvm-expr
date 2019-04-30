//
// ノードの処理
//
// ジャンプ文
//
#include "Jump.h"
#include "IRState.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <iostream>
#include <string>
#include <memory>

using namespace std;
using namespace llvm;
using namespace expr;


// JumpReturn

JumpReturn::JumpReturn(Node *ret)
	: ret(ret)
{}

void JumpReturn::print_ast(ostream &dout, int indent)
{
	Node::print_ast(dout, indent);
	// 子要素の表示
	const int next_indent = indent + 1;
	if (ret != nullptr)
		ret->print_ast(dout, next_indent);
	else
		this->print_ast_string("null", dout, next_indent);
}

// IR 生成
//
// return文
Value *JumpReturn::getValue(IRState &irs)
{
	auto &builder = irs.getBuilder();

	Value *retV = nullptr;
	if (this->ret == nullptr) {
		retV = builder.CreateRetVoid();
	} else {
		const auto retval = this->ret->getValue(irs);
		retV = builder.CreateRet(retval);
	}

	return retV;
}

