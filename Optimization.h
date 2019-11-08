#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include <llvm/IR/Module.h>


namespace expr {
	class Optimization {
	public:
		bool run(llvm::Module &module);
	};
}

#endif  // OPTIMIZATION_H

