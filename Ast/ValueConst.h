#ifndef VALUECONST_H
#define VALUECONST_H

#include "Node.h"
#include <llvm/IR/Value.h>
#include <string>
#include <memory>


namespace expr {
	class IRState;

	// 定数 整数
	class ConstInt: public Node
	{
		int num;

	public:
		ConstInt(int num);
		virtual llvm::Value *getValue(IRState &irs) override;
	};

	// 定数 文字列
	class ConstString: public Node
	{
		std::unique_ptr<std::string> str;

	public:
		ConstString(std::string *str);
		virtual llvm::Value *getValue(IRState &irs) override;
	};
}

#endif  // VALUECONST_H

