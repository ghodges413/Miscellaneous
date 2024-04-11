/*
========================================================================================================

Types.cpp

========================================================================================================
*/
#include "Types.h"

TypeTable_t g_typeTable;

/*
====================================================
IsType
====================================================
*/
bool IsType( Token_t tok ) {
	if ( TT_KEYWORD_INT == tok.type ) {
		return true;
	}
	if ( TT_KEYWORD_FLOAT == tok.type ) {
		return true;
	}
	return false;
}
bool IsType( int idx ) {
	return IsType( g_tokens[ idx ] );
}

bool IsType( std::vector< Token_t > & tokens, int & idx ) {
	return false;
}


void AddType( const std::vector< Token_t > & tokens ) {
}