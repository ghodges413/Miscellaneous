/*
========================================================================================================

Parser.h

========================================================================================================
*/
#pragma once
#include <vector>
#include <string>

/*
====================================================
Token_t
====================================================
*/

void Parser();


void PreParse();

int FindFunction( const char * name );

void GenerateLLVM();



// This data structure should eventually get replaced by a SymbolTable
struct Variable_t {
	std::string name;
	//TokenInternalRepresentation_t v_type; // data type	// for now, assume all variables are integers
	// eventually the above will be a type from the type table

	int value; // value
};
extern std::vector< Variable_t > g_globalVariables;

extern std::vector< Variable_t > g_localVariables;


struct Function_t {
	std::string name;
	//TokenInternalRepresentation_t ret_type; // return type
	// 	   for now, assume a return type of int
	//char * loc; // location of entry point in program
	int location;	// idx to the name of the function in the token list

	// TODO: Add the parameters/arguments too
};
extern std::vector< Function_t > g_functionTable;