/*
========================================================================================================

Scanner.h

========================================================================================================
*/
#pragma once
#include <vector>

enum TokenType_t {
	//TT_KEYWORD,		// while, true, if...
	TT_KEYWORD_WHILE,
	TT_KEYWORD_FOR,
	TT_KEYWORD_CONTINUE,
	TT_KEYWORD_BREAK,
	TT_KEYWORD_DO,
	TT_KEYWORD_IF,
	TT_KEYWORD_ELSE,
	TT_KEYWORD_SWITCH,
	TT_KEYWORD_CASE,
	TT_KEYWORD_DEFAULT,
	TT_KEYWORD_RETURN,
	TT_KEYWORD_FALSE,
	TT_KEYWORD_TRUE,
	TT_KEYWORD_BOOL,
	TT_KEYWORD_INT,
	TT_KEYWORD_FLOAT,
	TT_KEYWORD_CHAR,
	TT_KEYWORD_SHORT,
	TT_KEYWORD_STRUCT,
	TT_KEYWORD_ENUM,
	TT_KEYWORD_CONST,
	TT_KEYWORD_SIZEOF,
	TT_KEYWORD_TYPEDEF,

	TT_IDENTIFIER,	// names of variables, functions, struct...
	TT_INTEGER,		// ints, shorts, longs, chars
	TT_FLOAT,
	TT_STRING,
 	TT_WHITESPACE,
	TT_COMMA,				// ,
	TT_SEMICOLON,			// ;
	TT_DOT,					// .
 	TT_COMMENT,				// //
	TT_STAR,				// *
	TT_PLUS,				// +
	TT_DIVISION,			// /
	TT_MINUS,				// -
	TT_MODULUS,				// %
	TT_BRACKETOPEN,			// [
	TT_BRACKETCLOSE,		// ]
	TT_CURLYBRACKETOPEN,	// {
	TT_CURLYBRACKETCLOSE,	// }
	TT_PARENTHOPEN,			// (
	TT_PARENTHCLOSE,		// )
	TT_BRA,					// <
	TT_KET,					// >
	TT_ASSIGNEMNT,			// =
	TT_COMPARISON,			// ==
	TT_LESSEQUAL,			// <=
	TT_GREATEQUAL,			// >=
	TT_NOT,					// !
	TT_NOTEQUAL,			// !=
	TT_TIMESEQUAL,			// *=
	TT_PLUSEQUAL,			// +=
	TT_MINUSEQUAL,			// -=
	TT_DIVIDEEQUAL,			// /=
	TT_POINTER,				// ->
	TT_DECREMENT,			// --
	TT_INCREMENT,			// ++

	TT_AND,					// &&
	TT_OR,					// ||
	TT_BITAND,				// &
	TT_BITOR,				// |
	TT_BITXOR,				// ^
	TT_BITNOT,				// ~
	TT_SHIFTLEFT,			// <<
	TT_SHIFTRIGHT,			// >>
	TT_SHIFTLEFTEQUAL,		// <<=
	TT_SHIFTRIGHTEQUAL,		// >>=
};

struct coord_t {
	char * filename;
	int ln;
	int col;
};

/*
====================================================
Token_t
====================================================
*/
struct Token_t {
	TokenType_t type;
	const char * str;

	int valueInt;
	float valueFloat;

	coord_t coord;
	Token_t * next;
};

extern Token_t g_tokenStart;
extern std::vector< Token_t > g_tokens;

bool ScanFile( const char * filename );