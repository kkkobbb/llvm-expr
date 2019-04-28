#ifndef DEFINITION_H
#define DEFINITION_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "Base.h"
#include "Const.h"



namespace expr {
	class IRState;

	class DefinitionVar: public Node
	{
		protected:
		std::unique_ptr<Identifier> decl;
		std::unique_ptr<Node> init;

		public:
		DefinitionVar(Identifier *decl, Node *init);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	class DefinitionFunc: public Node
	{
		protected:
		std::unique_ptr<Identifier> decl;
		std::unique_ptr<IdentifierList> argumentList;
		std::unique_ptr<Node> body;

		public:
		DefinitionFunc(Identifier *decl, IdentifierList *argumentList, Node *body);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // DEFINITION_H

