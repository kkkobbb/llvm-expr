#ifndef ASTNODE_H
#define ASTNODE_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>


namespace expr {
	// ノード
	class AstNode
	{
		protected:
		bool throughFlag = false;
		std::string dbg_msg;

		public:
		virtual llvm::Value *generate(llvm::IRBuilder<> &builder)
		{
			return nullptr;
		}

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

		void add(AstNode *n)
		{
			if (n != nullptr)
				children.push_back(std::unique_ptr<AstNode>(n));
		}

		virtual llvm::Value *generate(llvm::IRBuilder<> &builder) override
		{
			// 子要素の実行
			for (auto itr = children.cbegin(); itr != children.cend(); ++itr)
				(*itr)->generate(builder);

			return nullptr;
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
		using AstList::AstList;
	};

	// 文のリスト
	class AstStatementList: public AstList
	{
		public:
		using AstList::AstList;
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

		virtual llvm::Value *generate(llvm::IRBuilder<> &builder) override
		{
			// 子要素の実行
			n->generate(builder);

			return nullptr;
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
		using AstExpression::AstExpression;
	};

	// コンマ演算子
	class AstExpressionCOMMA: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 論理演算 OR
	class AstExpressionLOR: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 論理演算 AND
	class AstExpressionLAND: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// ビット演算 OR
	class AstExpressionBOR: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// ビット演算 XOR
	class AstExpressionBXOR: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// ビット演算 AND
	class AstExpressionBAND: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 関係演算 等値
	class AstExpressionEQ: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 関係演算 非等値
	class AstExpressionNE: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 関係演算 <
	class AstExpressionLT: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 関係演算 >
	class AstExpressionGT: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 関係演算 <=
	class AstExpressionLTE: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 関係演算 >=
	class AstExpressionGTE: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 加算
	class AstExpressionADD: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 減算
	class AstExpressionSUB: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 乗算
	class AstExpressionMUL: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 除算
	class AstExpressionDIV: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 余算
	class AstExpressionMOD: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
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
	class AstConstantInt: public AstNode
	{
		int value;

		public:
		AstConstantInt(int value)
		{
			this->value = value;
			dbg_msg = "(" + std::to_string(value) + ")";
		}
		virtual llvm::Value *generate(llvm::IRBuilder<> &builder) override;
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

