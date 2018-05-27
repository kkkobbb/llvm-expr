#ifndef MY_AST_H
#define MY_AST_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>


namespace expr {
	// ノード
	class AstNode
	{
		protected:
		bool throughFlag = false;
		std::string node_type = "Node";

		public:
		virtual void print_debug(std::ostream &dout, int indent = 0)
		{
			for (int i = 0; i < indent; ++i)
				dout << "  ";
			dout << node_type << std::endl;
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
			node_type = "AstList";
			add(n);
		}

		AstNode *add(AstNode *n)
		{
			if (n != nullptr)
				children.push_back(std::unique_ptr<AstNode>(n));

			return this;
		}

		virtual void print_debug(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_debug(dout, indent);

			// 子要素の表示
			int next_indent = indent + 1;
			for (auto itr = children.cbegin(); itr != children.cend(); ++itr)
				(*itr)->print_debug(dout, next_indent);
		}
	};

	// 翻訳単位
	class AstUnit: public AstList
	{
		public:
		AstUnit(AstNode *n) : AstList(n)
		{
			node_type = "AstUnit";
		}
	};

	// 文のリスト
	class AstStatementList: public AstList
	{
		public:
		AstStatementList(AstNode *n) : AstList(n)
		{
			node_type = "AstStatementList";
		}
	};

	// 文
	class AstStatement: public AstNode
	{
		std::unique_ptr<AstNode> n;

		public:
		AstStatement(AstNode *n)
		{
			node_type = "AstStatement";
			this->n.reset(n);
		}

		virtual void print_debug(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_debug(dout, indent);
			n->print_debug(dout, indent + 1);
		}
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
			node_type = "AstExpression";
			this->ope = ope;
			this->r.reset(r);
			this->l.reset(l);
		}

		virtual void print_debug(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_debug(dout, indent);
			// 子要素の表示
			int next_indent = indent + 1;
			if (l != nullptr)
				l->print_debug(dout, next_indent);
			if (r != nullptr)
				r->print_debug(dout, next_indent);
		}
	};

	class AstExpressionCOMMA: public AstExpression
	{
		public:
		AstExpressionCOMMA(AstNode *l, AstNode *r): AstExpression(0, l, r)
		{
			node_type = "AstExpressionCOMMA";
		}
		virtual void print_debug(std::ostream &dout, int indent = 0) override
		{
			AstExpression::print_debug(dout, indent);
		}
	};

	// 単項演算子 前置
	class AstPrefixExpression: public AstExpression
	{
		public:
		AstPrefixExpression(int ope, AstNode *n) : AstExpression(ope, nullptr, n) {}

		virtual void print_debug(std::ostream &dout, int indent = 0) override
		{
			AstExpression::print_debug(dout, indent);
		}
	};

	// 単項演算子 後置
	class AstPostfixExpression: public AstExpression
	{
		public:
		AstPostfixExpression(int ope, AstNode *n) : AstExpression(ope, n, nullptr) {}

		virtual void print_debug(std::ostream &dout, int indent = 0) override
		{
			AstExpression::print_debug(dout, indent);
		}
	};

	// 値
	class AstConstant: public AstNode
	{
		int number;

		public:
		AstConstant(int number)
		{
			this->number = number;
			node_type = "AstConstant (";
			node_type += std::to_string(number);
			node_type += ")";
		}
	};

	// 識別子
	class AstIdentifier: public AstNode
	{
		std::unique_ptr<std::string> name;

		public:
		AstIdentifier(std::string *name)
		{
			this->name.reset(name);
			node_type = "AstIdentifier \"";
			node_type += this->name->c_str();
			node_type += "\"";
		}
	};
}

#endif  // MY_AST_H

