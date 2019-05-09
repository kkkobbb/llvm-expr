#ifndef DECLARATION_H
#define DECLARATION_H

#include "Node.h"
#include "ValueVariable.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>


namespace expr {
	class IRState;

	class DeclarationFunc: public Node
	{
	protected:
		std::unique_ptr<Identifier> decl;
		std::unique_ptr<IdentifierList> argumentList;
		bool vararg;

	public:
		DeclarationFunc(Identifier *decl, IdentifierList *argumentList, bool vararg=false);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // DECLARATION_H

