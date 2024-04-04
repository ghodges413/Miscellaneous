//
//  main.cpp
//  A mini c interpreter
//
#include <iostream>
#include <fstream>
#include <new>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include "Scanner.h"
#include "Parser.h"



// 1. The scanner converts the raw text into tokens
// 2. The parser converts the tokens to the abstract syntax tree (AST) - bonus output code formed form AST to check that it is working
// 3. Semantic Analysis - The AST is checked for errors
//		|-> Symbol Table
//		|-> Name Resolution
//		|-> Type checking
// 4. The AST is converted into an intermediate representation (IR)
// 5. The IR is passed through multiple optimization phases	(we may want to consider using the same IR as llvm)
// 6. The IR is converted to the target assembly language

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
	if ( argc != 2 ) {
		std::cout << "Usage: minicpp <filename>\n";
		return 1;
	}

	ScanFile( argv[ 1 ] );
	Parser();
	return 0;
}
