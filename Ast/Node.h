#ifndef NODE_H
#define NODE_H

#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <iostream>
#include <string>
#include <memory>


namespace expr {
	class IRState;

	// ノード
	class Node
	{
	protected:
		std::string dbg_msg;

	public:
		virtual llvm::Value *getValue(IRState &irs);
		virtual llvm::Type *getType(IRState &irs);
		void print_ast_string(const char *msg, std::ostream &dout, int indent = 0);
		virtual void print_ast(std::ostream &dout, int indent = 0);
	};
}

#endif  // NODE_H

