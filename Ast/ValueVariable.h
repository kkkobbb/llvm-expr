#ifndef VALUEVARIABLE_H
#define VALUEVARIABLE_H

#include "Node.h"
#include <llvm/IR/IRBuilder.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>


namespace expr {
	class IRState;

	// 識別子
	class Identifier: public Node
	{
		// TODO shared_ptrにする？
		std::unique_ptr<std::string> name;
		std::unique_ptr<Node> type;

	public:
		Identifier(std::string *name, Node *type);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		const std::string *getName();
		virtual llvm::Type *getType(IRState &irs) override;
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	// 識別子の一覧を持つ
	class IdentifierList: public Node
	{
	protected:
		std::vector<std::unique_ptr<Identifier>> children;

	public:
		IdentifierList(Identifier *n);
		void add(Identifier *n);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		std::unique_ptr<std::vector<llvm::Type*>> getTypes(IRState &irs);
		std::unique_ptr<std::vector<const std::string*>> getNames();
	};
}

#endif  // VALUEVARIABLE_H

