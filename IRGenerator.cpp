//
// LLVM IRを生成するためのクラス
//
#include "IRGenerator.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <iostream>
#include <memory>

using namespace std;
using namespace llvm;
using namespace expr;


// 構文木からLLVM IRを生成する
//
// 生成に成功した場合、真を返す
bool IRGenerator::generate(Node &ast_root)
{
	// IR生成
	irs.getValueInBlock(&ast_root);

	// llvmの検査に失敗した場合、エラーとする
	auto &m = irs.getModule();
	const bool errored = verifyModule(m, &errs());
	if (errored)
		irs.setError("Verification failed (llvm IR)");

	// エラーがあった場合、エラーメッセージを表示する
	if (irs.isError()) {
		const auto errList = irs.getErrorMsgList();

		cerr << "\nERROR : IR generate\n";
		for (auto itr = errList->cbegin(); itr != errList->cend(); itr++) {
			const string *msg = (*itr).get();
			cerr << "  " << *msg << "\n";
		}
	}

	return !irs.isError();
}

// 生成したModuleを返す
Module &IRGenerator::get()
{
	return irs.getModule();
}

