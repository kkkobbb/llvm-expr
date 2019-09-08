//
// LLVM IRを最適化するためのクラス
//
#include "OptimPass.h"
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/Utils.h>
#include <memory>

using namespace std;
using namespace llvm;
using namespace expr;


// 最適化を行ったLLVM IRを生成する
//
// 生成に成功した場合、真を返す
bool OptimPass::run(Module &module)
{
	legacy::PassManager pm;

	// opt -mem2reg 相当
	pm.add(createPromoteMemoryToRegisterPass());
	pm.run(module);

	return true;
}

