#ifndef IRGENERATOR_H
#define IRGENERATOR_H

#include <memory>

// llvm
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "AstNode.h"


namespace expr {
	// LLVM IR 生成器
	class IRGenerator
	{
		llvm::LLVMContext TheContext;
		std::unique_ptr<llvm::Module> TheModule;

		public:
		bool genarate(std::unique_ptr<AstNode> ast_root);
		std::unique_ptr<llvm::Module> get();
	};
}

#endif  // IRGENERATOR_H

