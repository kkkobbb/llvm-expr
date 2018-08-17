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
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>

#include "AstNode.h"
#include "IRGenInfo.h"


using namespace std;
using namespace llvm;
using namespace expr;



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
 * 現在の関数のスコープを確認して変数が存在しない場合、
 * モジュールに登録された変数(global)を確認する
 *
 * 値の参照以外の用途(代入先、関数名など)の場合、getName()呼び出しで
 * 親ノードが処理すること
 */
Value *AstIdentifier::getValue(IRGenInfo &igi)
{
	auto &m = igi.getModule();
	auto &builder = igi.getBuilder();

	auto vs_table = igi.curFunc->getValueSymbolTable();
	auto alloca = vs_table->lookup(*name);

	if (!alloca) {
		auto &global_vs_table = m.getValueSymbolTable();
		alloca = global_vs_table.lookup(*name);
	}

	return builder.CreateLoad(alloca, "var");
}


/*
 * リスト中の識別子から型を全て取り出す
 *
 * 型が設定されていない場合、nullptrが要素となる
 */
unique_ptr<vector<Type*>> AstIdentifierList::getTypes(IRGenInfo &igi)
{
	auto typelist = new vector<Type*>();

	for (auto itr = children.cbegin(); itr != this->children.cend(); ++itr)
	{
		auto child = (*itr).get();
		auto type = child->getType(igi);
		typelist->push_back(type);
	}

	unique_ptr<vector<Type*>> ptr(typelist);

	return move(ptr);
}


/*
 * リスト中の識別子から識別子名を全て取り出す
 */
unique_ptr<vector<string*>> AstIdentifierList::getNames()
{
	auto namelist = new vector<string*>();

	for (auto itr = children.cbegin(); itr != this->children.cend(); ++itr)
	{
		auto child = (*itr).get();
		auto name = child->getName();
		namelist->push_back(&name);
	}

	unique_ptr<vector<string*>> ptr(namelist);

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

	igi.curFunc = func;

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
 * 関数宣言
 */
Value *AstDefinitionFunc::getValue(IRGenInfo &igi)
{
	//auto &c = igi.getContext();
	auto &m = igi.getModule();
	//auto &builder = igi.getBuilder();

	// TODO ASTを見て生成
	auto argTypes = argumentList->getTypes(igi);
	auto retType = decl->getType(igi);
	auto funcType = FunctionType::get(retType, *argTypes, false);
	auto name = decl->getName();

	//auto func = 
	Function::Create(
			funcType,
			Function::ExternalLinkage, name, &m);

	// TODO 宣言時に返す値
	return nullptr;
}


Type *AstTypeInt::getType(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	return llvm::Type::getInt32Ty(c);
}


Type *AstTypeVoid::getType(IRGenInfo &igi)
{
	auto &c = igi.getContext();
	return llvm::Type::getVoidTy(c);
}


/*
 * IR 生成
 * 式
 */
Value *AstExpression::getValue(IRGenInfo &igi)
{
	Value *lv = nullptr;
	if(l != nullptr)
		lv = l->getValue(igi);
	Value *rv = nullptr;
	if(r != nullptr)
		rv = r->getValue(igi);

	return generate_exp(igi, lv, rv);
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

	auto &name = this->identifier->getName();

	IRBuilder<> fBuilder(&igi.curFunc->getEntryBlock(),
			igi.curFunc->getEntryBlock().begin());
	auto alloca = fBuilder.CreateAlloca(Type::getInt32Ty(c), 0, name);

	builder.CreateStore(rhs, alloca);

	return alloca;
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

