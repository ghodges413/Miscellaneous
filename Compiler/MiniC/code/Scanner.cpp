/*
========================================================================================================

Scanner.cpp

========================================================================================================
*/
#include "Scanner.h"
#include "Fileio.h"
#include "Error.h"
#include <vector>
#include <cctype>

Token_t g_tokenStart;
Token_t * g_tokenEnd;

std::vector< Token_t > g_tokens;

/*
====================================================
AppendToken
====================================================
*/
void AppendToken( TokenType_t type, const char * str, int valueInt, float valueFloat, int col, int line ) {
	Token_t token;
	memset( &token, 0, sizeof( token ) );

	token.type = type;
	token.str = str;

	token.valueInt = valueInt;
	token.valueFloat = valueFloat;

	token.coord.col = col;
	token.coord.ln = line;

	token.next = NULL;

	g_tokens.push_back( token );
}

/*
====================================================
ScanString
====================================================
*/
int ScanString( const char * buff, int col, int line ) {
	int col2 = col;
	while ( col2 < 512 ) {
		col2++;
		char d = buff[ col2 ];
		if ( d == '\"' ) {
			break;
		}
	}
	col++;	// move the start away from the opening parenthesis

	int strlength = col2 - col;
	char * str = (char *)malloc( strlength + 1 );
	memcpy( str, buff + col, strlength );
	str[ strlength ] = '\0';

	AppendToken( TT_STRING, str, 0, 0, col, line );
	
	return col2;
}

// const char * keywordArray[] = {
// 	"while",
// 	"for",
// 	"continue",
// 	"break",
// 	"do",
// 	"if",
// 	"else",
// 	"switch",
// 	"case",
// 	"default",
// 	"return",
// 	"false",
// 	"true",
// 	"bool",
// 	"int",
// 	"float",
// 	"double",
// 	"char",
// 	"short",
// 	"struct",
// 	"enum",
// 	"const",
// 	"sizeof",
// };

struct keywordPair_t {
	const char * str;
	TokenType_t type;
};

// bool IsKeyword( const char * str ) {
// 	const int numKeywords = sizeof( keywordArray ) / sizeof( const char * );
// 
// 	for ( int i = 0; i < numKeywords; i++ ) {
// 		const char * keyword = keywordArray[ i ];
// 
// 		if ( 0 == strcmp( str, keyword ) ) {
// 			return true;
// 		}
// 	}
// 	return false;
// }

const keywordPair_t keywordArray[] = {
	{ "while", TT_KEYWORD_WHILE },
	{ "for", TT_KEYWORD_FOR },
	{ "continue", TT_KEYWORD_CONTINUE },
	{ "break", TT_KEYWORD_BREAK },
	{ "do", TT_KEYWORD_DO },
	{ "if", TT_KEYWORD_IF },
	{ "else", TT_KEYWORD_ELSE },
	{ "switch", TT_KEYWORD_SWITCH },
	{ "case", TT_KEYWORD_CASE },
	{ "default", TT_KEYWORD_DEFAULT },
	{ "return", TT_KEYWORD_RETURN },
	{ "false", TT_KEYWORD_FALSE },
	{ "true", TT_KEYWORD_TRUE },
	{ "bool", TT_KEYWORD_BOOL },
	{ "int", TT_KEYWORD_INT },
	{ "float", TT_KEYWORD_FLOAT },
	{ "char", TT_KEYWORD_CHAR },
	{ "short", TT_KEYWORD_SHORT },
	{ "struct", TT_KEYWORD_STRUCT },
	{ "enum", TT_KEYWORD_ENUM },
	{ "const", TT_KEYWORD_CONST },
	{ "sizeof", TT_KEYWORD_SIZEOF },
	{ "typedef", TT_KEYWORD_TYPEDEF },
};

static bool AppendKeyword( const char * str, int col, int line ) {
	const int numKeywords = sizeof( keywordArray ) / sizeof( keywordPair_t );

	for ( int i = 0; i < numKeywords; i++ ) {
		const keywordPair_t & keyword = keywordArray[ i ];

		if ( 0 == strcmp( str, keyword.str ) ) {
			AppendToken( keyword.type, keyword.str, 0, 0, col, line );
			return true;
		}
	}
	return false;
}

/*
====================================================
ScanIdentifier
====================================================
*/
int ScanIdentifier( const char * buff, int col, int line ) {
	int col2 = col;
	while ( col2 < 512 ) {
		col2++;
		char c = buff[ col2 ];
		if ( !isalnum( c ) ) {
			break;
		}
	}

	int strlength = col2 - col;
	char * str = (char *)malloc( strlength + 1 );
	memcpy( str, buff + col, strlength );
	str[ strlength ] = '\0';

	// Attempt to append a keyword
	bool isKeyword = AppendKeyword( str, col, line );

	// If it's not a keyword then it's an identifier
	if ( !isKeyword ) {
		AppendToken( TT_IDENTIFIER, str, 0, 0, col, line );
	}
	
	return col2 - 1;
}

/*
====================================================
ScanNumeric
====================================================
*/
int ScanNumeric( const char * buff, int col, int line ) {
	int numDots = 0;
	bool isFloat = false;

	int col2 = col;
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

	int len = col2 - col;
	char * tmp = (char *)alloca( len + 1 );
	memcpy( tmp, buff + col, len );
	tmp[ len ] = '\0';

	if ( numDots > 1 ) {
		ReportError( "Numeric contains too many dots", col, line );
		return col2 - 1;
	}

	if ( isFloat || numDots > 0 ) {
		float value = (float)atof( tmp );
		AppendToken( TT_FLOAT, NULL, 0, value, col, line );
		return col2 - 1;
	}

	int value = atoi( tmp );
	AppendToken( TT_INTEGER, NULL, value, 0, col, line );

	return col2 - 1;
}

/*
====================================================
ScanLine
====================================================
*/
bool ScanLine( const char * buff, const int line ) {
	int col = 0;

	while ( buff[ col ] != 0 && col < 512 ) {
		char c = buff[ col ];

		switch ( c ) {
			default: {
				if ( isalpha( c ) ) {
					col = ScanIdentifier( buff, col, line );
				} else if ( isdigit( c ) ) {
					col = ScanNumeric( buff, col, line );
				} else {
					ReportError( "Unrecognized token", col, line );
				}
			} break;
			case '\"': col = ScanString( buff, col, line ); break;
			case ' ':
			case '\n':
			case '\r':
			case '\t':
				// Skip whitespace
				//AppendToken( TT_WHITESPACE, NULL, 0, 0, col, line );
				break;
			case ',': AppendToken( TT_COMMA, NULL, 0, 0, col, line ); break;
			case ';': AppendToken( TT_SEMICOLON, NULL, 0, 0, col, line ); break;
			case '.': AppendToken( TT_DOT, NULL, 0, 0, col, line ); break;
			case '{': AppendToken( TT_CURLYBRACKETOPEN, NULL, 0, 0, col, line ); break;
			case '}': AppendToken( TT_CURLYBRACKETCLOSE, NULL, 0, 0, col, line ); break;
			case '(': AppendToken( TT_PARENTHOPEN, NULL, 0, 0, col, line ); break;
			case ')': AppendToken( TT_PARENTHCLOSE, NULL, 0, 0, col, line ); break;
			case '[': AppendToken( TT_BRACKETOPEN, NULL, 0, 0, col, line ); break;
			case ']': AppendToken( TT_BRACKETCLOSE, NULL, 0, 0, col, line ); break;
			case '%': AppendToken( TT_MODULUS, NULL, 0, 0, col, line ); break;
			case '*': {
				// Peak the next character
				if ( buff[ col + 1 ] == '=' ) {
					AppendToken( TT_TIMESEQUAL, NULL, 0, 0, col, line );
					col++;
				} else {
					AppendToken( TT_STAR, NULL, 0, 0, col, line );
				}
			} break;
			case '+': {
				// Peak the next character
				if ( buff[ col + 1 ] == '=' ) {
					AppendToken( TT_PLUSEQUAL, NULL, 0, 0, col, line );
					col++;
				} else if ( buff[ col + 1 ] == '+' ) {
					AppendToken( TT_INCREMENT, NULL, 0, 0, col, line );
					col++;
				} else {
					AppendToken( TT_PLUS, NULL, 0, 0, col, line );
				}
			} break;
			case '/': {
				// Peak the next character
				if ( buff[ col + 1 ] == '=' ) {
					AppendToken( TT_DIVIDEEQUAL, NULL, 0, 0, col, line );
					col++;
				} else if ( buff[ col + 1 ] == '/' ) {
					// This a line comment, so just skip the rest of the line
					return true;
				} else {
					AppendToken( TT_DIVISION, NULL, 0, 0, col, line );
				}
			} break;
			case '-': {
				// Peak the next character
				if ( buff[ col + 1 ] == '=' ) {
					AppendToken( TT_MINUSEQUAL, NULL, 0, 0, col, line );
					col++;
				} else if ( buff[ col + 1 ] == '>' ) {
					AppendToken( TT_POINTER, NULL, 0, 0, col, line );
					col++;
				} else if ( buff[ col + 1 ] == '-' ) {
					AppendToken( TT_DECREMENT, NULL, 0, 0, col, line );
					col++;
				} else {
					AppendToken( TT_MINUS, NULL, 0, 0, col, line );
				}
			} break;
			case '<': {
				// Peak the next character
				if ( buff[ col + 1 ] == '=' ) {
					AppendToken( TT_LESSEQUAL, NULL, 0, 0, col, line );
					col++;
				} else if ( buff[ col + 1 ] == '<' ) {
					if ( buff[ col + 2 ] == '=' ) {
						AppendToken( TT_SHIFTLEFTEQUAL, NULL, 0, 0, col, line );
						col++;
					} else {
						AppendToken( TT_SHIFTLEFT, NULL, 0, 0, col, line );
					}
					col++;
				} else {
					AppendToken( TT_BRA, NULL, 0, 0, col, line );
				}
			} break;
			case '>': {
				// Peak the next character
				if ( buff[ col + 1 ] == '=' ) {
					AppendToken( TT_GREATEQUAL, NULL, 0, 0, col, line );
					col++;
				} else if ( buff[ col + 1 ] == '>' ) {
					if ( buff[ col + 2 ] == '=' ) {
						AppendToken( TT_SHIFTRIGHTEQUAL, NULL, 0, 0, col, line );
						col++;
					} else {
						AppendToken( TT_SHIFTRIGHT, NULL, 0, 0, col, line );
					}
					col++;
				} else {
					AppendToken( TT_KET, NULL, 0, 0, col, line );
				}
			} break;
			case '!': {
				// Peak the next character
				if ( buff[ col + 1 ] == '=' ) {
					AppendToken( TT_NOTEQUAL, NULL, 0, 0, col, line );
					col++;
				} else {
					AppendToken( TT_NOT, NULL, 0, 0, col, line );
				}
			} break;
			case '=': {
				// Peak the next character
				if ( buff[ col + 1 ] == '=' ) {
					AppendToken( TT_COMPARISON, NULL, 0, 0, col, line );
					col++;
				} else {
					AppendToken( TT_ASSIGNMENT, NULL, 0, 0, col, line );
				}
			} break;
			case '&': {
				if ( buff[ col + 1 ] == '&' ) {
					AppendToken( TT_AND, NULL, 0, 0, col, line );
					col++;
				} else {
					AppendToken( TT_BITAND, NULL, 0, 0, col, line );
				}
			} break;
			case '|': {
				if ( buff[ col + 1 ] == '|' ) {
					AppendToken( TT_OR, NULL, 0, 0, col, line );
					col++;
				} else {
					AppendToken( TT_BITOR, NULL, 0, 0, col, line );
				}
			} break;
			case '^': AppendToken( TT_BITXOR, NULL, 0, 0, col, line ); break;
			case '~': AppendToken( TT_BITNOT, NULL, 0, 0, col, line ); break;
		};

		col++;		
	}

	return true;
}

/*
====================================================
RemoveWhiteSpace
====================================================
*/
void RemoveWhiteSpace() {
	std::vector< Token_t > tokensTmp;
	tokensTmp.reserve( g_tokens.size() );

	for ( int i = 0; i < g_tokens.size(); i++ ) {
		const Token_t & tok = g_tokens[ i ];

		if ( TokenType_t::TT_WHITESPACE != tok.type ) {
			tokensTmp.push_back( tok );
		}
	}

	g_tokens = tokensTmp;
}

/*
====================================================
ScanFile
====================================================
*/
bool ScanFile( const char * fileName ) {
	char buff[ 512 ] = { 0 };

	char fullPath[ 2048 ];
	RelativePathToFullPath( fileName, fullPath );	
	
	FILE * fp = fopen( fullPath, "rb" );
	if ( !fp ) {
		fprintf( stderr, "Error: couldn't open \"%s\"!\n", fullPath );
		return false;
    }

	int line = 0;

	while ( !feof( fp ) ) {
		// Reset buffer
		memset( buff, 0, sizeof( buff ) );

		// Read whole line
		fgets( buff, sizeof( buff ), fp );

		ScanLine( buff, line );
		line++;
	}

	fclose( fp );

	// Alternatively, instead of reading a whole line at a time, you could go a single character at a time:
	//	File * fp = fopen( fullPath, "rb" );
	//	int c = fgetc( fp );
	//	ungetc( c, fp );

	//RemoveWhiteSpace();

	return true;
}

