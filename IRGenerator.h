#ifndef IRGENERATOR_H
#define IRGENERATOR_H

#include "IRState.h"
#include "Ast/Node.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalVariable.h>
#include <memory>
#include <vector>


namespace expr {
	// LLVM IR 生成器
	class IRGenerator
	{
		std::unique_ptr<llvm::Module> TheModule = nullptr;
		IRState irs;

		public:
		bool generate(Node &ast_root);
		std::unique_ptr<llvm::Module> get();
	};
}

#endif  // IRGENERATOR_H

