#ifndef IRGENERATOR_H
#define IRGENERATOR_H

#include <memory>
#include <vector>

// llvm
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalVariable.h>

#include "AstNode.h"
#include "IRGenInfo.h"


namespace expr {
	// LLVM IR 生成器
	class IRGenerator
	{
		std::unique_ptr<llvm::Module> TheModule = nullptr;
		// グローバル変数保存用
		// ここから取り出す必要はない
		IRGenInfo igi;

		public:
		bool genarate(std::unique_ptr<AstNode> ast_root);
		std::unique_ptr<llvm::Module> get();
	};
}

#endif  // IRGENERATOR_H

