#ifndef JUMP_H
#define JUMP_H

#include "Node.h"
#include <llvm/IR/Value.h>
#include <iostream>
#include <memory>


namespace expr {
	class IRState;

	class JumpReturn: public Node
	{
	protected:
		std::unique_ptr<Node> ret;

	public:
		JumpReturn(Node *ret);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // JUMP_H

