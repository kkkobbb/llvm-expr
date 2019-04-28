#ifndef TYPE_H
#define TYPE_H

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>

#include "Base.h"



namespace expr {
	class IRState;

	class TypeBase: public Node
	{
		public:
		using Node::Node;
	};

	class TypeVoid: public TypeBase
	{
		public:
		using TypeBase::TypeBase;
		virtual llvm::Type *getType(IRState &irs) override;
	};

	class TypeInt: public TypeBase
	{
		public:
		using TypeBase::TypeBase;
		virtual llvm::Type *getType(IRState &irs) override;
	};

	class TypeString: public TypeBase
	{
		public:
		using TypeBase::TypeBase;
		virtual llvm::Type *getType(IRState &irs) override;
	};
}

#endif  // TYPE_H

