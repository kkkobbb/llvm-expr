//
// ノードの処理
// 定数、識別子
//
#include "ValueConst.h"
#include "IRState.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <string>
#include <memory>

using namespace std;
using namespace llvm;
using namespace expr;


// ConstInt

ConstInt::ConstInt(int num)
	: num(num)
{
	dbg_msg = "(" + to_string(num) + ")";
}

// IR 生成
// 定数 整数
Value *ConstInt::getValue(IRState &irs)
{
	auto &builder = irs.getBuilder();

	return builder.getInt32(num);
}

// ConstString

ConstString::ConstString(string *str)
{
	if (str == nullptr)
		this->str = llvm::make_unique<string>("");
	else
		this->str = unique_ptr<string>(str);

	dbg_msg = "\"" + *str + "\"";
}

// IR 生成
// 定数 文字列
Value *ConstString::getValue(IRState &irs)
{
	auto &builder = irs.getBuilder();

	const auto gstr = irs.getGlobalString(str->c_str());

	// グローバル変数のポインタの取得
	const auto constZero = builder.getInt32(0);
	vector<Constant *> index(2, constZero);
	const auto strType = gstr->getInitializer()->getType();
	const auto strPtr = ConstantExpr::getGetElementPtr(strType, gstr, index);

	return strPtr;
}

