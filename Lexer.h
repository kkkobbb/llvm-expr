#ifndef MY_LEXER_H
#define MY_LEXER_H

#include "Parser.hh"
#ifndef yyFlexLexerOnce
#include <FlexLexer.h>
#endif
#include <iostream>

namespace expr {
	class Lexer: public yyFlexLexer {

	public:
		Lexer(std::istream* in = 0, std::ostream* out = 0) : yyFlexLexer(in, out) {}
		~Lexer() {}

		int yylex(Parser::semantic_type *val, Parser::location_type *loc);
	};
}

#endif  // MY_LEXER_H


