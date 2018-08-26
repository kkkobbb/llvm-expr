/*
 * IRState
 * IR出力時の状態保持
 */
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/ValueSymbolTable.h>


#include "IRState.h"


using namespace std;
using namespace llvm;
using namespace expr;



IRState::IRState()
{
	builder.reset(new IRBuilder<>(TheContext));
	TheModule.reset(new Module("code", TheContext));
}


/*
 * IR生成中にエラーが発生していた場合、真
 */
bool IRState::isError()
{
	return errorFlag;
}


LLVMContext &IRState::getContext()
{
	return TheContext;
}


Module &IRState::getModule()
{
	return *TheModule;
}


unique_ptr<Module> IRState::moveModule()
{
	return move(TheModule);
}


IRBuilder<> &IRState::getBuilder()
{
	return *builder;
}


size_t IRState::setValue(Value *val)
{
	ValueList.push_back(val);

	return ValueList.size() - 1;
}


Value *IRState::getValue(int num)
{
	return ValueList[num];
}


Function *IRState::getCurFunc()
{
	return funcStack.back();
}


void IRState::pushCurFunc(Function *func)
{
	funcStack.push_back(func);
}


void IRState::popCurFunc()
{
	funcStack.pop_back();
}


/*
 * nameという変数の領域を探して返す
 *
 * 見つからなかった場合、偽となる値を返す
 *
 * 1. 現在の関数のスコープに対象の変数があるか
 * 2. グローバルに対象の変数があるか
 */
Value *IRState::getVariable(const string *name)
{
	auto curFunc = this->getCurFunc();

	auto vs_table = curFunc->getValueSymbolTable();
	auto alloca = vs_table->lookup(*name);

	if (!alloca) {
		auto &global_vs_table = TheModule->getValueSymbolTable();
		alloca = global_vs_table.lookup(*name);
	}

	return alloca;
}


/*
 * グローバル(関数内)ではない場合、真を返す
 *
 * 内部的に作成するmain関数内の場合、グローバルだとする
 */
bool IRState::isGlobal()
{
	return funcStack.size() <= 1;
}


