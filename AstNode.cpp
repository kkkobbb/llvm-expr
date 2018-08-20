/*
 * 各ノードの処理
 *
 * dynamic_castしているところは
 * 構文解析処理でクラスが確定している部分のみ
 */
#include <iostream>
#include <string>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>

#include "AstNode.h"
#include "IRGenInfo.h"


using namespace std;
using namespace llvm;
using namespace expr;



/*
 * nameという変数の領域を探して返す
 *
 * retval = getVariable(igi);
 * if (retval) {} で偽の場合、変数がなかった
 *
 * 1. 引数に対象の変数があるか
 * 2. 現在の関数のスコープに対象の変数があるか
 * 3. グローバルに対象の変数があるか
 */
Value *getVariable(IRGenInfo &igi, const string *name)
{
	auto &m = igi.getModule();
	auto curFunc = igi.getCurFunc();

	auto vs_table = curFunc->getValueSymbolTable();
	auto alloca = vs_table->lookup(*name);

	if (!alloca) {
		auto &global_vs_table = m.getValueSymbolTable();
		alloca = global_vs_table.lookup(*name);
	}

	return alloca;
}


/*
 * IR 生成
 * List
 *
 * 子要素の実行のみ
 */
llvm::Value *AstList::getValue(IRGenInfo &igi)
{
	Value *lastVal = nullptr;

	for (auto itr = children.cbegin(); itr != children.cend(); ++itr)
		lastVal = (*itr)->getValue(igi);

	return lastVal;
}


/*
 * IR 生成
 * 定数 整数
 */
Value *AstConstantInt::getValue(IRGenInfo &igi)
{
	auto &builder = igi.getBuilder();

	return builder.getInt32(num);
}


/*
 * IR 生成
 *
 * 変数の値を返す
 *
 * 値の参照以外の用途(代入先、関数名など)の場合、getName()呼び出しで
 * 親ノードが処理すること
 */
Value *AstIdentifier::getValue(IRGenInfo &igi)
{
	auto &builder = igi.getBuilder();
	auto name = getName();
	auto alloca = getVariable(igi, name);

	return builder.CreateLoad(alloca, "var");
}


/*
 * リスト中の識別子から型を全て取り出す
 *
 * 型が設定されていない場合、nullptrが要素となる
 * Type自体のメモリはLLVMで管理されているためunique_ptrにしない
 */
unique_ptr<vector<Type*>> AstIdentifierList::getTypes(IRGenInfo &igi)
{
	auto typelist = new vector<Type*>();

	for (auto itr = children.cbegin(); itr != this->children.cend(); ++itr) {
		auto child = (*itr).get();
		auto type = child->getType(igi);
		typelist->push_back(type);
	}

	unique_ptr<vector<Type*>> ptr(typelist);

	return move(ptr);
}


/*
 * リスト中の識別子から識別子名を全て取り出す
 *
 * 識別子名はAstIdentifierで管理されているためunique_ptrにしない
 */
unique_ptr<vector<const string*>> AstIdentifierList::getNames()
{
	auto namelist = new vector<const string*>();

	for (auto itr = children.cbegin(); itr != this->children.cend(); ++itr) {
		auto child = (*itr).get();
		auto name = child->getName();
		namelist->push_back(name);
	}

	unique_ptr<vector<const string*>> ptr(namelist);

	return move(ptr);
}


/*
 * IR 生成
 * 翻訳単位 (1ファイル分)
 */
Value *AstUnit::getValue(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &m = igi.getModule();
	auto &builder = igi.getBuilder();

	// 文法には関数定義はないが、
	// LLVM IRでは関数を定義する必要があるため
	// 便宜上ここで関数mainを定義する
	// (全ての処理は関数mainの処理とする)

	auto func = Function::Create(
			FunctionType::get(Type::getInt32Ty(c), false),
			Function::ExternalLinkage, "main", &m);

	auto *bb = BasicBlock::Create(c, "entry", func);
	builder.SetInsertPoint(bb);

	igi.pushCurFunc(func);

	// 子要素の実行
	auto *v = AstList::getValue(igi);

	if(!v)
		v = builder.getInt32(0);

	builder.CreateRet(v);

	return nullptr;
}


/*
 * IR 生成
 *
 * 変数定義
 *
 * TODO エラー処理
 */
Value *AstDefinitionVar::getValue(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &builder = igi.getBuilder();

	auto name = decl->getName();
	auto alloca = builder.CreateAlloca(Type::getInt32Ty(c), 0, *name);

	if (this->init.get() != nullptr) {
		auto value = init->getValue(igi);
		builder.CreateStore(value, alloca);
	}
	// TODO 初期値無しの場合、型ごとの初期値

	return alloca;
}


/*
 * IR 生成
 *
 * 関数定義
 *
 * TODO エラー処理
 */
Value *AstDefinitionFunc::getValue(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &m = igi.getModule();
	auto &builder = igi.getBuilder();

	auto argTypes = argumentList->getTypes(igi);
	auto argNames = argumentList->getNames();
	auto retType = decl->getType(igi);
	auto funcType = FunctionType::get(retType, *argTypes, false);
	auto name = decl->getName();

	auto func = Function::Create(funcType, Function::ExternalLinkage, *name, &m);

	// 現在の関数を更新
	igi.pushCurFunc(func);

	// 命令挿入位置の更新
	BasicBlock *bb = BasicBlock::Create(c, "entry", func);
	auto oldBb = builder.GetInsertBlock();
	builder.SetInsertPoint(bb);

	// 各引数の名前、値の変数を作成
	auto ni = argNames->cbegin();
	auto ti = argTypes->cbegin();
	for (auto ai = func->arg_begin(); ai != func->arg_end(); ++ai, ++ni) {
		auto alloca = builder.CreateAlloca(*ti, 0, **ni);
		builder.CreateStore(ai, alloca);
	}

	auto bodyValue = this->body->getValue(igi);
	if (bodyValue == nullptr)
		return nullptr;

	if (retType == Type::getVoidTy(c))
		builder.CreateRetVoid();
	else
		builder.CreateRet(bodyValue);

	// 命令挿入位置を戻す
	builder.SetInsertPoint(oldBb);
	// 現在の関数を戻す
	igi.popCurFunc();

	verifyFunction(*func, &errs());

	return nullptr;
}


Type *AstTypeInt::getType(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	return Type::getInt32Ty(c);
}


Type *AstTypeVoid::getType(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	return Type::getVoidTy(c);
}


/*
 * IR 生成
 * 式
 */
Value *AstExpression::getValue(IRGenInfo &igi)
{
	Value *lv = nullptr;
	if (l != nullptr)
		lv = l->getValue(igi);
	Value *rv = nullptr;
	if (r != nullptr)
		rv = r->getValue(igi);

	return generate_exp(igi, lv, rv);
}


/*
 * IR 生成
 * 関数呼び出し
 */
Value *AstExpressionFunc::getValue(IRGenInfo &igi)
{
	auto &builder = igi.getBuilder();
	auto &m = igi.getModule();

	auto name = this->identifier->getName();
	auto argList = this->argumentList->getList();

	auto callee = m.getFunction(*name);
	if (callee->arg_size() > argList->size())
		return nullptr;  // TODO エラー処理 引数が少ない

	vector<Value*> args;
	for (auto itr = argList->cbegin(); itr != argList->cend(); ++itr)
		args.push_back((*itr)->getValue(igi));

	return builder.CreateCall(callee, args);
}


/*
 * IR 生成
 * 代入
 */
Value *AstExpressionAS::getValue(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	auto &builder = igi.getBuilder();
	auto rhs = r->getValue(igi);

	auto name = this->identifier->getName();

	auto alloca = getVariable(igi, name);
	// FIXME 変数がない場合、領域を確保している
	//       ない場合はエラーにする？
	if (!alloca)
		alloca = builder.CreateAlloca(Type::getInt32Ty(c), 0, *name);

	builder.CreateStore(rhs, alloca);

	return rhs;
}


/*
 * IR 生成
 * 加算
 */
Value *AstExpressionADD::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
{
	auto &builder = igi.getBuilder();

	return builder.CreateAdd(lv, rv, "add_tmp");
}


/*
 * IR 生成
 * 減算
 */
Value *AstExpressionSUB::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
{
	auto &builder = igi.getBuilder();

	return builder.CreateSub(lv, rv, "sub_tmp");
}


/*
 * IR 生成
 * 乗算
 */
Value *AstExpressionMUL::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
{
	auto &builder = igi.getBuilder();

	return builder.CreateMul(lv, rv, "mul_tmp");
}


/*
 * IR 生成
 * 除算
 */
Value *AstExpressionDIV::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
{
	auto &builder = igi.getBuilder();

	return builder.CreateSDiv(lv, rv, "div_tmp");
}


/*
 * IR 生成
 * 余算
 */
Value *AstExpressionMOD::generate_exp(IRGenInfo &igi, Value *lv, Value *rv)
{
	auto &builder = igi.getBuilder();

	return builder.CreateSRem(lv, rv, "mod_tmp");
}

