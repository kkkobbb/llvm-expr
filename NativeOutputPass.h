#ifndef NATIVE_OUTPUT_PASS_H
#define NATIVE_OUTPUT_PASS_H

#include <string>

#include <llvm/IR/Module.h>



namespace expr {
	class NativeOutputPass {
		public:
		bool run(llvm::Module &module, std::string &fname);
	};
}


#endif  // NATIVE_OUTPUT_PASS_H

