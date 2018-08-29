#ifndef OPTIMGENERATOR_H
#define OPTIMGENERATOR_H

#include <memory>

#include <llvm/IR/Module.h>



namespace expr {
	class OptimGenerator {
		std::unique_ptr<llvm::Module> TheModule = nullptr;

		public:
		bool generate(std::unique_ptr<llvm::Module> module);
		std::unique_ptr<llvm::Module> get();
	};
}


#endif  // OPTIMGENERATOR_H

