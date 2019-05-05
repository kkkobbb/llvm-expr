//
// LLVM IRを生成するためのクラス
//
#include "IRGenerator.h"
#include "IRState.h"
#include "Ast/Node.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/GlobalVariable.h>
#include <iostream>
#include <fstream>
#include <memory>

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
			true,  // isConstant
			GlobalValue::PrivateLinkage,
			builder.getInt32(20),  // Initializer
			"test_const_20"  // name
			);

	// IR生成
	ast_root.getValue(irs);

	if (irs.isError()) {
		auto errList = irs.getErrorMsgList();

		cerr << "ERROR : IR generate\n";
		for (auto itr = errList->cbegin(); itr != errList->cend(); itr++) {
			string *msg = (*itr).get();
			cerr << "  " << *msg << "\n";
		}
		cerr << "\n";
	}

	// エラーがあった場合、フラグを設定する
	const bool errored = verifyModule(m, &errs());
	if (errored)
		irs.setError();

	TheModule = irs.moveModule();

	return !irs.isError();
}

// 生成したModuleを返す
unique_ptr<Module> IRGenerator::get()
{
	return move(TheModule);
}

