//
// LLVM IRを生成するためのクラス
//
#include <fstream>
#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/GlobalVariable.h>

#include "Ast/Node.h"
#include "IRGenerator.h"
#include "IRState.h"


using namespace std;
using namespace llvm;
using namespace expr;



// 構文木からLLVM IRを生成する
//
// 生成に成功した場合、真を返す
bool IRGenerator::generate(Node &ast_root)
{
	// デバッグ用
	// グローバル変数(定数)を生成する
	auto &c = irs.getContext();
	auto &m = irs.getModule();
	auto &builder = irs.getBuilder();
	new GlobalVariable(
			m,
			Type::getInt32Ty(c),
			true,  /* isConstant */
			GlobalValue::PrivateLinkage,
			builder.getInt32(20),  /* Initializer */
			"test_const_20"  /* name */
			);

	// IR生成
	ast_root.getValue(irs);

	// エラーがあった場合、終了する
	const bool errored = verifyModule(m, &errs());
	if (errored)
		return false;

	TheModule = irs.moveModule();

	return !irs.isError();
}


// 生成したModuleを返す
unique_ptr<Module> IRGenerator::get()
{
	return move(TheModule);
}

