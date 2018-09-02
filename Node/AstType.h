#ifndef ASTTYPE_H
#define ASTTYPE_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "AstBase.h"



namespace expr {
	class IRState;

	class AstType: public AstNode
	{
		public:
		using AstNode::AstNode;
	};

	class AstTypeVoid: public AstType
	{
		public:
		using AstType::AstType;
		virtual llvm::Type *getType(IRState &irs) override;
	};

	class AstTypeInt: public AstType
	{
		public:
		using AstType::AstType;
		virtual llvm::Type *getType(IRState &irs) override;
	};

	class AstTypeString: public AstType
	{
		public:
		using AstType::AstType;
		virtual llvm::Type *getType(IRState &irs) override;
	};
}

#endif  // ASTTYPE_H

