#ifndef ASTNODE_H
#define ASTNODE_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>



namespace expr {
	class IRGenInfo;
	// ノード
	class AstNode
	{
		protected:
		bool throughFlag = false;
		std::string dbg_msg;

		public:
		virtual llvm::Value *generate(IRGenInfo &igi)
		{
			return nullptr;
		}

		bool get_through()
		{
			return throughFlag;
		}
		void set_through(bool t)
		{
			throughFlag = throughFlag || t;
		}

		virtual void print_ast(std::ostream &dout, int indent = 0)
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
			if (n == nullptr)
				return;
			set_through(n->get_through());
			children.push_back(std::unique_ptr<AstNode>(n));
		}
		virtual llvm::Value *generate(IRGenInfo &igi) override;

		virtual void print_ast(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_ast(dout, indent);

			// 子要素の表示
			int next_indent = indent + 1;
			for (auto itr = children.cbegin(); itr != children.cend(); ++itr)
				(*itr)->print_ast(dout, next_indent);
		}
	};

	// 翻訳単位
	class AstUnit: public AstList
	{
		public:
		using AstList::AstList;
		virtual llvm::Value *generate(IRGenInfo &igi) override;
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
			if(n != nullptr)
				set_through(n->get_through());
			this->n.reset(n);
		}
		virtual llvm::Value *generate(IRGenInfo &igi) override;

		virtual void print_ast(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_ast(dout, indent);
			n->print_ast(dout, indent + 1);
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
			if(l != nullptr)
				set_through(l->get_through());
			if(r != nullptr)
				set_through(r->get_through());
			this->l.reset(l);
			this->r.reset(r);
		}

		virtual void print_ast(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_ast(dout, indent);
			// 子要素の表示
			int next_indent = indent + 1;
			if (l != nullptr)
				l->print_ast(dout, next_indent);
			if (r != nullptr)
				r->print_ast(dout, next_indent);
		}
	};

	// 代入演算子
	class AstExpressionAS: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate(IRGenInfo &igi) override;
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
		virtual llvm::Value *generate(IRGenInfo &igi) override;

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
		int num;

		public:
		AstConstantInt(int num)
		{
			this->num = num;
			dbg_msg = "(" + std::to_string(num) + ")";
		}
		virtual llvm::Value *generate(IRGenInfo &igi) override;
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
		const std::string &getName()
		{
			return *name;
		}
		virtual llvm::Value *generate(IRGenInfo &igi) override;
	};
}

#endif  // ASTNODE_H

