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
		std::string dbg_msg;

		public:
		virtual llvm::Value *getValue(IRGenInfo &igi)
		{
			return nullptr;
		}

		virtual llvm::Type *getType(IRGenInfo &igi)
		{
			return nullptr;
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
			children.push_back(std::unique_ptr<AstNode>(n));
		}
		virtual llvm::Value *getValue(IRGenInfo &igi) override;

		virtual void print_ast(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_ast(dout, indent);

			// 子要素の表示
			int next_indent = indent + 1;
			for (auto itr = children.cbegin(); itr != children.cend(); ++itr)
				(*itr)->print_ast(dout, next_indent);
		}
	};

	// 定数 整数
	class AstConstantInt: public AstNode
	{
		int num;

		public:
		AstConstantInt(int num)
		{
			this->num = num;
			dbg_msg = "(" + std::to_string(num) + ")";
		}
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
	};

	// 識別子
	class AstIdentifier: public AstNode
	{
		// TODO shared_ptrにする？
		std::unique_ptr<std::string> name;
		std::unique_ptr<AstNode> type;

		public:
		AstIdentifier(std::string *name, AstNode *type)
		{
			if(name == nullptr)
				this->name.reset(new std::string(""));
			else
				this->name.reset(name);
			dbg_msg = "\"" + *this->name + "\"";
			this->type.reset(type);
		}

		virtual void print_ast(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_ast(dout, indent);
			// 子要素の表示
			int next_indent = indent + 1;
			if (type != nullptr)
				type->print_ast(dout, next_indent);
		}

		const std::string *getName()
		{
			return name.get();
		}
		virtual llvm::Type *getType(IRGenInfo &igi) override
		{
			return this->type->getType(igi);
		}
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
	};

	// 識別子の一覧を持つ
	class AstIdentifierList: public AstNode
	{
		protected:
		std::vector<std::unique_ptr<AstIdentifier>> children;

		public:
		AstIdentifierList(AstIdentifier *n)
		{
			add(n);
		}

		void add(AstIdentifier *n)
		{
			if (n == nullptr)
				return;
			children.push_back(std::unique_ptr<AstIdentifier>(n));
		}

		virtual void print_ast(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_ast(dout, indent);

			// 子要素の表示
			int next_indent = indent + 1;
			for (auto itr = children.cbegin(); itr != children.cend(); ++itr)
				(*itr)->print_ast(dout, next_indent);
		}

		std::unique_ptr<std::vector<llvm::Type*>> getTypes(IRGenInfo &igi);
		std::unique_ptr<std::vector<const std::string*>> getNames();
	};

	// 翻訳単位
	class AstUnit: public AstList
	{
		public:
		using AstList::AstList;
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
	};

	class AstDefinitionVar: public AstNode
	{
		protected:
		std::unique_ptr<AstIdentifier> decl;
		std::unique_ptr<AstNode> init;

		public:
		AstDefinitionVar(AstIdentifier *decl, AstNode *init)
		{
			this->decl.reset(decl);
			this->init.reset(init);
		}

		virtual void print_ast(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_ast(dout, indent);
			// 子要素の表示
			int next_indent = indent + 1;
			if (decl != nullptr)
				decl->print_ast(dout, next_indent);
			if (init != nullptr)
				init->print_ast(dout, next_indent);
		}
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
	};

	class AstDefinitionFunc: public AstNode
	{
		protected:
		std::unique_ptr<AstIdentifier> decl;
		std::unique_ptr<AstIdentifierList> argumentList;
		std::unique_ptr<AstNode> body;

		public:
		AstDefinitionFunc(AstIdentifier *decl, AstIdentifierList *argumentList, AstNode *body)
		{
			this->decl.reset(decl);
			this->argumentList.reset(argumentList);
			this->body.reset(body);
		}

		virtual void print_ast(std::ostream &dout, int indent = 0) override
		{
			AstNode::print_ast(dout, indent);
			// 子要素の表示
			int next_indent = indent + 1;
			if (decl != nullptr)
				decl->print_ast(dout, next_indent);
			if (argumentList != nullptr)
				argumentList->print_ast(dout, next_indent);
			if (body != nullptr)
				body->print_ast(dout, next_indent);
		}
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
	};

	class AstType: public AstNode
	{
		public:
		using AstNode::AstNode;
	};

	class AstTypeInt: public AstType
	{
		public:
		using AstType::AstType;
		virtual llvm::Type *getType(IRGenInfo &igi) override;
	};

	class AstTypeVoid: public AstType
	{
		public:
		using AstType::AstType;
		virtual llvm::Type *getType(IRGenInfo &igi) override;
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
			this->l.reset(l);
			this->r.reset(r);
		}

		virtual llvm::Value *getValue(IRGenInfo &igi) override;

		/*
		 * 演算処理
		 *
		 * 演算子毎に変更すること
		 */
		virtual llvm::Value *generate_exp(IRGenInfo &igi, llvm::Value *lv, llvm::Value *rv)
		{
			return nullptr;
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

	// 関数呼び出し
	class AstExpressionFunc: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
	};

	// 代入演算子
	class AstExpressionAS: public AstExpression
	{
		protected:
		AstIdentifier *identifier;

		public:
		AstExpressionAS(AstIdentifier *identifier, AstNode *value)
			: AstExpression(identifier, value) {
			this->identifier = identifier;
		}
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
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
		virtual llvm::Value *generate_exp(IRGenInfo &igi, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 減算
	class AstExpressionSUB: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRGenInfo &igi, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 乗算
	class AstExpressionMUL: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRGenInfo &igi, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 除算
	class AstExpressionDIV: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRGenInfo &igi, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 余算
	class AstExpressionMOD: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRGenInfo &igi, llvm::Value *lv, llvm::Value *rv) override;
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
}

#endif  // ASTNODE_H

