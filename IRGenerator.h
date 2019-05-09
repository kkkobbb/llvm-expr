#ifndef IRGENERATOR_H
#define IRGENERATOR_H

#include "Ast/Node.h"
#include "IRState.h"
#include <llvm/IR/Module.h>
#include <memory>


namespace expr {
	// LLVM IR 生成器
	class IRGenerator
	{
		IRState irs;
		std::unique_ptr<llvm::Module> TheModule = nullptr;

	public:
		bool generate(Node &ast_root);
		llvm::Module &get();
	};
}

#endif  // IRGENERATOR_H

