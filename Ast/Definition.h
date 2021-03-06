#ifndef DEFINITION_H
#define DEFINITION_H

#include "Node.h"
#include "ValueVariable.h"
#include <llvm/IR/Value.h>
#include <iostream>
#include <memory>


namespace expr {
	class IRState;

	class DefinitionVar: public Node
	{
	protected:
		std::unique_ptr<Identifier> decl;
		std::unique_ptr<Node> init;
		virtual bool verifyType(IRState &irs);

	public:
		DefinitionVar(Identifier *decl, Node *init);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRState &irs) = 0;
	};

	class DefinitionVarLocal: public DefinitionVar
	{
	public:
		DefinitionVarLocal(Identifier *decl, Node *init) : DefinitionVar(decl, init) {}
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	class DefinitionVarGlobal: public DefinitionVar
	{
	public:
		DefinitionVarGlobal(Identifier *decl, Node *init) : DefinitionVar(decl, init) {}
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

