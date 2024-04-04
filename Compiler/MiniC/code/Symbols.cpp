/*
========================================================================================================

Symbols.cpp

========================================================================================================
*/
#include "Symbols.h"

SymbolTable2_t g_symbolTable;

/*
====================================================
FindSymbol
====================================================
*/
Symbol_t * FindSymbol( const char * symbolName ) {
	// If function is NULL then we're looking for a global variable or a function
	// If function is not NULL then we're looking for local variable

	// To make our life easier all symbols have unique names for right now
	for ( int i = 0; i < g_symbolTable.symbols.size(); i++ ) {
		Symbol_t * sym = g_symbolTable.symbols[ i ];

		// if symbol name doesn't match, continue
		if ( 0 == strcmp( sym->name.c_str(), symbolName ) ) {
			return sym;
		}
	}
	return NULL;
}

/*
====================================================
FindConstant
====================================================
*/
Symbol_t * FindConstant( int value ) {
	for ( int i = 0; i < g_symbolTable.symbols.size(); i++ ) {
		Symbol_t * sym = g_symbolTable.symbols[ i ];
		if ( ST_CONSTANT != sym->type ) {
			continue;
		}

		if ( sym->constantValue == value ) {
			return sym;
		}
	}

	// if we get here, then the constant doesn't exist in the symbol table yet, so add i
	Symbol_t * sym = new Symbol_t;
	sym->type = ST_CONSTANT;
	sym->constantValue = value;
	g_symbolTable.symbols.push_back( sym );
	return sym;
}

/*
====================================================
AddFunctionToSymbolTable
====================================================
*/
bool AddFunctionToSymbolTable( const char * name ) {
	Symbol_t * sym = FindSymbol( name );
	if ( NULL == sym ) {
		sym = new Symbol_t;
		sym->type = ST_FUNCTION;
		sym->name = name;
		g_symbolTable.symbols.push_back( sym );
		return true;
	}

	return false;
}

/*
====================================================
AddConsantToSymbolTable
====================================================
*/
bool AddConsantToSymbolTable( const int value ) {
	FindConstant( value );
	return true;
}

/*
====================================================
AddVariableToSymbolTable
====================================================
*/
bool AddVariableToSymbolTable( const char * name ) {
	Symbol_t * sym = FindSymbol( name );
	if ( NULL == sym ) {
		sym = new Symbol_t;
		sym->type = ST_VARIABLE;
		sym->name = name;
		g_symbolTable.symbols.push_back( sym );
		return true;
	}

	return false;
}