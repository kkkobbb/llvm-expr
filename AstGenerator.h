#ifndef ASTGENERATOR_H
#define ASTGENERATOR_H

#include <fstream>
#include <memory>

#include "Ast.h"


namespace expr {
	// 構文木 生成器
	class AstGenerator
	{
		std::unique_ptr<AstNode> ast_root;

		public:
		bool genarate(std::ifstream &fin);
		std::unique_ptr<AstNode> get();
	};
}

#endif  // ASTGENERATOR_H

