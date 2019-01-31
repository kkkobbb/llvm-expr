#ifndef BCGENPASS_H
#define BCGENPASS_H

#include <string>

#include <llvm/IR/Module.h>



namespace expr {
	class BCGenPass {
		public:
		bool run(llvm::Module &module, std::string &fname);
	};
}


#endif  // BCGENPASS_H

