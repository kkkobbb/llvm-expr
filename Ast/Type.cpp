//
// ノードの処理
//
// 型
//
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

#include "Type.h"
#include "IRState.h"


using namespace std;
using namespace llvm;
using namespace expr;



Type *TypeVoid::getType(IRState &irs)
{
	auto &c = irs.getContext();
	return Type::getVoidTy(c);
}


Type *TypeInt::getType(IRState &irs)
{
	auto &c = irs.getContext();
	return Type::getInt32Ty(c);
}


Type *TypeString::getType(IRState &irs)
{
	auto &builder = irs.getBuilder();
	return builder.getInt8PtrTy();
}


