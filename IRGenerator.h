#ifndef IRGENERATOR_H
#define IRGENERATOR_H

#include <memory>

// llvm
#include <llvm/IR/Module.h>

#include "Ast.h"


namespace expr {
	// LLVM IR 生成器
	class IRGenerator
	{
		std::unique_ptr<llvm::Module> module;

		public:
		bool genarate(std::unique_ptr<AstNode> ast_root);
		std::unique_ptr<llvm::Module> get();
	};
}

#endif  // IRGENERATOR_H

