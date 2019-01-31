#ifndef BITCODE_OUPUT_PASS_H
#define BITCODE_OUPUT_PASS_H

#include <string>

#include <llvm/IR/Module.h>



namespace expr {
	class BitcodeOutputPass {
		public:
		bool run(llvm::Module &module, std::string &fname);
	};
}


#endif  // BITCODE_OUPUT_PASS_H

