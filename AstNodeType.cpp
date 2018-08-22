/*
 * ノードの処理
 *
 * 型
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



Type *AstTypeInt::getType(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	return Type::getInt32Ty(c);
}


Type *AstTypeVoid::getType(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	return Type::getVoidTy(c);
}


