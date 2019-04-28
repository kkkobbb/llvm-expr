#ifndef BASE_H
#define BASE_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>



namespace expr {
	class IRState;

	// ノード
	class Node
	{
		protected:
		std::string dbg_msg;

		public:
		virtual llvm::Value *getValue(IRState &irs);
		virtual llvm::Type *getType(IRState &irs);
		void print_ast_string(const char *msg, std::ostream &dout, int indent = 0);
		virtual void print_ast(std::ostream &dout, int indent = 0);
	};

	// ノードの一覧を持つ
	class NodeList: public Node
	{
		protected:
		std::vector<std::unique_ptr<Node>> children;

		public:
		NodeList(Node *n);
		void add(Node *n);
		virtual llvm::Value *getValue(IRState &irs) override;
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		std::vector<std::unique_ptr<Node>> *getList();
	};

	// 翻訳単位
	class Unit: public NodeList
	{
		public:
		using NodeList::NodeList;
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // BASE_H

