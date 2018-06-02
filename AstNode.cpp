/*
 * 各ノードの処理
 */
#include <iostream>
#include <string>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>

#include "AstNode.h"


using namespace std;
using namespace expr;
using namespace llvm;




/*
 * IR 生成
 */
Value *AstConstantInt::generate(IRBuilder<> &builder)
{
	LLVMContext &c = builder.getContext();

	return ConstantInt::get(c, APInt(sizeof(int) * 8, value, true));
}

