#ifndef OUTPUT_PASS_H
#define OUTPUT_PASS_H

#include <llvm/IR/Module.h>
#include <string>


namespace expr {
	class OutputPass {
		public:
		virtual bool run(llvm::Module &module, std::string &fname) = 0;
	};
}

#endif  // OUTPUT_PASS_H

