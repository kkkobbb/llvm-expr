#ifndef ASTBASE_H
#define ASTBASE_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>



namespace expr {
	class IRState;

	// ノード
	class AstNode
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
	class AstList: public AstNode
	{
		protected:
		std::vector<std::unique_ptr<AstNode>> children;

		public:
		AstList(AstNode *n);
		void add(AstNode *n);
		virtual llvm::Value *getValue(IRState &irs) override;
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		std::vector<std::unique_ptr<AstNode>> *getList();
	};

	// 翻訳単位
	class AstUnit: public AstList
	{
		public:
		using AstList::AstList;
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // ASTBASE_H

