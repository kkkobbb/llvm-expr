#ifndef BITCODE_OUPUT_PASS_H
#define BITCODE_OUPUT_PASS_H

#include "OutputPass.h"
#include <llvm/IR/Module.h>
#include <string>


namespace expr {
	class BitcodeOutputPass: public OutputPass {
		public:
		bool run(llvm::Module &module, std::string &fname) override;
	};
}

#endif  // BITCODE_OUPUT_PASS_H

