//
// ノードの処理
//
// 基本のノード
//
#include "NodeList.h"
#include "IRState.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <iostream>
#include <string>
#include <memory>

using namespace std;
using namespace llvm;
using namespace expr;


// NodeList

NodeList::NodeList(Node *n)
{
	add(n);
}

void NodeList::add(Node *n)
{
	if (n == nullptr)
		return;
	children.push_back(unique_ptr<Node>(n));
}

// IR 生成
// List
//
// 子要素の実行のみ
Value *NodeList::getValue(IRState &irs)
{
	Value *lastVal = nullptr;

	for (auto &child : children)
		lastVal = irs.getValueInBlock(child.get());

	return lastVal;
}

void NodeList::print_ast(std::ostream &dout, int indent)
{
	Node::print_ast(dout, indent);

	// 子要素の表示
	const int next_indent = indent + 1;
	for (auto &child : children)
		child->print_ast(dout, next_indent);
}

vector<unique_ptr<Node>> *NodeList::getList()
{
	return &children;
}

// Unit

// IR 生成
// 翻訳単位 (1ファイル分)
Value *Unit::getValue(IRState &irs)
{
	auto &c = irs.getContext();
	auto &m = irs.getModule();
	auto &builder = irs.getBuilder();

	// 文法には関数定義はないが、
	// LLVM IRではmain関数を定義する必要があるため
	// 便宜上ここで関数mainを定義する
	// (全ての処理は関数mainの処理とする)

	const auto func = Function::Create(
			FunctionType::get(Type::getInt32Ty(c), false),
			Function::ExternalLinkage, "main", &m);

	auto *bb = BasicBlock::Create(c, "entry", func);
	builder.SetInsertPoint(bb);

	// 子要素の実行
	auto *v = irs.getValueInBlock<NodeList>(this, true);

	// returnしていない場合、最後の式を戻り値とする
	if (!isa<ReturnInst>(v)) {
		// 値がない、またはvoidを返している場合、0を返す
		if (!v || v->getType() == Type::getVoidTy(c))
			v = builder.getInt32(0);
		builder.CreateRet(v);
	}

	// 最後にverifyModule()を実行するのでここで関数のチェックをしない

	return nullptr;
}

