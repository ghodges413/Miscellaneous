/*
========================================================================================================

Symbols.h

========================================================================================================
*/
#pragma once
#include <vector>
#include <string>
#include "Scanner.h"


/*
====================================================
====================================================
*/
enum SymbolType_t {
	ST_CONSTANT_INT,
	ST_CONSTANT_FLOAT,
	ST_VARIABLE,
	ST_FUNCTION,
};

// What is a symbol?  It's a variable, for sure.  Is it also a function declaration?  Probably not?  Actually, probably yes
struct Symbol_t {
	Symbol_t() { type = ST_CONSTANT_INT; constantValueInt = 0; constantValueFloat = 0; }
	SymbolType_t type;

	// Type type;	// TODO: Add support for multiple types, not just integers
	std::string name;

	int constantValueInt;
	float constantValueFloat;

// 	int scope;	// -1, for global or 0 for global?
// 
// 	std::string functionName;
// 	struct FunctionDeclaration_t * function;	// pointer to function that it belongs to? (if it's a local variable)

//	VariableTypeChain_t * varTypeChain; // The type for a variable, it could also be the return type for a function
};

struct SymbolTable_t {
	// TODO:	Replace with a hash table?
	//			Brute force for now is okay
	std::vector< Symbol_t * > symbols;
};
extern SymbolTable_t g_symbolTable;

/*
====================================================
====================================================
*/
Symbol_t * FindSymbol( const char * symbolName );
Symbol_t * FindConstantInt( int value );
Symbol_t * FindConstantFloat( float value );
bool AddFunctionToSymbolTable( const char * name );
bool AddConsantToSymbolTableInt( const int value );
bool AddConsantToSymbolTableFloat( const float value );
bool AddVariableToSymbolTable( const char * name );







