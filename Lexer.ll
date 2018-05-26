/* vim: set filetype=lex:
 *
 * flex 2.6.4
 * generating c++ code
 */

%top {
/* .ccの先頭に追加するコード */
// TODO yy_size_t の変数とintにキャストした変数の比較をしているアホな処理への応急処置
typedef int yy_size_t;
#define YY_TYPEDEF_YY_SIZE_T
}

%{
/* .ccの各種定義後に追加するコード */
#include <iostream>

#include "Lexer.h"
#include "Parser.hh"
using namespace expr;

typedef Parser::token token;
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
"through"  { return token::RE_THROUGH; }
"return"   { return token::RE_RETURN; }
"void"     { return token::RE_VOID; }
"int"      { return token::RE_INT; }
"if"       { return token::RE_IF; }
"else"     { return token::RE_ELSE; }
"while"    { return token::RE_WHILE; }

 /* 識別子 */
{Letter}({Letter}|{Digit})*  { val->sval = new std::string(yytext, yyleng); return token::IDENTIFIER; }
 /* 10進数の整数定数 */
{Digit}+                     { val->ival = atoi(yytext); return token::INTEGER; }
 /* 文字定数 */
'(\\.|[^\\'\n])+'            { val->ival = yytext[1]; return token::INTEGER; }

 /* 演算子 */
"<="  { return token::OP_LTE; }
">="  { return token::OP_GTE; }
"=="  { return token::OP_EQ; }
"!="  { return token::OP_NE; }
"||"  { return token::OP_LOR; }
"&&"  { return token::OP_LAND; }
"<-"  { return token::OP_ARROW_L; }
"->"  { return token::OP_ARROW_R; }
"="   { return '='; }
"<"   { return '<'; }
">"   { return '>'; }
"|"   { return '|'; }
"&"   { return '&'; }
"^"   { return '^'; }
"~"   { return '~'; }
"-"   { return '-'; }
"+"   { return '+'; }
"*"   { return '*'; }
"/"   { return '/'; }
"%"   { return '%'; }
"!"   { return '!'; }
";"   { return ';'; }
"{"   { return '{'; }
"}"   { return '}'; }
","   { return ','; }
"?"   { return '?'; }
":"   { return ':'; }
"("   { return '('; }
")"   { return ')'; }
"["   { return '['; }
"]"   { return ']'; }
"."   { return '.'; }

 /* その他 */
.     {};

%%

