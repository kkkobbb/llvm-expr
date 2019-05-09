#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Node.h"
#include "NodeList.h"
#include "ValueVariable.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>


namespace expr {
	class IRState;

	// 式
	class Expression: public Node
	{
	protected:
		std::unique_ptr<Node> l;  // 左辺
		std::unique_ptr<Node> r;  // 右辺

	public:
		Expression(Node *l, Node *r);
		virtual llvm::Value *getValue(IRState &irs) override;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv);
		virtual void print_ast(std::ostream &dout, int indent = 0) override;
	};

	// 関数呼び出し
	class ExpressionFunc: public Expression
	{
	protected:
		std::unique_ptr<Identifier> identifier;
		std::unique_ptr<NodeList> argumentList;

	public:
		ExpressionFunc(Identifier *identifier, NodeList *argumentList);
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	// 代入演算子
	class ExpressionAS: public Expression
	{
	protected:
		std::unique_ptr<Identifier> identifier;

	public:
		ExpressionAS(Identifier *identifier, Node *value);
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	// 論理演算 OR
	class ExpressionLOR: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 論理演算 AND
	class ExpressionLAND: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// ビット演算 OR
	class ExpressionBOR: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// ビット演算 XOR
	class ExpressionBXOR: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// ビット演算 AND
	class ExpressionBAND: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 等値
	class ExpressionEQ: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 非等値
	class ExpressionNE: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 <
	class ExpressionLT: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 >
	class ExpressionGT: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 <=
	class ExpressionLTE: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 関係演算 >=
	class ExpressionGTE: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 加算
	class ExpressionADD: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 減算
	class ExpressionSUB: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 乗算
	class ExpressionMUL: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 除算
	class ExpressionDIV: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 余算
	class ExpressionMOD: public Expression
	{
	public:
		using Expression::Expression;
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 単項演算子 正
	class ExpressionSPOS: public Expression
	{
	public:
		ExpressionSPOS(Node *n) : Expression(nullptr, n) {}
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 単項演算子 負
	class ExpressionSNEG: public Expression
	{
	public:
		ExpressionSNEG(Node *n) : Expression(nullptr, n) {}
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// 論理演算 否定
	class ExpressionLNOT: public Expression
	{
	public:
		ExpressionLNOT(Node *n) : Expression(nullptr, n) {}
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};

	// ビット演算 否定
	class ExpressionBNOT: public Expression
	{
	public:
		ExpressionBNOT(Node *n) : Expression(nullptr, n) {}
		virtual llvm::Value *generate_exp(IRState &irs, llvm::Value *lv, llvm::Value *rv) override;
	};
}

#endif  // EXPRESSION_H

