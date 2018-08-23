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
		virtual llvm::Value *getValue(IRGenInfo &igi);
		virtual llvm::Type *getType(IRGenInfo &igi);
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
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		std::vector<std::unique_ptr<AstNode>> *getList();
	};

	// 翻訳単位
	class AstUnit: public AstList
	{
		public:
		using AstList::AstList;
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
	};

	// 定数 整数
	class AstConstantInt: public AstNode
	{
		int num;

		public:
		AstConstantInt(int num);
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
	};

	// 識別子
	class AstIdentifier: public AstNode
	{
		// TODO shared_ptrにする？
		std::unique_ptr<std::string> name;
		std::unique_ptr<AstNode> type;

		public:
		AstIdentifier(std::string *name, AstNode *type);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		const std::string *getName();
		virtual llvm::Type *getType(IRGenInfo &igi) override;
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
	};

	// 識別子の一覧を持つ
	class AstIdentifierList: public AstNode
	{
		protected:
		std::vector<std::unique_ptr<AstIdentifier>> children;

		public:
		AstIdentifierList(AstIdentifier *n);
		void add(AstIdentifier *n);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		std::unique_ptr<std::vector<llvm::Type*>> getTypes(IRGenInfo &igi);
		std::unique_ptr<std::vector<const std::string*>> getNames();
	};

	class AstDefinitionVar: public AstNode
	{
		protected:
		std::unique_ptr<AstIdentifier> decl;
		std::unique_ptr<AstNode> init;

		public:
		AstDefinitionVar(AstIdentifier *decl, AstNode *init);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
	};

	class AstDefinitionFunc: public AstNode
	{
		protected:
		std::unique_ptr<AstIdentifier> decl;
		std::unique_ptr<AstIdentifierList> argumentList;
		std::unique_ptr<AstNode> body;

		public:
		AstDefinitionFunc(AstIdentifier *decl, AstIdentifierList *argumentList, AstNode *body);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
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
		AstExpression(AstNode *l, AstNode *r);
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
		virtual llvm::Value *generate_exp(IRGenInfo &igi, llvm::Value *lv, llvm::Value *rv);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
	};

	// 関数呼び出し
	class AstExpressionFunc: public AstExpression
	{
		protected:
		AstIdentifier *identifier;
		AstList *argumentList;

		public:
		AstExpressionFunc(AstIdentifier *identifier, AstList *argumentList);
		virtual llvm::Value *getValue(IRGenInfo &igi) override;
	};

	// 代入演算子
	class AstExpressionAS: public AstExpression
	{
		protected:
		AstIdentifier *identifier;

		public:
		AstExpressionAS(AstIdentifier *identifier, AstNode *value);
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

