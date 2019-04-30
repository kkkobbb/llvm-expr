//
// IR出力時の状態保持
//
#include "IRState.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/GlobalVariable.h>
#include <memory>
#include <vector>

using namespace std;
using namespace llvm;
using namespace expr;


IRState::IRState()
{
	builder = llvm::make_unique<IRBuilder<> >(TheContext);
	TheModule = llvm::make_unique<Module>("code", TheContext);
}

// IR生成中にエラーが発生していた場合、真
bool IRState::isError()
{
	return errorFlag;
}

// エラーフラグを真にする
void IRState::setError()
{
	errorFlag = true;
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

// 文字列のグローバル変数を作成して返す
//
// 既に同じ文字列が生成されていた場合、それを返す
GlobalVariable *IRState::getGlobalString(const char *str)
{
	// 今まで同じ文字列のグローバル変数を作成したか確認する
	string newStr = str;
	newStr.append(1, 0);  // ConstantDataArray::getString()で末尾に追加があるため
	GlobalVariable *gStr = nullptr;
	for (auto gval : GlobalStrList) {
		if (!isa<GlobalVariable>(*gval))
			continue;
		const auto glo = (GlobalVariable *)gval;
		const auto val = glo->getInitializer();
		if (!isa<ConstantDataArray>(*val))
			continue;
		const auto arrayData = (ConstantDataArray *)val;
		if (!arrayData->isString())
			continue;
		const auto existStr = arrayData->getAsString();
		if (newStr == existStr.str()) {
			gStr = glo;
			break;
		}
	}

	if (gStr == nullptr) {
		gStr = this->createGlobalString(str);
		GlobalStrList.push_back(gStr);
	}

	return gStr;
}

// 文字列のグローバル変数を作成する
GlobalVariable *IRState::createGlobalString(const char *str)
{
	const auto strValue = ConstantDataArray::getString(TheContext, str);
	const auto strType = strValue->getType();
	const auto gvar = new GlobalVariable(
			*TheModule,
			strType,
			true,
			GlobalValue::PrivateLinkage,
			strValue,
			".str");
	gvar->setAlignment(1);

	return gvar;
}

[[deprecated("please use llvm::IRBuilder<>::GetInsertBlock()->getParent()")]]
// TODO 削除? funcStackはIRStateでしか使用しない？
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

// nameという変数の領域を探して返す
//
// 見つからなかった場合、偽となる値を返す
//
// 1. 現在の関数のスコープに対象の変数があるか
// 2. グローバルに対象の変数があるか
Value *IRState::getVariable(const string *name)
{
	const auto curFunc = builder->GetInsertBlock()->getParent();

	const auto vs_table = curFunc->getValueSymbolTable();
	auto alloca = vs_table->lookup(*name);

	if (!alloca) {
		const auto &global_vs_table = TheModule->getValueSymbolTable();
		alloca = global_vs_table.lookup(*name);
	}

	return alloca;
}

// グローバル(関数内)ではない場合、真を返す
//
// 内部的に作成するmain関数内の場合、グローバルだとする
bool IRState::isGlobal()
{
	return funcStack.size() <= 1;
}

