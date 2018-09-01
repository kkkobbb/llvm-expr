#ifndef ASTCONTROL_H
#define ASTCONTROL_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "AstBase.h"



namespace expr {
	class IRState;

	class AstControlIf: public AstNode
	{
		protected:
		std::unique_ptr<AstNode> cond;
		std::unique_ptr<AstNode> proc;
		std::unique_ptr<AstNode> elseProc;

		public:
		AstControlIf(AstNode *cond, AstNode *proc, AstNode *elseProc);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	class AstControlWhile: public AstNode
	{
		protected:
		std::unique_ptr<AstNode> cond;
		std::unique_ptr<AstNode> proc;

		public:
		AstControlWhile(AstNode *cond, AstNode *proc);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // ASTCONTROL_H

