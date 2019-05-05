#ifndef IRSTATE_H
#define IRSTATE_H

#include "Ast/Node.h"
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
		std::vector<llvm::BasicBlock *> blockStack;
		// 文字列のグローバル変数の保存用
		std::vector<llvm::Value *> GlobalStrList;

		void pushBlock(llvm::BasicBlock *block);
		llvm::BasicBlock *popBlock();

		// TがNode型以外の場合のみT::getValue()を実行する
		template <class T>
		llvm::Value *getValue_(T *node)
		{
			// 基底クラスのgetValue()を実行する
			return node->T::getValue(*this);
		}
		llvm::Value *getValue_(Node *node)
		{
			return node->getValue(*this);
		}

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
		llvm::Value *getVariable(const std::string *name);

		// ブロックの階層をスタックに保存してIR生成する
		//
		// テンプレートにNode以外の型を指定した場合、その型のgetValue()を使用する
		//   (基底クラスのgetValue()を実行したい場合用)
		// T Node*の派生クラスであること (そうでない場合、T=Nodeとみなす)
		//   かつnodeに渡した型の基底クラスであること
		//
		// node IR生成するノード
		// needNewBlock 新しい階層のブロックでIR生成する場合、真
		template<class T=Node>
		llvm::Value *createValueInBlock(Node *node, bool needNewBlock=false)
		{
			if (node == nullptr)
				return nullptr;

			if (needNewBlock)
				pushBlock(builder->GetInsertBlock());

			llvm::Value *value;
			// FIXME dynamic_cast を使っているので実行コストが大きい
			if(dynamic_cast<T *>(node) != nullptr)
				value = getValue_((T *)node);
			else
				value = getValue_(node);

			if (needNewBlock)
				popBlock();

			return value;
		}

		// 生成後の処理用
		std::unique_ptr<llvm::Module> moveModule();
	};

}

#endif  // IRSTATE_H

