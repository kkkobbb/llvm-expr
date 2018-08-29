/*
 * OptimGenerator
 * LLVM IRを最適化するためのクラス
 */
#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/Scalar.h>

#include "OptimGenerator.h"


using namespace std;
using namespace llvm;
using namespace expr;



/*
 * 最適化を行ったLLVM IRを生成する
 *
 * 生成に成功した場合、真を返す
 */
bool OptimGenerator::generate(unique_ptr<Module> module)
{
	legacy::PassManager fpm;

	// opt -mem2reg 相当
	fpm.add(createPromoteMemoryToRegisterPass());
	fpm.run(*module);

	TheModule = move(module);

	return true;
}


/*
 * 生成したModuleを返す
 */
unique_ptr<llvm::Module> OptimGenerator::get()
{
	return move(TheModule);
}

