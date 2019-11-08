#ifndef OUTPUT_BASE_H
#define OUTPUT_BASE_H

#include <llvm/IR/Module.h>
#include <string>


namespace expr {
	class OutputBase {
	public:
		virtual bool run(llvm::Module &module, std::string &fname) = 0;
	};
}

#endif  // OUTPUT_BASE_H

