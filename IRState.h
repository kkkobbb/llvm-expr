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
		// エラー関係
		bool isError();
		const std::vector<std::unique_ptr<std::string> > *getErrorMsgList();
		void setError();
		void setError(const char *msg);
		void setError(std::unique_ptr<std::string> msg);

		// LLVM IR生成用
		llvm::LLVMContext &getContext();
		llvm::Module &getModule();
		llvm::IRBuilder<> &getBuilder();
		llvm::GlobalVariable *getGlobalString(const char *str);
		llvm::GlobalVariable *createGlobalString(const char *str);
		llvm::Function *getCurFunc();
		void pushCurFunc(llvm::Function *func);
		llvm::Function *popCurFunc();
		llvm::Value *getVariable(const std::string *name);

		// 生成後の処理用
		std::unique_ptr<llvm::Module> moveModule();
		bool isGlobal();
	};
}

#endif  // IRGENINFO_H

