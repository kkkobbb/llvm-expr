#ifndef NATIVE_OUTPUT_PASS_H
#define NATIVE_OUTPUT_PASS_H

#include "OutputPass.h"
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>
#include <string>


namespace expr {
	class NativeOutputPass: public OutputPass {
		public:
		NativeOutputPass(llvm::TargetMachine::CodeGenFileType fileType);
		bool run(llvm::Module &module, std::string &fname) override;

		private:
		const llvm::TargetMachine::CodeGenFileType fileType;
		const std::string defaultName;
		std::string getDefaultName(llvm::TargetMachine::CodeGenFileType fileType);
	};
}

#endif  // NATIVE_OUTPUT_PASS_H

