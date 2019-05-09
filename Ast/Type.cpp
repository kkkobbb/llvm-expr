//
// ノードの処理
//
// 型
//
#include "Type.h"
#include "IRState.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>

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

