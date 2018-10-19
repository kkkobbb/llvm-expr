#ifndef OPTIMGENERATOR_H
#define OPTIMGENERATOR_H

#include <llvm/IR/Module.h>



namespace expr {
	class OptimGenerator {
		public:
		bool run(llvm::Module &module);
	};
}


#endif  // OPTIMGENERATOR_H

