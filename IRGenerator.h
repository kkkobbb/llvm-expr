#ifndef IRGENERATOR_H_H
#define IRGENERATOR_H_H

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
		IRGenerator(std::unique_ptr<AstUnit> ast_root);
		bool genarate();
		std::unique_ptr<llvm::Module> get_module();
	};
}

#endif  // IRGENERATOR_H_H

