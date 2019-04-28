#ifndef CONSTANT_H
#define CONSTANT_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "Node.h"



namespace expr {
	class IRState;

	// 定数 整数
	class ConstInt: public Node
	{
		int num;

		public:
		ConstInt(int num);
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	// 定数 文字列
	class ConstString: public Node
	{
		std::unique_ptr<std::string> str;

		public:
		ConstString(std::string *str);
		virtual llvm::Value *getValue(IRState &irs) override;
	};

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

#endif  // CONSTANT_H

