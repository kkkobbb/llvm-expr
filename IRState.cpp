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

const std::vector<unique_ptr<string> > *IRState::getErrorMsgList()
{
	return &errorMstList;
}

// エラーフラグを真にする
void IRState::setError()
{
	errorFlag = true;
}

void IRState::setError(const char *msg)
{
	setError(make_unique<string>(msg));
}

void IRState::setError(unique_ptr<string> msg)
{
	setError();
	errorMstList.push_back(move(msg));
}

LLVMContext &IRState::getContext()
{
	return TheContext;
}

Module &IRState::getModule()
{
	return *TheModule;
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
		gStr = createGlobalString(str);
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

// nameという変数の領域を探して返す
//
// 見つからなかった場合、偽となる値を返す
//
// 1. 現在の関数のスコープに対象の変数があるか
// 2. グローバルに対象の変数があるか
Value *IRState::getVariable(const string *name)
{

	Value *alloca = nullptr;

	// 現在の関数のスコープでの検索
	if (!alloca) {  // 変数の名前空間を制限するため、無駄なif文を付けている
		const auto func = builder->GetInsertBlock()->getParent();
		const auto vs_table = func->getValueSymbolTable();
		alloca = vs_table->lookup(*name);
	}

	// グローバル領域での検索
	if (!alloca) {
		const auto &vs_table = TheModule->getValueSymbolTable();
		alloca = vs_table.lookup(*name);
	}

	return alloca;
}

void IRState::pushBlock(BasicBlock *block)
{
	blockStack.push_back(block);
}

BasicBlock *IRState::popBlock()
{
	if (blockStack.empty())
		return nullptr;

	const auto block = blockStack.back();
	blockStack.pop_back();

	return block;
}

// IR生成中にエラーが発生していた場合、真
bool IRState::isError()
{
	return errorFlag;
}

unique_ptr<Module> IRState::moveModule()
{
	return move(TheModule);
}

