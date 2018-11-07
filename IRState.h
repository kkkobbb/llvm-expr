#ifndef IRGENINFO_H
#define IRGENINFO_H

#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>



namespace expr {
	class IRState {
		public:
		IRState();
		bool isError();
		llvm::LLVMContext &getContext();
		llvm::Module &getModule();
		std::unique_ptr<llvm::Module> moveModule();
		llvm::IRBuilder<> &getBuilder();
		llvm::GlobalVariable *getGlobalString(const char *str);
		llvm::GlobalVariable *createGlobalString(const char *str);
		llvm::Function *getCurFunc();
		void pushCurFunc(llvm::Function *func);
		void popCurFunc();
		llvm::Value *getVariable(const std::string *name);
		bool isGlobal();

		private:
		bool errorFlag = false;
		llvm::LLVMContext TheContext;
		std::unique_ptr<llvm::Module> TheModule;
		std::unique_ptr<llvm::IRBuilder<>> builder;
		std::vector<llvm::Function*> funcStack;
		// 文字列のグローバル変数の保存用
		std::vector<llvm::Value *> GlobalStrList;
	};
}


#endif  // IRGENINFO_H
