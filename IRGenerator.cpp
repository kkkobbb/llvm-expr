/*
 * IRGenerator
 * LLVM IRを生成するためのクラス
 */
#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "AstNode.h"
#include "IRGenerator.h"


using namespace std;
using namespace expr;



/*
 * 構文木からLLVM IRを生成する
 *
 * 生成に成功した場合、真を返す
 */
bool IRGenerator::genarate(std::unique_ptr<AstNode> ast_root)
{
	llvm::IRBuilder<> builder(TheContext);

	TheModule = llvm::make_unique<llvm::Module>("code", TheContext);

	ast_root->generate(TheModule.get(), builder);

	return true;
}


/*
 * 生成したModuleを返す
 */
unique_ptr<llvm::Module> IRGenerator::get()
{
	return move(TheModule);
}

