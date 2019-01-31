#ifndef NATIVEGENPASS_H
#define NATIVEGENPASS_H

#include <string>

#include <llvm/IR/Module.h>



namespace expr {
	class NativeGenPass {
		public:
		bool run(llvm::Module &module, std::string &fname);
	};
}


#endif  // NATIVEGENPASS_H

