/*
========================================================================================================

Types.h

========================================================================================================
*/
#pragma once
#include <vector>
#include <string>
#include "Scanner.h"



enum VariableType_t {
	VT_INT,
	VT_FLOAT,
	VT_BOOL,
	VT_STRUCT,
	VT_ENUM,
	VT_PTR,
	//VT_CONST,
	//VT_ARRAY,
};

struct VariableTypeChain_t {
  	std::string name;	// name of struct or enum
//  	int scope;

	VariableType_t type;

	VariableTypeChain_t * next;	// This is for composite variable types... structs, integer pointers, etc
};

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
bool IsType( int idx );
bool IsType( Token_t tok );
bool IsType( std::vector< Token_t > & tokens, int & idx );
void AddType( const std::vector< Token_t > & tokens );