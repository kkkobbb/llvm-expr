//
// ノードの処理
//
// 基本のノード
//
#include <iostream>
#include <string>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/ValueSymbolTable.h>

#include "Node.h"
#include "IRState.h"


using namespace std;
using namespace llvm;
using namespace expr;



// Node

Value *Node::getValue(IRState &irs)
{
	return nullptr;
}


Type *Node::getType(IRState &irs)
{
	return nullptr;
}


void Node::print_ast_string(const char *msg, ostream &dout, int indent)
{
	// インデント
	for (int i = 0; i < indent; ++i)
		dout << "  ";

	dout << msg << endl;
}


void Node::print_ast(ostream &dout, int indent)
{
	string msg = "";

	// クラス名かメッセージ表示
	if (!dbg_msg.empty())
		msg.assign(dbg_msg + " ");
	else
		msg.assign(typeid(*this).name());

	this->print_ast_string(msg.c_str(), dout, indent);
}


