#ifndef ASTDEFINITION_H
#define ASTDEFINITION_H

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

	class AstDefinitionVar: public AstNode
	{
		protected:
		std::unique_ptr<AstIdentifier> decl;
		std::unique_ptr<AstNode> init;

		public:
		AstDefinitionVar(AstIdentifier *decl, AstNode *init);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	class AstDefinitionFunc: public AstNode
	{
		protected:
		std::unique_ptr<AstIdentifier> decl;
		std::unique_ptr<AstIdentifierList> argumentList;
		std::unique_ptr<AstNode> body;

		public:
		AstDefinitionFunc(AstIdentifier *decl, AstIdentifierList *argumentList, AstNode *body);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // ASTDEFINITION_H

