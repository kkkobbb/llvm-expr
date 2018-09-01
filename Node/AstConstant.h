#ifndef ASTCONSTANT_H
#define ASTCONSTANT_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "AstBase.h"



namespace expr {
	class IRState;

	// 定数 整数
	class AstConstantInt: public AstNode
	{
		int num;

		public:
		AstConstantInt(int num);
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	// 定数 文字列
	class AstConstantString: public AstNode
	{
		std::unique_ptr<std::string> str;

		public:
		AstConstantString(std::string *str);
		virtual llvm::Value *getValue(IRState &irs) override;
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
		virtual llvm::Type *getType(IRState &irs) override;
		virtual llvm::Value *getValue(IRState &irs) override;
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
		std::unique_ptr<std::vector<llvm::Type*>> getTypes(IRState &irs);
		std::unique_ptr<std::vector<const std::string*>> getNames();
	};
}

#endif  // ASTCONSTANT_H

