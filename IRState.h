#ifndef IRGENINFO_H
#define IRGENINFO_H

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <string>
#include <memory>
#include <vector>


namespace expr {
	class IRState {
		// 解析時にエラーがある場合、真
		bool errorFlag = false;
		// エラーメッセージ保存用
		std::vector<std::unique_ptr<std::string> > errorMstList;
		llvm::LLVMContext TheContext;
		std::unique_ptr<llvm::Module> TheModule;
		std::unique_ptr<llvm::IRBuilder<> > builder;
		std::vector<llvm::Function*> funcStack;  // 現在グローバルかどうかの判定にしか使用していない
		// 文字列のグローバル変数の保存用
		std::vector<llvm::Value *> GlobalStrList;

	public:
		IRState();
		bool isError();
		const std::vector<std::unique_ptr<std::string> > *getErrorMsgList();
		void setError();
		void setError(std::unique_ptr<std::string> msg);
		llvm::LLVMContext &getContext();
		llvm::Module &getModule();
		std::unique_ptr<llvm::Module> moveModule();
		llvm::IRBuilder<> &getBuilder();
		llvm::GlobalVariable *getGlobalString(const char *str);
		llvm::GlobalVariable *createGlobalString(const char *str);
		llvm::Function *getCurFunc();
		void pushCurFunc(llvm::Function *func);
		llvm::Function *popCurFunc();
		llvm::Value *getVariable(const std::string *name);
		bool isGlobal();
	};
}

#endif  // IRGENINFO_H

