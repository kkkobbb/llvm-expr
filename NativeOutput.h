#ifndef NATIVE_OUTPUT_H
#define NATIVE_OUTPUT_H

#include "OutputBase.h"
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>
#include <string>


namespace expr {
	class NativeOutput: public OutputBase {
		const llvm::TargetMachine::CodeGenFileType fileType;
		const std::string defaultName;

	public:
		NativeOutput(llvm::TargetMachine::CodeGenFileType fileType);
		bool run(llvm::Module &module, std::string &fname) override;

	private:
		std::string getDefaultName(llvm::TargetMachine::CodeGenFileType fileType);
	};
}

#endif  // NATIVE_OUTPUT_H

