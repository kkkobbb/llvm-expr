/*
 * IRGenerator
 * LLVM IRを生成するためのクラス
 */
#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>

#include "AstNode.h"
#include "IRGenerator.h"
#include "IRState.h"


using namespace std;
using namespace expr;



/*
 * 構文木からLLVM IRを生成する
 *
 * 生成に成功した場合、真を返す
 */
bool IRGenerator::genarate(std::unique_ptr<AstNode> ast_root)
{
	// グローバル変数(定数)を生成する
	auto &c = irs.getContext();
	auto &m = irs.getModule();
	auto &builder = irs.getBuilder();
	irs.setValue(new llvm::GlobalVariable(
			m,
			llvm::Type::getInt32Ty(c),
			true,  /* isConstant */
			llvm::GlobalValue::PrivateLinkage,
			builder.getInt32(20),  /* Initializer */
			"test_const_20"  /* name */
			));

	// IR生成
	ast_root->getValue(irs);
	TheModule = irs.moveModule();

	return !irs.isError();
}


/*
 * 生成したModuleを返す
 */
unique_ptr<llvm::Module> IRGenerator::get()
{
	return move(TheModule);
}

