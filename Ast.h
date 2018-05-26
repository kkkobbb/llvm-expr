#ifndef MY_AST_H
#define MY_AST_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

// llvm
#include <llvm/IR/Value.h>


namespace expr {
	// ノード
	class AstNode
	{
		protected:
		bool throughFlag = false;

		public:
		// インデントのみ
		virtual void print_debug(std::ostream &dout, int indent = 0)
		{
			for (int i = 0; i < indent; ++i)
				dout << "  ";
		}
	};

	// ノードの一覧を持つ
	class AstList: public AstNode
	{
		protected:
		std::vector<std::unique_ptr<AstNode>> children;

		public:
		AstList(AstNode *n)
		{
			add(n);
		}

		AstNode *add(AstNode *n)
		{
			if (n != nullptr)
				children.push_back(std::unique_ptr<AstNode>(n));

			return this;
		}

		virtual void print_debug(std::ostream &dout, int indent = 0) override;
	};

	// 翻訳単位
	class AstUnit: public AstList
	{
		public:
		AstUnit(AstNode *n) : AstList(n) {}

		virtual void print_debug(std::ostream &dout, int indent = 0) override;
	};

	// 文のリスト
	class AstStatementList: public AstList
	{
		public:
		AstStatementList(AstNode *n) : AstList(n) {}

		virtual void print_debug(std::ostream &dout, int indent = 0) override;
	};

	// 文
	class AstStatement: public AstNode
	{
		std::unique_ptr<AstNode> n;

		public:
		AstStatement(AstNode *n)
		{
			this->n.reset(n);
		}

		virtual void print_debug(std::ostream &dout, int indent = 0) override;
	};

	// 式
	class AstExpression: public AstNode
	{
		protected:
		int ope;  // 演算子
		std::unique_ptr<AstNode> l;  // 左辺
		std::unique_ptr<AstNode> r;  // 右辺

		public:
		AstExpression(int ope, AstNode *l, AstNode *r)
		{
			this->ope = ope;
			this->r.reset(r);
			this->l.reset(l);
		}

		virtual void print_debug(std::ostream &dout, int indent = 0) override;
	};

	// 単項演算子 前置
	class AstPrefixExpression: public AstExpression
	{
		public:
		AstPrefixExpression(int ope, AstNode *n) : AstExpression(ope, nullptr, n) {}

		virtual void print_debug(std::ostream &dout, int indent = 0) override;
	};

	// 単項演算子 後置
	class AstPostfixExpression: public AstExpression
	{
		public:
		AstPostfixExpression(int ope, AstNode *n) : AstExpression(ope, n, nullptr) {}

		virtual void print_debug(std::ostream &dout, int indent = 0) override;
	};

	// 値
	class AstConstant: public AstNode
	{
		int number;

		public:
		AstConstant(int number)
		{
			this->number = number;
		}

		virtual void print_debug(std::ostream &dout, int indent = 0) override;
	};

	// 識別子
	class AstIdentifier: public AstNode
	{
		std::unique_ptr<std::string> name;

		public:
		AstIdentifier(std::string *name)
		{
			this->name.reset(name);
		}

		virtual void print_debug(std::ostream &dout, int indent = 0) override;
	};

	void set_ast(AstNode *root);
	std::unique_ptr<AstNode> get_ast();
}

#endif  // MY_AST_H

