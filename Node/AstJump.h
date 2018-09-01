#ifndef ASTJUMP_H
#define ASTJUMP_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "AstBase.h"



namespace expr {
	class IRState;

	class AstJumpReturn: public AstNode
	{
		protected:
		std::unique_ptr<AstNode> ret;

		public:
		AstJumpReturn(AstNode *ret);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // ASTJUMP_H

