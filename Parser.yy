/* vim: set filetype=yacc:
 *
 * bison 3.0.4
 * generating c++ code
 */

%{
#include <iostream>
#include <string>
#include <memory>
#include <utility>
%}

%code requires
{
/* .hhに追加するコード */
#include "AstNode.h"
#define MY_NAMESPACE expr
namespace MY_NAMESPACE {
	bool is_parse_err();
	void set_ast(AstNode *root);
	std::unique_ptr<AstNode> get_ast();
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

/* 構文木格納用 */
static std::unique_ptr<AstNode> ast_root;

/* エラー確認用の変数 */
static bool parse_err_f = false;
static int parse_err_num = 0;
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
%token           RE_RETURN
%token           RE_VOID RE_INT RE_STRING
%token           RE_IF RE_ELSE RE_WHILE
%token           RE_VAR RE_FNC
%token           RE_DECL

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
%type <astnode>  expression expression_list
%type <astnode>  expression_unit
%type <astnode>  compound_expression
%type <astnode>  extended_expression
/* 制御文 */
%type <astnode>  control
%type <astnode>  if_expression
%type <astnode>  while_expression
/* 宣言 */
%type <astnode>  definition
%type <astnode>  identifier_type_list
%type <astnode>  primary_type
%type <astnode>  pure_expression_list
/* 式 */
%type <astnode>  pure_expression
%type <astnode>  assignment_expression_l assignment_expression_r
%type <astnode>  constant_expression
%type <astnode>  logical_OR_expression logical_AND_expression
%type <astnode>  AND_expression exclusive_OR_expression inclusive_OR_expression
%type <astnode>  equality_expression
%type <astnode>  relational_expression
%type <astnode>  additive_expression multiplicative_expression
%type <astnode>  cast_expression
%type <astnode>  unary_expression
%type <astnode>  postfix_expression
%type <astnode>  primary_expression
%type <astnode>  constant
%type <astnode>  identifier
%type <astnode>  identifier_type


%%

/* 構文木のルート*/
ast_root
    : translation_unit
        { set_ast($1); }
    ;

/* 翻訳単位 */
translation_unit
    : expression
        { $$ = new AstUnit($1); }
    | translation_unit expression
        {
          ((AstUnit *)$1)->add($2);
          $$ = std::move($1);
        }
    ;


/********** 式 **********/
expression
    : expression_unit ';'
        { $$ = std::move($1); }
    ;

expression_list
    : expression
        { $$ = new AstList($1); }
    | expression_list expression
        {
          ((AstList *)$1)->add($2);
          $$ = std::move($1);
        }
    ;

expression_unit
    : extended_expression
        { $$ = std::move($1); }
    | compound_expression
        { $$ = std::move($1); }
    ;

compound_expression
    : '{' expression_list '}'
        { $$ = std::move($2); }
    | '{' '}'
        { $$ = nullptr; }
    ;

/* 拡張された式 */
extended_expression
    : pure_expression
        { $$ = std::move($1); }
    | control
        { $$ = std::move($1); }
    | definition
        { $$ = std::move($1); }
    | error
        { $$ = nullptr; }  /* エラー処理 */
    ;

/* 制御文 */
control
    : if_expression
        { $$ = std::move($1); }
    | while_expression
        { $$ = std::move($1); }
    ;

/* TODO 文法 */
if_expression
    : RE_IF expression_unit ':' expression_list RE_ELSE expression_list '.'
        { $$ = new AstControlIf($2, $4, $6); }
    | RE_IF expression_unit ':' expression_list '.'
        { $$ = new AstControlIf($2, $4, nullptr); }
    | RE_IF expression_unit ':' RE_ELSE expression_list '.'
        { $$ = new AstControlIf($2, nullptr, $5); }
    ;

/* TODO 文法 */
while_expression
    : RE_WHILE expression_unit ':' expression_list '.'
        { $$ = new AstControlWhile($2, $4); }
    | RE_WHILE expression_unit ':' '.'
        { $$ = new AstControlWhile($2, nullptr); }
    ;

/* 定義文 */
definition
    : RE_VAR identifier_type
        { $$ = new AstDefinitionVar((AstIdentifier *)$2, nullptr); }
    | RE_FNC identifier_type '(' ')' expression_unit
        { $$ = new AstDefinitionFunc((AstIdentifier *)$2, nullptr, $5);}
    | RE_FNC identifier_type '(' identifier_type_list ')' expression_unit
        { $$ = new AstDefinitionFunc((AstIdentifier *)$2, (AstIdentifierList *)$4, $6);}
    ;

identifier_type_list
    : identifier_type
       { $$ = new AstIdentifierList((AstIdentifier *)$1); }
    | identifier_type_list ',' identifier_type
        {
          ((AstIdentifierList *)$1)->add((AstIdentifier *)$3);
          $$ = std::move($1);
        }
    ;

primary_type
    : RE_VOID
        { $$ = new AstTypeVoid(); }
    | RE_INT
        { $$ = new AstTypeInt(); }
    | RE_STRING
        { $$ = new AstTypeString(); }
    ;

/* 式のリスト */
pure_expression_list
    : pure_expression
        { $$ = new AstList($1); }
    | pure_expression_list ',' pure_expression
        {
          ((AstList *)$1)->add($3);
          $$ = std::move($1);
        }
    ;

/* 純粋な式 */
pure_expression
    : assignment_expression_r
        { $$ = std::move($1); }
    ;

/* 代入式右 */
assignment_expression_r
    : assignment_expression_l
        { $$ = std::move($1); }
    | assignment_expression_r OP_ARROW_R identifier
        { $$ = new AstExpressionAS((AstIdentifier *)$3, $1); }
    ;

/* 代入式左 */
assignment_expression_l
    : constant_expression
        { $$ = std::move($1); }
    | identifier OP_ARROW_L assignment_expression_l
        { $$ = new AstExpressionAS((AstIdentifier *)$1, $3); }
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
        { $$ = new AstExpressionLOR($1, $3); }
    ;

/* 論理積 */
logical_AND_expression
    : inclusive_OR_expression
        { $$ = std::move($1); }
    | logical_AND_expression OP_LAND inclusive_OR_expression
        { $$ = new AstExpressionLAND($1, $3); }
    ;

/* ビット演算OR */
inclusive_OR_expression
    : exclusive_OR_expression
        { $$ = std::move($1); }
    | inclusive_OR_expression '|' exclusive_OR_expression
        { $$ = new AstExpressionBOR($1, $3); }
    ;

/* ビット演算XOR */
exclusive_OR_expression
    : AND_expression
        { $$ = std::move($1); }
    | exclusive_OR_expression '^' AND_expression
        { $$ = new AstExpressionBXOR($1, $3); }
    ;

/* ビット演算AND */
AND_expression
    : equality_expression
        { $$ = std::move($1); }
    | AND_expression '&' equality_expression
        { $$ = new AstExpressionBAND($1, $3); }
    ;

/* 等値演算 */
equality_expression
    : relational_expression
        { $$ = std::move($1); }
    | equality_expression OP_EQ relational_expression
        { $$ = new AstExpressionEQ($1, $3); }
    | equality_expression OP_NE relational_expression
        { $$ = new AstExpressionNE($1, $3); }
    ;

/* 関係演算 */
relational_expression
    : additive_expression
        { $$ = std::move($1); }
    | relational_expression '<' additive_expression
        { $$ = new AstExpressionLT($1, $3); }
    | relational_expression '>' additive_expression
        { $$ = new AstExpressionGT($1, $3); }
    | relational_expression OP_LTE additive_expression
        { $$ = new AstExpressionLTE($1, $3); }
    | relational_expression OP_GTE additive_expression
        { $$ = new AstExpressionGTE($1, $3); }
    ;

/* 加減算 */
additive_expression
    : multiplicative_expression
        { $$ = std::move($1); }
    | additive_expression '+' multiplicative_expression
        { $$ = new AstExpressionADD($1, $3); }
    | additive_expression '-' multiplicative_expression
        { $$ = new AstExpressionSUB($1, $3); }
    ;

/* 乗除余算 */
multiplicative_expression
    : cast_expression
        { $$ = std::move($1); }
    | multiplicative_expression '*' cast_expression
        { $$ = new AstExpressionMUL($1, $3); }
    | multiplicative_expression '/' cast_expression
        { $$ = new AstExpressionDIV($1, $3); }
    | multiplicative_expression '%' cast_expression
        { $$ = new AstExpressionMOD($1, $3); }
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
    | '+' cast_expression
        { $$ = new AstExpressionSPOS($2); }
    | '-' cast_expression
        { $$ = new AstExpressionSNEG($2); }
    | '~' cast_expression
        { $$ = new AstExpressionBNOT($2); }
    | '!' cast_expression
        { $$ = new AstExpressionLNOT($2); }
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
    | '(' expression_unit ')'
        { $$ = std::move($2); }
    | identifier '(' pure_expression_list ')'
        { $$ = new AstExpressionFunc((AstIdentifier *)$1, (AstList *)$3); }
    ;

/* 定数 */
constant
    : INTEGER
        { $$ = new AstConstantInt($1); }
    ;

identifier
    : IDENTIFIER
        { $$ = new AstIdentifier($1, nullptr); }
    ;

/* 型付き識別子 */
identifier_type
    : IDENTIFIER ':' primary_type
        { $$ = new AstIdentifier($1, $3); }
    ;

%%


void Parser::error(const location_type& l, const std::string& msg)
{
	parse_err_f = true;
	++parse_err_num;
	std::cerr << "Error (" << l.end.line << "): " << msg << std::endl;
}


bool MY_NAMESPACE::is_parse_err()
{
	return parse_err_f;
}


/*
 * 構文木のルートを保存する
 */
void MY_NAMESPACE::set_ast(AstNode *root)
{
	ast_root.reset(root);
}


/*
 * 構文木のルートを取得する
 */
std::unique_ptr<AstNode> MY_NAMESPACE::get_ast()
{
	return std::move(ast_root);
}

