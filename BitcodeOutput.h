#ifndef BITCODE_OUPUT_H
#define BITCODE_OUPUT_H

#include "OutputBase.h"
#include <llvm/IR/Module.h>
#include <string>


namespace expr {
	class BitcodeOutput: public OutputBase {
	public:
		bool run(llvm::Module &module, std::string &fname) override;
	};
}

#endif  // BITCODE_OUPUT_H

