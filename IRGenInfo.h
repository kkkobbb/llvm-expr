#ifndef IRGENINFO_H
#define IRGENINFO_H

#include <memory>
#include <vector>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/ValueSymbolTable.h>



namespace expr {
	class IRGenInfo {
		protected:
		std::vector<llvm::Function*> funcStack;

		public:
		bool errorFlag = false;

		IRGenInfo()
		{
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
		llvm::Function *getCurFunc()
		{
			return funcStack.back();
		}
		void pushCurFunc(llvm::Function *func)
		{
			funcStack.push_back(func);
		}
		void popCurFunc()
		{
			funcStack.pop_back();
		}

		/*
		 * nameという変数の領域を探して返す
		 *
		 * 見つからなかった場合、偽となる値を返す
		 *
		 * 1. 現在の関数のスコープに対象の変数があるか
		 * 2. グローバルに対象の変数があるか
		 */
		llvm::Value *getVariable(const std::string *name)
		{
			auto curFunc = this->getCurFunc();

			auto vs_table = curFunc->getValueSymbolTable();
			auto alloca = vs_table->lookup(*name);

			if (!alloca) {
				auto &global_vs_table = TheModule->getValueSymbolTable();
				alloca = global_vs_table.lookup(*name);
			}

			return alloca;
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

