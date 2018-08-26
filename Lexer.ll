/* vim: set filetype=lex:
 *
 * flex 2.6.4
 * generating c++ code
 */

%top {
/* .ccの先頭に追加するコード */
}

%{
/* .ccの各種定義後に追加するコード */
#include <iostream>

#include "Lexer.h"
#include "Parser.hh"
using namespace expr;

typedef Parser::token_type token_type;
typedef Parser::semantic_type semantic_type;
typedef Parser::location_type location_type;

#undef YY_DECL
#define YY_DECL int Lexer::yylex(semantic_type *val, location_type *loc)
%}


%option c++
%option noyywrap
%option yyclass="Lexer"
%option yylineno

Digit   [0-9]
Letter  [a-zA-Z_]
Space   [ \t]


%%

 /* 空白 */
{Space}  {}
 /* 改行 */
\n       { loc->end.line = yylineno; loc->step(); }
 /* 1行コメント */
#.*      {}


 /* 予約語 */
"void"        { return token_type::RE_VOID; }
"int"         { return token_type::RE_INT; }
"string"      { return token_type::RE_STRING; }
"if"          { return token_type::RE_IF; }
"else"        { return token_type::RE_ELSE; }
"while"       { return token_type::RE_WHILE; }
"decl"        { return token_type::RE_DECL; }
"var"         { return token_type::RE_VAR; }
"fnc"         { return token_type::RE_FNC; }
"return"      { return token_type::RE_RETURN; }
"break"       { return token_type::RE_BREAK; }
"continue"    { return token_type::RE_CONTINUE; }
"compileerr"  { return token_type::RE_COMPILEERR; }
"runtimeerr"  { return token_type::RE_RUNTIMEERR; }

 /* 識別子 */
{Letter}({Letter}|{Digit})*  { val->sval = new std::string(yytext, yyleng); return token_type::IDENTIFIER; }
 /* 10進数の整数定数 */
{Digit}+                     { val->ival = atoi(yytext); return token_type::INTEGER; }
 /* 文字定数 */
'(\\.|[^\\'\n])+'            { val->ival = yytext[1]; return token_type::INTEGER; }

 /* 2文字 */
"<="  { return token_type::OP_LTE; }
">="  { return token_type::OP_GTE; }
"=="  { return token_type::OP_EQ; }
"!="  { return token_type::OP_NE; }
"||"  { return token_type::OP_LOR; }
"&&"  { return token_type::OP_LAND; }
"<-"  { return token_type::OP_ARROW_L; }
"->"  { return token_type::OP_ARROW_R; }
 /* 1文字 */
.     { return yytext[0]; }

%%

