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
#include "IRGenInfo.h"


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
	llvm::LLVMContext &c = igi.getContext();
	llvm::Module &m = igi.getModule();
	llvm::IRBuilder<> &builder = igi.getBuilder();
	auto gv = unique_ptr<llvm::GlobalVariable>(new llvm::GlobalVariable(
			m,
			llvm::Type::getInt32Ty(c),
			true,  /* isConstant */
			llvm::GlobalValue::PrivateLinkage,
			builder.getInt32(20),  /* Initializer */
			"test"  /* name */
			));
	globalList.push_back(move(gv));

	// IR生成
	ast_root->generate(igi);
	TheModule = igi.moveModule();

	return !igi.errorFlag;
}


/*
 * 生成したModuleを返す
 */
unique_ptr<llvm::Module> IRGenerator::get()
{
	return move(TheModule);
}

