#ifndef ASTGENERATOR_H
#define ASTGENERATOR_H

#include <fstream>
#include <memory>

#include "AstNode.h"


namespace expr {
	// 構文木 生成器
	class AstGenerator
	{
		std::unique_ptr<AstNode> ast_root;

		public:
		bool generate(std::ifstream &fin);
		std::unique_ptr<AstNode> get();
	};
}

#endif  // ASTGENERATOR_H

