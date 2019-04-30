#ifndef CONTROL_H
#define CONTROL_H

#include "Node.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>


namespace expr {
	class IRState;

	class ControlIf: public Node
	{
		protected:
		std::unique_ptr<Node> cond;
		std::unique_ptr<Node> proc;
		std::unique_ptr<Node> elseProc;

		public:
		ControlIf(Node *cond, Node *proc, Node *elseProc);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	class ControlWhile: public Node
	{
		protected:
		std::unique_ptr<Node> cond;
		std::unique_ptr<Node> proc;

		public:
		ControlWhile(Node *cond, Node *proc);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // CONTROL_H

