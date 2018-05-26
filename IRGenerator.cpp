/*
 * IRGenerator
 * LLVM IRを生成するためのクラス
 */
#include <memory>

#include <llvm/IR/Module.h>

#include "Ast.h"
#include "ope.h"
#include "IRGenerator.h"


using namespace std;
using namespace expr;



/*
 * 構文木からLLVM IRを生成する
 *
 * 生成に成功した場合、真を返す
 */
bool IRGenerator::genarate(std::unique_ptr<AstUnit> ast_root)
{
	// TODO

	return false;
}


/*
 * 生成したModuleを返す
 */
unique_ptr<llvm::Module> IRGenerator::get()
{
	// TODO

	return nullptr;
}

