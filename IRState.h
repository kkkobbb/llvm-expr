#ifndef IRGENINFO_H
#define IRGENINFO_H

#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>



namespace expr {
	class IRState {
		protected:
		std::vector<llvm::Function*> funcStack;

		public:
		bool errorFlag = false;

		IRState();
		llvm::LLVMContext &getContext();
		llvm::Module &getModule();
		std::unique_ptr<llvm::Module> moveModule();
		llvm::IRBuilder<> &getBuilder();
		std::size_t setValue(llvm::Value *val);
		llvm::Value *getValue(int num);
		llvm::Function *getCurFunc();
		void pushCurFunc(llvm::Function *func);
		void popCurFunc();
		llvm::Value *getVariable(const std::string *name);

		private:
		llvm::LLVMContext TheContext;
		std::unique_ptr<llvm::Module> TheModule;
		std::unique_ptr<llvm::IRBuilder<>> builder;
		// グローバルのValueの保存用
		// llvm::Moduleに登録していれば、deleteされるはず
		std::vector<llvm::Value *> ValueList;
	};
}


#endif  // IRGENINFO_H

