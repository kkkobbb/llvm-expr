/*
 * flex
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

std::string *get_string(char *txt);
%}


%option c++
%option noyywrap
%option yyclass="Lexer"
%option yylineno

Digit     [0-9]
HexDigit  [0-9a-fA-F]
Letter    [a-zA-Z_]
Space     [ \t]
EscSq1    \\[btnvfr"\\]
EscSqX    \\(x|X){HexDigit}{HexDigit}
EscSq     ({EscSq1}|{EscSqX})


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
"..."         { return token_type::RE_VARARG; }

 /* 識別子 */
{Letter}({Letter}|{Digit})*  { val->sval = new std::string(yytext); return token_type::IDENTIFIER; }
 /* 10進数の整数定数 */
{Digit}+                     { val->ival = atoi(yytext); return token_type::INTEGER; }
 /* 16進数の整数定数 */
0[xX]{HexDigit}+             { val->ival = std::stoi(&yytext[2], nullptr, 16); return token_type::INTEGER; }
 /* 文字定数 */
'(\\.|[^\\'\n])+'            { val->ival = yytext[1]; return token_type::INTEGER; }
 /* 文字列定数 */
\"({EscSq}|[^\\"\n]*)*\"     { val->sval = get_string(yytext); return token_type::STRING; }

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


/*
 * 文字列生成
 *
 * "は取り除かれる
 * エスケープシーケンス(\と1文字)を認識する
 *
 * txt  先頭と末尾が"の文字列
 *      末尾-1、末尾が\ではないこと
 *      (不正な形式のエスケープシーケンスではないこと)
 */
std::string *get_string(char *txt)
{
	auto str = new std::string(txt);

	// 先頭、末尾の"の除去
	str->erase(str->begin());
	str->erase(str->end() - 1);

	// escape sequence
	std::string::size_type pos = 0;
	while ((pos = str->find("\\", pos)) != std::string::npos) {
		int replace_size = 2;
		std::string escsq = "";
		switch ((*str)[pos + 1]) {
		case '"': escsq += "\""; break;
		case 'b': escsq += "\b"; break;
		case 't': escsq += "\t"; break;
		case 'n': escsq += "\n"; break;
		case 'v': escsq += "\v"; break;
		case 'f': escsq += "\f"; break;
		case 'r': escsq += "\r"; break;
		case 'X':
		case 'x':
			escsq += std::stoi(str->substr(pos + 2, 2), nullptr, 16);
			replace_size = 4;
			break;
		case '\\':
			pos += 2;
			continue;
		default:
			++pos;
			continue;
		}
		str->replace(pos, replace_size, escsq);
	}

	// \\の処理
	pos = 0;
	while ((pos = str->find("\\\\", pos)) != std::string::npos) {
		str->replace(pos, 2, "\\");
		++pos;
	}

	return str;
}


