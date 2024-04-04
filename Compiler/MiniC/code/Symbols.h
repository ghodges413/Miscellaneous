/*
========================================================================================================

Symbols.h

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

/*
====================================================
====================================================
*/
enum SymbolType_t {
	ST_CONSTANT,
	ST_VARIABLE,
	ST_FUNCTION,
};

// What is a symbol?  It's a variable, for sure.  Is it also a function declaration?  Probably not?  Actually, probably yes
struct Symbol_t {
	Symbol_t() { type = ST_CONSTANT; constantValue = 0; }
	SymbolType_t type;

	// Type type;	// TODO: Add support for multiple types, not just integers
	std::string name;

	int constantValue;

// 	int scope;	// -1, for global or 0 for global?
// 
// 	std::string functionName;
// 	struct FunctionDeclaration_t * function;	// pointer to function that it belongs to? (if it's a local variable)

	VariableTypeChain_t * varTypeChain; // The type for a variable, it could also be the return type for a function
};

struct SymbolTable2_t {
	// TODO:	Replace with a hash table?
	//			Brute force for now is okay
	std::vector< Symbol_t * > symbols;
};
extern SymbolTable2_t g_symbolTable;

/*
====================================================
====================================================
*/
Symbol_t * FindSymbol( const char * symbolName );
Symbol_t * FindConstant( int value );
bool AddFunctionToSymbolTable( const char * name );
bool AddConsantToSymbolTable( const int value );
bool AddVariableToSymbolTable( const char * name );







