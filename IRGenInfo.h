#ifndef IRGENINFO_H
#define IRGENINFO_H

#include <memory>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>



namespace expr {
	class IRGenInfo {
		public:
		bool errorFlag = false;
		llvm::Function *curFunc = nullptr;

		IRGenInfo()
		{
			//builder = llvm::make_unique<llvm::IRBuilder<>>(TheContext);
			//TheModule = llvm::make_unique<llvm::Module>("code", TheContext);
			builder.reset(new llvm::IRBuilder<>(TheContext));
			TheModule.reset(new llvm::Module("code", TheContext));
		}
		llvm::LLVMContext &getContext()
		{
			return TheContext;
		}
		llvm::Module &getModule()
		{
			return *TheModule;
		}
		std::unique_ptr<llvm::Module> moveModule()
		{
			return move(TheModule);
		}
		llvm::IRBuilder<> &getBuilder()
		{
			return *builder;
		}
		std::size_t setValue(llvm::Value *val)
		{
			ValueList.push_back(val);

			return ValueList.size() - 1;
		}
		llvm::Value *getValue(int num)
		{
			return ValueList[num];
		}

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

