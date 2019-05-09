#ifndef IRSTATE_H
#define IRSTATE_H

#include "Ast/Node.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>
#include <string>
#include <memory>
#include <vector>
#include <type_traits>


namespace expr {
	class IRState {
		llvm::LLVMContext TheContext;
		llvm::Module TheModule;
		llvm::IRBuilder<> builder;
		std::vector<llvm::BasicBlock *> blockStack;
		// 文字列のグローバル変数の保存用
		std::vector<llvm::Value *> GlobalStrList;
		// 解析時にエラーがある場合、真
		bool errorFlag = false;
		// エラーメッセージ保存用
		std::vector<std::unique_ptr<std::string> > errorMstList;

	public:
		IRState();
		// LLVM IR生成用
		llvm::LLVMContext &getContext();
		llvm::Module &getModule();
		llvm::IRBuilder<> &getBuilder();
		llvm::GlobalVariable *getGlobalString(const char *str);
		llvm::Value *getVariable(const std::string *name);

		// (必要があれば)ブロックの階層をスタックに保存してIR生成する
		//
		// テンプレートTにNode以外の型を指定した場合、その型のgetValue()を使用する
		//   (基底クラスのgetValue()を実行したい場合用)
		//
		// T Nodeの派生クラスであること
		//   かつnodeの型Uの基底クラスであること
		// U nodeの型であり、明示的に指定しないこと
		// (TもUもNodeの派生クラス(Node型含む)であること)
		//
		// node IR生成するノード
		// needNewBlock 新しい階層のブロックでIR生成する場合、真
		template<class T=Node, class U>
		llvm::Value *getValueInBlock(U *node, bool needNewBlock=false)
		{
			// TがNodeの派生クラスであること (そうでない場合、コンパイル時エラー)
			static_assert(std::is_base_of<Node, T>::value, "'T' is bad class");
			// TがUの基底クラスであること (そうでない場合、コンパイル時エラー)
			static_assert(std::is_base_of<T, U>::value, "'T' is not base of 'U'");

			if (node == nullptr)
				return nullptr;

			if (needNewBlock)
				pushBlock(builder.GetInsertBlock());

			const auto value = getValue_((T *)node);

			if (needNewBlock)
				popBlock();

			return value;
		}

		// エラー関係
		void setError();
		void setError(const char *msg);
		void setError(std::unique_ptr<std::string> msg);
		bool isError();
		const std::vector<std::unique_ptr<std::string> > *getErrorMsgList();

	private:
		llvm::GlobalVariable *createGlobalString(const char *str);
		void pushBlock(llvm::BasicBlock *block);
		llvm::BasicBlock *popBlock();

		// TがNode型以外の場合のみT::getValue()を実行する
		template <class T>
		llvm::Value *getValue_(T *node)
		{
			// T型のgetValue()を実行する
			return node->T::getValue(*this);
		}
		llvm::Value *getValue_(Node *node)
		{
			// nodeの実体の型のgetValue()を実行する
			return node->getValue(*this);
		}
	};

}

#endif  // IRSTATE_H

