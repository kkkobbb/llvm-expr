#ifndef NODELIST_H
#define NODELIST_H

#include "Node.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>


namespace expr {
	class IRState;

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

#endif  // NODELIST_H

