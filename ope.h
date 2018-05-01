#ifndef OPE_H
#define OPE_H

#include "Parser.hh"

namespace expr {
	// 演算子の識別子
	typedef Parser::token ope;

#if 0
	enum ope {
		OP_COMMA,
		OP_BXOR, OP_BOR, OP_BAND, OP_BNOT,
		OP_LOR, OP_LAND,
		OP_LT, OP_GT, OP_LTE, OP_GTE, OP_EQ, OP_NE,
		OP_ADD, OP_SUB,
		OP_MUL, OP_DIV, OP_MOD,
		OP_NOT,
		OP_ARROW_L, OP_ARROW_R,
	};
#endif
}


#endif  // OPE_H

