/*
========================================================================================================

Translate.cpp

// This should assist, as it is a short introduction to x86 assembly
https://www.cs.virginia.edu/~evans/cs216/guides/x86.html

========================================================================================================
*/
#include "Translate.h"
#include "Error.h"
#include <vector>
#include <cctype>
#include <string>

enum TokenTypeLLVM_t {
	TT_KEYWORD_DECLARE,
	TT_KEYWORD_DEFINE,
	TT_KEYWORD_INT32,
	TT_KEYWORD_ADD,
	TT_KEYWORD_SUB,
	TT_KEYWORD_MUL,
	TT_KEYWORD_DIV,
	TT_KEYWORD_RETURN,

	TT_REGISTER,
	TT_GLOBAL,
	TT_INTEGER,
	TT_FLOAT,
	TT_STRING,
	TT_WHITESPACE,
	TT_IDENTIFIER,
	TT_COMMA,

	TT_CURLYBRACKETOPEN,
	TT_CURLYBRACKETCLOSE,
	TT_PARENTHOPEN,
	TT_PARENTHCLOSE,
	TT_BRACKETOPEN,
	TT_BRACKETCLOSE,
	TT_EQUAL,
};

struct TokenLLVM_t {
	TokenTypeLLVM_t type;
	const char * str;
	//std::string str;

	int valueInt;
	float valueFloat;
};

std::vector< TokenLLVM_t > g_tokensLLVM;

/*
====================================================
AppendTokenLLVM
====================================================
*/
void AppendTokenLLVM( TokenTypeLLVM_t type, const char * str, int valueInt, float valueFloat, int col, int line ) {
	TokenLLVM_t token;
	memset( &token, 0, sizeof( token ) );

	token.type = type;
	token.str = str;

	token.valueInt = valueInt;
	token.valueFloat = valueFloat;

// 	token.coord.col = col;
// 	token.coord.ln = line;
//
// 	token.next = NULL;

	g_tokensLLVM.push_back( token );
//	g_tokensLLVM.erase( g_tokensLLVM.end() );
}

/*
====================================================
ScanStringLLVM
====================================================
*/
int ScanStringLLVM( const char * buff, int idx, const int col, const int line ) {
	int col2 = idx;
	while ( col2 < 512 ) {
		col2++;
		char d = buff[ col2 ];
		if ( d == '\"' ) {
			break;
		}
	}
	idx++;	// move the start away from the opening parenthesis

	int strlength = col2 - idx;
	char * str = (char *)malloc( strlength + 1 );
	memcpy( str, buff + idx, strlength );
	str[ strlength ] = '\0';

	AppendTokenLLVM( TT_STRING, str, 0, 0, col, line );
	
	return col2;
}

struct keywordPairLLVM_t {
	const char * str;
	TokenTypeLLVM_t type;
};

static const keywordPairLLVM_t keywordArray[] = {
	{ "declare", TT_KEYWORD_DECLARE },
	{ "define", TT_KEYWORD_DEFINE },
	{ "i32", TT_KEYWORD_INT32 },
	{ "add", TT_KEYWORD_ADD },
	{ "sub", TT_KEYWORD_SUB },
	{ "mul", TT_KEYWORD_MUL },
	{ "div", TT_KEYWORD_DIV },
	{ "ret", TT_KEYWORD_RETURN },
};

/*
====================================================
AppendKeywordLLVM
====================================================
*/
bool AppendKeywordLLVM( const char * str, int col, int line ) {
	const int numKeywords = sizeof( keywordArray ) / sizeof( keywordPairLLVM_t );

	for ( int i = 0; i < numKeywords; i++ ) {
		const keywordPairLLVM_t & keyword = keywordArray[ i ];

		if ( 0 == strcmp( str, keyword.str ) ) {
			AppendTokenLLVM( keyword.type, keyword.str, 0, 0, col, line );
			return true;
		}
	}
	return false;
}

/*
====================================================
ScanIdentifierLLVM
====================================================
*/
int ScanIdentifierLLVM( const char * buff, int idx, int col, int line ) {
	int col2 = idx;
	while ( col2 < 512 ) {
		col2++;
		char c = buff[ col2 ];
		if ( !isalnum( c ) ) {
			break;
		}
	}

	int strlength = col2 - idx;
	char * str = (char *)malloc( strlength + 1 );
	memcpy( str, buff + idx, strlength );
	str[ strlength ] = '\0';

	// Attempt to append a keyword
	bool isKeyword = AppendKeywordLLVM( str, col, line );

	// If it's not a keyword then it's an identifier
	if ( !isKeyword ) {
		AppendTokenLLVM( TT_IDENTIFIER, str, 0, 0, col, line );
	}
	
	return col2 - 1;
}

/*
====================================================
ScanNumericLLVM
====================================================
*/
int ScanNumericLLVM( const char * buff, int idx, int col, int line ) {
	int numDots = 0;
	bool isFloat = false;

	int col2 = idx;
	while ( col2 < 512 ) {
		col2++;
		char c = buff[ col2 ];

		if ( c == '.' ) {
			numDots++;
			continue;
		}

		if ( c == 'f' ) {
			isFloat = true;
			break;
		}

		if ( !isdigit( c ) ) {
			break;
		}
	}

	int len = col2 - idx;
	char * tmp = (char *)alloca( len + 1 );
	memcpy( tmp, buff + idx, len );
	tmp[ len ] = '\0';

	if ( numDots > 1 ) {
		ReportError( "Numeric contains too many dots", col, line );
		return col2 - 1;
	}

	if ( isFloat || numDots > 0 ) {
		float value = (float)atof( tmp );
		AppendTokenLLVM( TT_FLOAT, NULL, 0, value, col, line );
		return col2 - 1;
	}

	int value = atoi( tmp );
	AppendTokenLLVM( TT_INTEGER, NULL, value, 0, col, line );

	return col2 - 1;
}

/*
====================================================
ScanLLVM
====================================================
*/
void ScanLLVM( const char * llvm ) {
	g_tokensLLVM.clear();

	int col = 0;
	int line = 0;

	int idx = 0;

	while ( llvm[ idx ] ) {
		char c = llvm[ idx ];

		switch ( c ) {
			default: {
				if ( isalpha( c ) || '%' == c || '@' == c ) {
					idx = ScanIdentifierLLVM( llvm, idx, col, line );
				} else if ( isdigit( c ) ) {
					idx = ScanNumericLLVM( llvm, idx, col, line );
				} else {
					ReportError( "Unrecognized token", col, line );
				}
			} break;
			case '\"': idx = ScanStringLLVM( llvm, idx, col, line ); break;
			case '\n': col = 0; line++;
			case '\r':
			case ' ':
			case '\t':
				// Skip whitespace
				//AppendTokenLLVM( TT_WHITESPACE, NULL, 0, 0, col, line );
				break;
			case ',': AppendTokenLLVM( TT_COMMA, NULL, 0, 0, col, line ); break;
			case ';':
				// This is a comment.  Loop to the end of the line to skip it
				while ( c != '\n' ) {
					idx++;
					c = llvm[ idx ];
				}
				col = 0;
				line++;
				break;
			case '{': AppendTokenLLVM( TT_CURLYBRACKETOPEN, NULL, 0, 0, col, line ); break;
			case '}': AppendTokenLLVM( TT_CURLYBRACKETCLOSE, NULL, 0, 0, col, line ); break;
			case '(': AppendTokenLLVM( TT_PARENTHOPEN, NULL, 0, 0, col, line ); break;
			case ')': AppendTokenLLVM( TT_PARENTHCLOSE, NULL, 0, 0, col, line ); break;
			case '[': AppendTokenLLVM( TT_BRACKETOPEN, NULL, 0, 0, col, line ); break;
			case ']': AppendTokenLLVM( TT_BRACKETCLOSE, NULL, 0, 0, col, line ); break;
			case '=': AppendTokenLLVM( TT_EQUAL, NULL, 0, 0, col, line ); break;
		};

		col++;
		idx++;
	}
}





enum vartype_t {
	VT_INT32,
	VT_FLOAT
};

struct variable_t {
	vartype_t type;
	std::string name;
};

std::vector< variable_t > g_globals;

enum expType_t {
	ET_ADD,
	ET_SUB,
	ET_MUL,
	ET_DIV,
	ET_STO,	// Storage/assignment
};

enum varType_t {
	VT_GLOBAL,
	VT_CONSTANT,
	VT_REGISTER,	
};

struct varId_t {
	int idx;
	varType_t type;
};

struct expression_t {
	expType_t type;
	std::vector< varId_t > vars;
};

struct function_t {
	std::string name;

	std::vector< variable_t > registers;

	std::vector< expression_t > expressions;
};

std::vector< function_t > g_functions;

std::vector< TokenLLVM_t > g_constants;

/*
====================================================
FindVariable
====================================================
*/
varId_t FindVariable( TokenLLVM_t tok, function_t & func ) {
	varId_t vid;
	vid.idx = -1;
	vid.type = VT_GLOBAL;

	// Check for a constant
	if ( tok.type == TT_INTEGER ) {
		for ( int i = 0; i < g_constants.size(); i++ ) {
			const TokenLLVM_t & c = g_constants[ i ];
			if ( c.valueInt == tok.valueInt ) {
				vid.idx = i;
				vid.type = VT_CONSTANT;
				return vid;
			}
		}

		
		vid.idx = (int)g_constants.size();
		vid.type = VT_CONSTANT;
		g_constants.push_back( tok );
		return vid;
	}

	if ( tok.str[ 0 ] == '@' ) {
		// Variable is labeled as a global, find it
		for ( int i = 0; i < g_globals.size(); i++ ) {
			const variable_t & var = g_globals[ i ];
			if ( var.name == tok.str ) {
				vid.idx = i;
				vid.type = VT_GLOBAL;
				return vid;
			}
		}
		ReportError( "Couldn't find global variable", 0, 0 );
	} else if ( tok.str[ 0 ] == '%' ) {
		// Variable is labeled as a local, find it
		for ( int i = 0; i < func.registers.size(); i++ ) {
			const variable_t & var = func.registers[ i ];
			if ( var.name == tok.str ) {
				vid.idx = i;
				vid.type = VT_REGISTER;
				return vid;
			}
		}

		// Must be a generated register, so add it
		vid.idx = (int)func.registers.size();
		vid.type = VT_REGISTER;

		variable_t var;
		var.name = tok.str;
		var.type = VT_INT32;
		func.registers.push_back( var );		
	} else {
		ReportError( "Variable has invalid naming convention", 0, 0 );
	}
	
	return vid;
}

/*
====================================================
ParseFunction
====================================================
*/
int ParseFunction( int idx ) {
	TokenLLVM_t tok = g_tokensLLVM[ idx ];

	if ( tok.type != TT_KEYWORD_DEFINE ) {
		ReportError( "Expected define keyword", 0, 0 );
	}

	// Get the return type
	idx++;
	tok = g_tokensLLVM[ idx ];

	if ( tok.type == TT_KEYWORD_INT32 ) {
		//var.type = VT_INT32;
	} else {
		ReportError( "Expected type keyword", 0, 0 );
	}

	// Get the function name
	idx++;
	tok = g_tokensLLVM[ idx ];

	if ( tok.type != TT_IDENTIFIER ) {
		ReportError( "Expected identifier", 0, 0 );
	}

	function_t func;
	func.name = tok.str;

	idx++;
	tok = g_tokensLLVM[ idx ];
	if ( tok.type != TT_PARENTHOPEN ) {
		ReportError( "Expected opening parenthesis", 0, 0 );
	}

	// TODO: Parse function parameters

	idx++;
	tok = g_tokensLLVM[ idx ];
	if ( tok.type != TT_PARENTHCLOSE ) {
		ReportError( "Expected closing parenthesis", 0, 0 );
	}

	idx++;
	tok = g_tokensLLVM[ idx ];
	if ( tok.type != TT_CURLYBRACKETOPEN ) {
		ReportError( "Expected open curly bracket", 0, 0 );
	}

	idx++;
	tok = g_tokensLLVM[ idx ];

	while ( tok.type != TT_CURLYBRACKETCLOSE ) {
		switch ( tok.type ) {
			case TT_KEYWORD_DECLARE: {
				// TODO: add local variable declaration
			} break;
			case TT_KEYWORD_RETURN: {
				// TODO: add a return expression
				idx += 2;
			} break;
			default: {
				// Parse expression
				if ( tok.type != TT_IDENTIFIER ) {
					ReportError( "Expected identifier", 0, 0 );
				}
				varId_t vid0 = FindVariable( tok, func );

				idx++;
				tok = g_tokensLLVM[ idx ];
				if ( tok.type != TT_EQUAL ) {
					ReportError( "Expected assignment operator", 0, 0 );
				}

				idx++;
				tok = g_tokensLLVM[ idx ];
				if ( tok.type == TT_IDENTIFIER ) {
					// This is purely assignment operation
					varId_t vid1 = FindVariable( tok, func );
					
					// Create an assignment expression and append it
					expression_t expression;
					expression.type = ET_STO;
					expression.vars.push_back( vid0 );
					expression.vars.push_back( vid1 );
					func.expressions.push_back( expression );
					break;
				} else {
					expType_t expType;
					switch ( tok.type ) {
						default: ReportError( "Unexpected token", 0, 0 ); break;
						case TT_KEYWORD_ADD: expType = ET_ADD; break;
						case TT_KEYWORD_SUB: expType = ET_SUB; break;
						case TT_KEYWORD_MUL: expType = ET_MUL; break;
						case TT_KEYWORD_DIV: expType = ET_DIV; break;
					}

					idx++;
					tok = g_tokensLLVM[ idx ];	// TODO: check i32

					idx++;
					tok = g_tokensLLVM[ idx ];
					varId_t vid1 = FindVariable( tok, func );

					idx++;
					tok = g_tokensLLVM[ idx ];	// TODO: check ,

					idx++;
					tok = g_tokensLLVM[ idx ];
					varId_t vid2 = FindVariable( tok, func );

					expression_t expression;
					expression.type = expType;
					expression.vars.push_back( vid0 );
					expression.vars.push_back( vid1 );
					expression.vars.push_back( vid2 );
					func.expressions.push_back( expression );
				}
			} break;

		}

		idx++;
		tok = g_tokensLLVM[ idx ];
	}

	g_functions.push_back( func );
	
	return idx;
}

/*
====================================================
ParseGlobalDeclaration
====================================================
*/
int ParseGlobalDeclaration( int idx ) {
	TokenLLVM_t tok = g_tokensLLVM[ idx ];

	if ( tok.type != TT_KEYWORD_DECLARE ) {
		ReportError( "Expected declare keyword", 0, 0 );
	}

	idx++;
	tok = g_tokensLLVM[ idx ];

	variable_t var;

	if ( tok.type == TT_KEYWORD_INT32 ) {
		var.type = VT_INT32;
	} else {
		ReportError( "Expected type keyword", 0, 0 );
	}

	idx++;
	tok = g_tokensLLVM[ idx ];

	if ( tok.type != TT_IDENTIFIER ) {
		ReportError( "Expected identifier", 0, 0 );
	}

	var.name = tok.str;

	g_globals.push_back( var );

	return idx;
}

/*
====================================================
ParseLLVM
====================================================
*/
void ParseLLVM() {
	int idx = 0;
	while ( idx < g_tokensLLVM.size() ) {
		TokenLLVM_t tok = g_tokensLLVM[ idx ];

		if ( tok.type == TT_KEYWORD_DECLARE ) {
			idx = ParseGlobalDeclaration( idx );
		} else if ( tok.type == TT_KEYWORD_DEFINE ) {
			idx = ParseFunction( idx );
		}

		idx++;
	}
}

/*
====================================================
TranslateLLVMtoX86
====================================================
*/
void TranslateLLVMtoX86( const char * llvm ) {
	ScanLLVM( llvm );
	ParseLLVM();

	// Now that we have LLVM parsed, it needs to be translated to x86 assembly
}