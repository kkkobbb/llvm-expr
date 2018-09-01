#ifndef ASTDECLARATION_H
#define ASTDECLARATION_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "AstBase.h"
#include "AstConstant.h"



namespace expr {
	class IRState;

	class AstDeclarationFunc: public AstNode
	{
		protected:
		std::unique_ptr<AstIdentifier> decl;
		std::unique_ptr<AstIdentifierList> argumentList;

		public:
		AstDeclarationFunc(AstIdentifier *decl, AstIdentifierList *argumentList);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // ASTDECLARATION_H

