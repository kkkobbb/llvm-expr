#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <string>

#include <llvm/IR/Module.h>



namespace expr {
	class CodeGenerator {
		public:
		bool generate(llvm::Module *module, std::string *fname);
	};
}


#endif  // CODEGENERATOR_H

