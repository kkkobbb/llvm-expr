#ifndef NATIVE_OUTPUT_PASS_H
#define NATIVE_OUTPUT_PASS_H

#include <string>

#include <llvm/IR/Module.h>

#include "OutputPass.h"



namespace expr {
	class NativeOutputPass: public OutputPass {
		public:
		bool run(llvm::Module &module, std::string &fname) override;
	};
}


#endif  // NATIVE_OUTPUT_PASS_H

