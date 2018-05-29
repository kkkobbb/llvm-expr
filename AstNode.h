#ifndef ASTNODE_H
#define ASTNODE_H

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
		std::string dbg_msg;

		public:
		virtual void print_debug(std::ostream &dout, int indent = 0)
		{
			// インデント
			for (int i = 0; i < indent; ++i)
				dout << "  ";

			// クラス名かメッセージ表示
			if (!dbg_msg.empty())
				dout << dbg_msg << " ";
			else
				dout << typeid(*this).name();
			dout << std::endl;
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
		AstUnit(AstNode *n) : AstList(n) {}
	};

	// 文のリスト
	class AstStatementList: public AstList
	{
		public:
		AstStatementList(AstNode *n) : AstList(n) {}
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
		std::unique_ptr<AstNode> l;  // 左辺
		std::unique_ptr<AstNode> r;  // 右辺

		public:
		AstExpression(AstNode *l, AstNode *r)
		{
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

	// 代入演算子
	class AstExpressionAS: public AstExpression
	{
		public:
		AstExpressionAS(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// コンマ演算子
	class AstExpressionCOMMA: public AstExpression
	{
		public:
		AstExpressionCOMMA(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 論理演算 OR
	class AstExpressionLOR: public AstExpression
	{
		public:
		AstExpressionLOR(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 論理演算 AND
	class AstExpressionLAND: public AstExpression
	{
		public:
		AstExpressionLAND(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// ビット演算 OR
	class AstExpressionBOR: public AstExpression
	{
		public:
		AstExpressionBOR(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// ビット演算 XOR
	class AstExpressionBXOR: public AstExpression
	{
		public:
		AstExpressionBXOR(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// ビット演算 AND
	class AstExpressionBAND: public AstExpression
	{
		public:
		AstExpressionBAND(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 関係演算 等値
	class AstExpressionEQ: public AstExpression
	{
		public:
		AstExpressionEQ(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 関係演算 非等値
	class AstExpressionNE: public AstExpression
	{
		public:
		AstExpressionNE(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 関係演算 <
	class AstExpressionLT: public AstExpression
	{
		public:
		AstExpressionLT(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 関係演算 >
	class AstExpressionGT: public AstExpression
	{
		public:
		AstExpressionGT(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 関係演算 <=
	class AstExpressionLTE: public AstExpression
	{
		public:
		AstExpressionLTE(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 関係演算 >=
	class AstExpressionGTE: public AstExpression
	{
		public:
		AstExpressionGTE(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 加算
	class AstExpressionADD: public AstExpression
	{
		public:
		AstExpressionADD(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 減算
	class AstExpressionSUB: public AstExpression
	{
		public:
		AstExpressionSUB(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 乗算
	class AstExpressionMUL: public AstExpression
	{
		public:
		AstExpressionMUL(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 除算
	class AstExpressionDIV: public AstExpression
	{
		public:
		AstExpressionDIV(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 余算
	class AstExpressionMOD: public AstExpression
	{
		public:
		AstExpressionMOD(AstNode *l, AstNode *r) : AstExpression(l, r) {}
	};

	// 単項演算子 正
	class AstExpressionSPOS: public AstExpression
	{
		public:
		AstExpressionSPOS(AstNode *n) : AstExpression(nullptr, n) {}
	};

	// 単項演算子 負
	class AstExpressionSNEG: public AstExpression
	{
		public:
		AstExpressionSNEG(AstNode *n) : AstExpression(nullptr, n) {}
	};

	// 論理演算 否定
	class AstExpressionLNOT: public AstExpression
	{
		public:
		AstExpressionLNOT(AstNode *n) : AstExpression(nullptr, n) {}
	};

	// ビット演算 否定
	class AstExpressionBNOT: public AstExpression
	{
		public:
		AstExpressionBNOT(AstNode *n) : AstExpression(nullptr, n) {}
	};

	// 値
	class AstConstant: public AstNode
	{
		int number;

		public:
		AstConstant(int number)
		{
			this->number = number;
			dbg_msg = "(" + std::to_string(number) + ")";
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
			dbg_msg = "\"" + *this->name + "\"";
		}
	};
}

#endif  // ASTNODE_H

