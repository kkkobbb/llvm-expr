/*
 * bison 3.0.4
 * generating c++ code
 */

%{
/* .ccに追加するコード */
%}

%code top
{
/* .ccの先頭に追加するコード */
#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include "Lexer.h"
#include "Node/AstAll.h"

/* 関数名を強引に変更している */
#define yylex lexer.yylex

using namespace MY_NAMESPACE;

static void set_ast(AstNode *root);

/* 構文木格納用 */
static std::unique_ptr<AstNode> ast_root;
/* エラー確認用の変数 */
static bool parse_err_f = false;
static int parse_err_num = 0;
}

%code requires
{
/* .hhの先頭に追加するコード */
#include <memory>
#define MY_NAMESPACE expr
namespace MY_NAMESPACE {
	class Lexer;
	class AstNode;
}
}

%code provides
{
/* .hhの末尾に追加するコード */
bool is_parse_err();
std::unique_ptr<MY_NAMESPACE::AstNode> get_ast();
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
%token           RE_VOID RE_INT RE_STRING
%token           RE_IF RE_ELSE RE_WHILE
%token           RE_VAR RE_FNC
%token           RE_DECL
%token           RE_RETURN RE_BREAK RE_CONTINUE
%token           RE_COMPILEERR RE_RUNTIMEERR
%token           RE_VARARG

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
%type <astnode>  mixed_expression_list
%type <astnode>  mixed_expression
/* ジャンプ文 */
%type <astnode>  jump
/* 制御文 */
%type <astnode>  control
%type <astnode>  if_expression
%type <astnode>  while_expression
/* 宣言 */
%type <astnode>  declarator
%type <astnode>  definition
%type <astnode>  identifier_type_list
%type <astnode>  primary_type
/* 独自のエラー発生 */
%type <astnode>  myerror myerror_compile myerror_runtime
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
%type <astnode>  constant_int constant_str
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
    | ';'
        { $$ = nullptr; }
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

/* 拡張された式 (値を持つが、その値に意味がない式(jumpなど)を含む) */
extended_expression
    : mixed_expression
        { $$ = std::move($1); }
    | jump
        { $$ = std::move($1); }
    | declarator
        { $$ = std::move($1); }
    | definition
        { $$ = std::move($1); }
    | myerror
        { $$ = std::move($1); }
    | error
        { $$ = nullptr; }  /* エラー処理 */
    ;

/* 色々な式のリスト */
mixed_expression_list
    : mixed_expression
        { $$ = new AstList($1); }
    | mixed_expression_list ',' mixed_expression
        {
          ((AstList *)$1)->add($3);
          $$ = std::move($1);
        }
    ;

/* 色々な式 */
mixed_expression
    : pure_expression
        { $$ = std::move($1); }
    | constant_str
        { $$ = std::move($1); }
    | control
        { $$ = std::move($1); }
    ;

/* ジャンプ文 */
jump
    : RE_RETURN expression_unit
        { $$ = new AstJumpReturn($2); }
    | RE_RETURN
        { $$ = new AstJumpReturn(nullptr); }
    | RE_BREAK
        { error(yyla.location, "Not Implemented: break"); }
    | RE_CONTINUE
        { error(yyla.location, "Not Implemented: continue"); }
    ;

/* 制御文 */
control
    : if_expression
        { $$ = std::move($1); }
    | while_expression
        { $$ = std::move($1); }
    ;

if_expression
    : RE_IF expression_unit ':' expression_list RE_ELSE expression_list '.'
        { $$ = new AstControlIf($2, $4, $6); }
    | RE_IF expression_unit ':' expression_list '.'
        { $$ = new AstControlIf($2, $4, nullptr); }
    | RE_IF expression_unit ':' RE_ELSE expression_list '.'
        { $$ = new AstControlIf($2, nullptr, $5); }
    ;

while_expression
    : RE_WHILE expression_unit ':' expression_list '.'
        { $$ = new AstControlWhile($2, $4); }
    | RE_WHILE expression_unit ':' '.'
        { $$ = new AstControlWhile($2, nullptr); }
    ;

/* 宣言文 */
declarator
    : RE_DECL RE_VAR identifier_type
        {}
    | RE_DECL RE_FNC identifier_type '(' ')'
        { $$ = new AstDeclarationFunc((AstIdentifier *)$3, nullptr); }
    | RE_DECL RE_FNC identifier_type '(' identifier_type_list ')'
        { $$ = new AstDeclarationFunc((AstIdentifier *)$3, (AstIdentifierList *)$5); }
    | RE_DECL RE_FNC identifier_type '(' identifier_type_list ',' RE_VARARG ')'
        { $$ = new AstDeclarationFunc((AstIdentifier *)$3, (AstIdentifierList *)$5, true); }
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

/* 独自のエラー発生 */
myerror
    : myerror_compile
        { $$ = std::move($1); }
    | myerror_runtime
        { $$ = std::move($1); }
    ;

/* コンパイル時にエラーを発生させる */
myerror_compile
    : RE_COMPILEERR
        { error(yyla.location, "Compile Error:"); }
    | RE_COMPILEERR STRING
        {
          std::string msg = "Compile Error: ";
          msg += *$2;
          error(yyla.location, msg);
        }
    ;

/* 実行時にエラーとして強制終了させる */
myerror_runtime
    : RE_RUNTIMEERR
        {}
    | RE_RUNTIMEERR STRING
        {}
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
    | constant_int
        { $$ = std::move($1); }
    | '(' expression_unit ')'
        { $$ = std::move($2); }
    | identifier '(' mixed_expression_list ')'
        { $$ = new AstExpressionFunc((AstIdentifier *)$1, (AstList *)$3); }
    | identifier '(' ')'
        { $$ = new AstExpressionFunc((AstIdentifier *)$1, nullptr); }
    ;

/* 定数 */
constant_int
    : INTEGER
        { $$ = new AstConstantInt($1); }

constant_str
    : STRING
        { $$ = new AstConstantString($1); }
    ;

identifier
    : IDENTIFIER
        { $$ = new AstIdentifier($1, nullptr); }
    ;

/* 型付き識別子 */
identifier_type
    : IDENTIFIER ':' primary_type
        { $$ = new AstIdentifier($1, $3); }
    | ':' primary_type  /* 型付き無名識別子 */
        { $$ = new AstIdentifier(nullptr, $2); }
    ;

%%


void Parser::error(const location_type& l, const std::string& msg)
{
	parse_err_f = true;
	++parse_err_num;
	std::cerr << "Error (" << l.end.line << "): " << msg << std::endl;
}


/*
 * 構文解析中にエラーがあった場合、真
 */
bool is_parse_err()
{
	return parse_err_f;
}


/*
 * 構文木のルートを保存する
 */
static void set_ast(AstNode *root)
{
	ast_root = std::unique_ptr<AstNode>(root);
}


/*
 * 構文木のルートを取得する
 */
std::unique_ptr<AstNode> get_ast()
{
	return std::move(ast_root);
}

