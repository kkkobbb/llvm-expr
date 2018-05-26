/* vim: set filetype=yacc:
 *
 * bison 3.0.4
 * generating c++ code
 */

%{
#include <iostream>
#include <string>
#include <utility>

#include "ope.h"
%}

%code requires
{
/* .hhに追加するコード */
#include "Ast.h"
#define MY_NAMESPACE expr
namespace MY_NAMESPACE {
	bool is_parse_err();
	class Lexer;
}
}

%code top
{
/* .ccの先頭に追加するコード */
#include "Lexer.h"
using namespace MY_NAMESPACE;

/* 関数名を強引に変更している */
#define yylex lexer.yylex

/* エラー確認用の変数 */
bool parse_err_f = false;
int parse_err_num = 0;
}

%code
{
/* .ccに追加するコード */
}


%defines
%skeleton "lalr1.cc"
%locations
%define api.namespace {MY_NAMESPACE}
%define parser_class_name {Parser}
%parse-param {Lexer &lexer}


%union {
    int ival;
    std::string *sval;

    int ntype;
    AstNode *astnode;
}

/* 終端記号 */
%token <ival>    INTEGER
%token <sval>    IDENTIFIER STRING

/* 予約語 */
%token           RE_THROUGH
%token           RE_RETURN
%token           RE_VOID RE_INT
%token           RE_IF RE_ELSE RE_WHILE

/* 1文字の演算子(定義のみ) */
%token           OP_COMMA
%token           OP_BXOR OP_BOR OP_BAND OP_BNOT
%token           OP_LT OP_GT
%token           OP_ADD OP_SUB
%token           OP_MUL OP_DIV OP_MOD
%token           OP_NOT

/* 2文字以上の演算子 */
%token           OP_LTE OP_GTE OP_EQ OP_NE
%token           OP_LOR OP_LAND
%token           OP_ARROW_L OP_ARROW_R

/* 構文木のルート */
%type <astnode>  ast_root
%type <astnode>  translation_unit
/* 文 */
%type <astnode>  statement statement_list
%type <astnode>  compound_statement
%type <astnode>  expression_statement
/* 式 */
%type <astnode>  expression
%type <astnode>  assignment_expression
%type <astnode>  constant_expression
%type <astnode>  logical_OR_expression logical_AND_expression
%type <astnode>  AND_expression exclusive_OR_expression inclusive_OR_expression
%type <astnode>  equality_expression
%type <astnode>  relational_expression
%type <astnode>  additive_expression multiplicative_expression
%type <astnode>  cast_expression
%type <astnode>  unary_expression
%type <ntype>    unary_operator
%type <astnode>  postfix_expression
%type <astnode>  primary_expression
%type <astnode>  constant
%type <astnode>  identifier


%%

/* 構文木のルート*/
ast_root
    : translation_unit
        { set_ast($1); }
    ;

/* 翻訳単位 */
/* 一文のみ */
translation_unit
    : statement
        { $$ = new AstUnit($1); }
    ;


/********* 文 ***********/
statement
    : expression_statement
        { $$ = std::move($1); }
    | compound_statement
        { $$ = std::move($1); }
    ;

statement_list
    : statement
        { $$ = new AstStatementList($1); }
    | statement_list statement
        { $$ = std::move(((AstStatementList *)$1)->add($2)); }
    ;

compound_statement
    : '{' statement_list '}'
        { $$ = std::move($2); }
    | '{' '}'
        { $$ = nullptr; }
    ;

expression_statement
    : expression ';'
        { $$ = new AstStatement($1); }
    | ';'
        { $$ = nullptr; }
    | error
        { $$ = nullptr; yyerrok; }  /* エラー処理 */
    ;


/********** 式 **********/
/* コンマ演算子 */
expression
    : assignment_expression
        { $$ = std::move($1); }
    | expression ',' assignment_expression
        { $$ = new AstExpression(ope::OP_COMMA, $1, $3); }
    ;

/* 代入式 */
assignment_expression
    : constant_expression
        { $$ = std::move($1); }
    | identifier OP_ARROW_L assignment_expression
        { $$ = new AstExpression(ope::OP_ARROW_L, $1, $3); }
    ;

/* 定数式 */
constant_expression
    : logical_OR_expression
        { $$ = std::move($1); }
    ;

/* 論理和 */
logical_OR_expression
    : logical_AND_expression
        { $$ = std::move($1); }
    | logical_OR_expression OP_LOR logical_AND_expression
        { $$ = new AstExpression(ope::OP_LOR, $1, $3); }
    ;

/* 論理積 */
logical_AND_expression
    : inclusive_OR_expression
        { $$ = std::move($1); }
    | logical_AND_expression OP_LAND inclusive_OR_expression
        { $$ = new AstExpression(ope::OP_LAND, $1, $3); }
    ;

/* ビット演算OR */
inclusive_OR_expression
    : exclusive_OR_expression
        { $$ = std::move($1); }
    | inclusive_OR_expression '|' exclusive_OR_expression
        { $$ = new AstExpression(ope::OP_BOR, $1, $3); }
    ;

/* ビット演算XOR */
exclusive_OR_expression
    : AND_expression
        { $$ = std::move($1); }
    | exclusive_OR_expression '^' AND_expression
        { $$ = new AstExpression(ope::OP_BXOR, $1, $3); }
    ;

/* ビット演算AND */
AND_expression
    : equality_expression
        { $$ = std::move($1); }
    | AND_expression '&' equality_expression
        { $$ = new AstExpression(ope::OP_BAND, $1, $3); }
    ;

/* 等値演算 */
equality_expression
    : relational_expression
        { $$ = std::move($1); }
    | equality_expression OP_EQ relational_expression
        { $$ = new AstExpression(ope::OP_EQ, $1, $3); }
    | equality_expression OP_NE relational_expression
        { $$ = new AstExpression(ope::OP_NE, $1, $3); }
    ;

/* 関係演算 */
relational_expression
    : additive_expression
        { $$ = std::move($1); }
    | relational_expression '<' additive_expression
        { $$ = new AstExpression(ope::OP_LT, $1, $3); }
    | relational_expression '>' additive_expression
        { $$ = new AstExpression(ope::OP_GT, $1, $3); }
    | relational_expression OP_LTE additive_expression
        { $$ = new AstExpression(ope::OP_LTE, $1, $3); }
    | relational_expression OP_GTE additive_expression
        { $$ = new AstExpression(ope::OP_GTE, $1, $3); }
    ;

/* 加減算 */
additive_expression
    : multiplicative_expression
        { $$ = std::move($1); }
    | additive_expression '+' multiplicative_expression
        { $$ = new AstExpression(ope::OP_ADD, $1, $3); }
    | additive_expression '-' multiplicative_expression
        { $$ = new AstExpression(ope::OP_SUB, $1, $3); }
    ;

/* 乗除余算 */
multiplicative_expression
    : cast_expression
        { $$ = std::move($1); }
    | multiplicative_expression '*' cast_expression
        { $$ = new AstExpression(ope::OP_MUL, $1, $3); }
    | multiplicative_expression '/' cast_expression
        { $$ = new AstExpression(ope::OP_DIV, $1, $3); }
    | multiplicative_expression '%' cast_expression
        { $$ = new AstExpression(ope::OP_MOD, $1, $3); }
    ;

/* キャスト */
cast_expression
    : unary_expression
        { $$ = std::move($1); }
    ;

/* 前置演算子 */
unary_expression
    : postfix_expression
        { $$ = std::move($1); }
    | unary_operator cast_expression
        { $$ = new AstPrefixExpression($1, $2); }
    ;

/* 単項演算子 */
unary_operator
    : '+'
        { $$ = ope::OP_ADD; }
    | '-'
        { $$ = ope::OP_SUB; }
    | '!'
        { $$ = ope::OP_NOT; }
    | '~'
        { $$ = ope::OP_BNOT; }
    ;

/* 後置演算子 */
postfix_expression
    : primary_expression
        { $$ = std::move($1); }
    ;

/* 式の項 */
primary_expression
    : identifier
        { $$ = std::move($1); }
    | constant
        { $$ = std::move($1); }
    | '(' expression ')'
        { $$ = std::move($2); }
    ;

/* 定数 */
constant
    : RE_THROUGH
        { error(yyla.location, "through"); }
    | INTEGER
        { $$ = new AstConstant($1); }
    ;

identifier
    : IDENTIFIER
        { $$ = new AstIdentifier($1); }
    ;

%%

void Parser::error(const location_type& l, const std::string& msg)
{
	parse_err_f = true;
	++parse_err_num;
	std::cerr << "Error (" << l.end.line << "): " << msg << std::endl;
}


bool expr::is_parse_err()
{
	return parse_err_f;
}

