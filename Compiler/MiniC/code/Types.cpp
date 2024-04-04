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
	if ( tok.type == TT_KEYWORD_INT ) {
		return true;
	}
	return false;
}

bool IsType( std::vector< Token_t > & tokens, int & idx ) {
	return false;
}


void AddType( const std::vector< Token_t > & tokens ) {
}