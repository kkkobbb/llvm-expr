#ifndef ASTEXPRESSION_H
#define ASTEXPRESSION_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "AstBase.h"
#include "AstConstant.h"



namespace expr {
	class IRState;

	// 式
	class AstExpression: public AstNode
	{
		protected:
		std::unique_ptr<AstNode> l;  // 左辺
		std::unique_ptr<AstNode> r;  // 右辺

		public:
		AstExpression(AstNode *l, AstNode *r);
		virtual llvm::Value *getValue(IRState &irs) override;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
	};

	// 関数呼び出し
	class AstExpressionFunc: public AstExpression
	{
		protected:
		std::unique_ptr<AstIdentifier> identifier;
		std::unique_ptr<AstList> argumentList;

		public:
		AstExpressionFunc(AstIdentifier *identifier, AstList *argumentList);
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	// 代入演算子
	class AstExpressionAS: public AstExpression
	{
		protected:
		std::unique_ptr<AstIdentifier> identifier;

		public:
		AstExpressionAS(AstIdentifier *identifier, AstNode *value);
		virtual llvm::Value *getValue(IRState &irs) override;
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
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 非等値
	class AstExpressionNE: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 <
	class AstExpressionLT: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 >
	class AstExpressionGT: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 <=
	class AstExpressionLTE: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 >=
	class AstExpressionGTE: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 加算
	class AstExpressionADD: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 減算
	class AstExpressionSUB: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 乗算
	class AstExpressionMUL: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 除算
	class AstExpressionDIV: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 余算
	class AstExpressionMOD: public AstExpression
	{
		public:
		using AstExpression::AstExpression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
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

#endif  // ASTEXPRESSION_H

