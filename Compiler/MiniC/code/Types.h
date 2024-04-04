/*
========================================================================================================

Types.h

========================================================================================================
*/
#pragma once
#include <vector>
#include <string>
#include "Scanner.h"


enum BuiltInType_t {
	BIT_CHAR,
	BIT_INT,
	BIT_UNSIGNED,
	BIT_SHORT,
	BIT_LONG,
	BIT_ENUM,
	BIT_FLOAT, 
	BIT_DOUBLE, 
	BIT_ARRAY,
	BIT_STRUCT, 
	BIT_UNION,
	BIT_POINTER,
	BIT_FUNCTION, 
	BIT_CONST,
	BIT_VOLATILE,
	BIT_VOID,
};

struct Type_t {
	BuiltInType_t bit;
	Type_t * next;	// used to chain together multiple built-ins
};

struct TypeTable_t {
	std::vector< Type_t > types;
};

extern TypeTable_t g_typeTable;

/*
====================================================
====================================================
*/
bool IsType( Token_t tok );
bool IsType( std::vector< Token_t > & tokens, int & idx );
void AddType( const std::vector< Token_t > & tokens );