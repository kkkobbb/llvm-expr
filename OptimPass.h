#ifndef OPTIMPASS_H
#define OPTIMPASS_H

#include <llvm/IR/Module.h>


namespace expr {
	class OptimPass {
	public:
		bool run(llvm::Module &module);
	};
}

#endif  // OPTIMPASS_H

