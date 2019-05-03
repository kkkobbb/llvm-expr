//
// ノードの処理
//
// 単純な式
//
#include "Expression.h"
#include "IRState.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <iostream>
#include <string>
#include <memory>

using namespace std;
using namespace llvm;
using namespace expr;


// Expression

Expression::Expression(Node *l, Node *r)
	: l(l), r(r)
{}

// IR 生成
// 式
Value *Expression::getValue(IRState &irs)
{
	Value *lv = nullptr;
	if (l != nullptr)
		lv = l->getValue(irs);
	Value *rv = nullptr;
	if (r != nullptr)
		rv = r->getValue(irs);

	return generate_exp(irs, lv, rv);
}

// 演算処理
//
// 演算子毎に変更すること
Value *Expression::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	return nullptr;
}

void Expression::print_ast(ostream &dout, int indent)
{
	Node::print_ast(dout, indent);
	// 子要素の表示
	const int next_indent = indent + 1;
	if (l != nullptr)
		l->print_ast(dout, next_indent);
	if (r != nullptr)
		r->print_ast(dout, next_indent);
}

// ExpressionFunc

ExpressionFunc::ExpressionFunc(Identifier *identifier, NodeList *argumentList)
	: Expression(identifier, argumentList), identifier(identifier), argumentList(argumentList)
{}

// IR 生成
// 関数呼び出し
Value *ExpressionFunc::getValue(IRState &irs)
{
	auto &builder = irs.getBuilder();
	auto &m = irs.getModule();

	const auto name = identifier->getName();
	const auto callee = m.getFunction(*name);
	vector<Value*> args;

	if (argumentList) {
		const auto argList = argumentList->getList();
		if (callee->arg_size() > argList->size())
			return nullptr;  // TODO エラー処理 引数が少ない

		for (auto &arg : *argList)
			args.push_back(arg->getValue(irs));
	}

	return builder.CreateCall(callee, args);
}

// ExpressionAS

ExpressionAS::ExpressionAS(Identifier *identifier, Node *value)
	: Expression(identifier, value), identifier(identifier)
{}

// IR 生成
// 代入
Value *ExpressionAS::getValue(IRState &irs)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();
	auto rhs = r->getValue(irs);

	const auto name = identifier->getName();

	// FIXME 変数がない場合、領域を確保している(ない場合はエラーにする？)
	auto alloca = irs.getVariable(name);
	if (!alloca)
		alloca = builder.CreateAlloca(Type::getInt32Ty(c), 0, *name);

	builder.CreateStore(rhs, alloca);

	return rhs;
}

// 以降、各演算用ノード

// IR 生成
// 論理和
Value *ExpressionLOR::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	// 0か1(0以外の場合)に変換する
	const auto lv1 = builder.CreateIsNotNull(lv, "zero_or_one_tmp");
	const auto rv1 = builder.CreateIsNotNull(rv, "zero_or_one_tmp");

	// 32bit型に変換する
	const auto lv32 = builder.CreateIntCast(lv1, Type::getInt32Ty(c), false);
	const auto rv32 = builder.CreateIntCast(rv1, Type::getInt32Ty(c), false);

	// bit演算or を実行
	return builder.CreateOr(lv32, rv32, "lor_tmp");
}

// IR 生成
// 論理積
Value *ExpressionLAND::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	// 0か1(0以外の場合)に変換する
	const auto lv1 = builder.CreateIsNotNull(lv, "zero_or_one_tmp");
	const auto rv1 = builder.CreateIsNotNull(rv, "zero_or_one_tmp");

	// 32bit型に変換する
	const auto lv32 = builder.CreateIntCast(lv1, Type::getInt32Ty(c), false);
	const auto rv32 = builder.CreateIntCast(rv1, Type::getInt32Ty(c), false);

	// bit演算and を実行
	return builder.CreateAnd(lv32, rv32, "land_tmp");
}

// IR 生成
// ビット演算 or
Value *ExpressionBOR::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateOr(lv, rv, "bor_tmp");
}

// IR 生成
// ビット演算 排他的論理和
Value *ExpressionBXOR::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateXor(lv, rv, "bxor_tmp");
}

// IR 生成
// ビット演算 and
Value *ExpressionBAND::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateAnd(lv, rv, "band_tmp");
}

// IR 生成
// 比較演算 等値
Value *ExpressionEQ::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	const auto cmp = builder.CreateICmpEQ(lv, rv, "eq_tmp");

	// signed i32へ型変換
	return builder.CreateIntCast(cmp, Type::getInt32Ty(c), false);
}

// IR 生成
// 比較演算 不等
Value *ExpressionNE::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	const auto cmp = builder.CreateICmpNE(lv, rv, "ne_tmp");

	// signed i32へ型変換
	return builder.CreateIntCast(cmp, Type::getInt32Ty(c), false);
}

// IR 生成
// 比較演算 <
Value *ExpressionLT::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	const auto cmp = builder.CreateICmpSLT(lv, rv, "lt_tmp");

	// signed i32へ型変換
	return builder.CreateIntCast(cmp, Type::getInt32Ty(c), false);
}

// IR 生成
// 比較演算 >
Value *ExpressionGT::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	const auto cmp = builder.CreateICmpSGT(lv, rv, "gt_tmp");

	// signed i32へ型変換
	return builder.CreateIntCast(cmp, Type::getInt32Ty(c), false);
}

// IR 生成
// 比較演算 <=
Value *ExpressionLTE::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	const auto cmp = builder.CreateICmpSLE(lv, rv, "lte_tmp");

	// signed i32へ型変換
	return builder.CreateIntCast(cmp, Type::getInt32Ty(c), false);
}

// IR 生成
// 比較演算 >=
Value *ExpressionGTE::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &c = irs.getContext();
	auto &builder = irs.getBuilder();

	const auto cmp = builder.CreateICmpSGE(lv, rv, "gte_tmp");

	// signed i32へ型変換
	return builder.CreateIntCast(cmp, Type::getInt32Ty(c), false);
}

// IR 生成
// 加算
Value *ExpressionADD::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateAdd(lv, rv, "add_tmp");
}

// IR 生成
// 減算
Value *ExpressionSUB::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateSub(lv, rv, "sub_tmp");
}

// IR 生成
// 乗算
Value *ExpressionMUL::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateMul(lv, rv, "mul_tmp");
}

// IR 生成
// 除算
Value *ExpressionDIV::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateSDiv(lv, rv, "div_tmp");
}

// IR 生成
// 余算
Value *ExpressionMOD::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateSRem(lv, rv, "mod_tmp");
}

// IR 生成
// 単項演算子 +
Value *ExpressionSPOS::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	return rv;
}

// IR 生成
// 単項演算子 -
Value *ExpressionSNEG::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	auto &builder = irs.getBuilder();

	return builder.CreateNeg(rv, "neg");
}

// IR 生成
// 単項演算子 !
Value *ExpressionLNOT::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	// TODO
	return nullptr;
}

// IR 生成
// 単項演算子 ~
Value *ExpressionBNOT::generate_exp(IRState &irs, Value *lv, Value *rv)
{
	// TODO
	return nullptr;
}

