//
// ノードの処理
//
// 基本のノード
//
#include "Node.h"
#include "IRState.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <string>

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

	dout << msg << "\n";
}

void Node::print_ast(ostream &dout, int indent)
{
	string msg = "";

	// クラス名かメッセージ表示
	if (!dbg_msg.empty())
		msg.assign(dbg_msg + " ");
	else
		msg.assign(typeid(*this).name());

	print_ast_string(msg.c_str(), dout, indent);
}

