/*
========================================================================================================

Parser.cpp

========================================================================================================
*/
#include "Scanner.h"
#include "Parser.h"
#include "Symbols.h"
#include "Types.h"
#include "Fileio.h"
#include "Error.h"
#include "Translate.h"
#include "AST.h"
#include <vector>
#include <cctype>
#include <string>



/*
========================================================================================================

LLVM Generation

========================================================================================================
*/

const std::string indent = "    ";

/*
====================================================
GenerateExpression_r
====================================================
*/
std::string GenerateExpression_r( const ExpressionNode_t * expresson_node, std::string & llvm_code, int & register_count ) {
	if ( NULL == expresson_node ) {
		ReportError( "Bad Expression in Abstract Syntax Tree\n", 0, 0 );
		return "";
	}

	if ( expresson_node->symbol ) {
		std::string name;
		if ( ST_VARIABLE == expresson_node->symbol->type ) {
			// check if it's global vs local
			name += "@";
			name += expresson_node->symbol->name;
		}
		if ( ST_CONSTANT_INT == expresson_node->symbol->type ) {
			char tmp[ 32 ] = { 0 };
			sprintf( tmp, "%i", expresson_node->symbol->constantValueInt );
			name = tmp;
		}
		if ( ST_CONSTANT_FLOAT == expresson_node->symbol->type ) {
			char tmp[ 32 ] = { 0 };
			sprintf( tmp, "%f", expresson_node->symbol->constantValueFloat );
			name = tmp;
		}
		return name;
	}

	if ( OT_ASSIGNMENT == expresson_node->operation ) {
		// check if it's global vs local
		std::string nameRight = GenerateExpression_r( expresson_node->expressionRight, llvm_code, register_count );

		// check if it's global vs local
		std::string nameLeft = GenerateExpression_r( expresson_node->expressionLeft, llvm_code, register_count );

		llvm_code += indent;
		llvm_code += nameLeft;
		llvm_code += " = ";
		llvm_code += nameRight;
		llvm_code += "\n";

		// return the assigned variable
		return nameLeft;
	}

	//if ( OT_ADDITION == expresson_node->operation ) 
	{
		// check if it's global vs local
		std::string codeRight;
		std::string nameRight = GenerateExpression_r( expresson_node->expressionRight, codeRight, register_count );

		// check if it's global vs local
		std::string codeLeft;
		std::string nameLeft = GenerateExpression_r( expresson_node->expressionLeft, codeLeft, register_count );

		llvm_code += codeRight;
		llvm_code += codeLeft;

		// return the assigned variable
		char name[ 32 ] = { 0 };
		sprintf( name, "%%%i", register_count );
		register_count++;

		std::string name0 = name;

		std::string line;
		line = indent;
		line += name0;
		switch ( expresson_node->operation ) {
			default:
			case OT_ADDITION:       line += " = add i32 "; break;
			case OT_SUBTRACTION:    line += " = sub i32 "; break;
			case OT_MULTIPLICATION: line += " = mul i32 "; break;
			case OT_DIVISION:       line += " = div i32 "; break;
		}
		line += nameLeft;
		line += ", ";
		line += nameRight;
		line += "\n";

		llvm_code += line;

		return name0;
	}

	ReportError( "Unsupported operation in Abstract Syntax Tree\n", 0, 0 );
	return "";
}

/*
====================================================
GenerateFunction
====================================================
*/
void GenerateFunction( const FunctionDeclaration_t * functionNode, std::string & llvm_code ) {
	llvm_code += "\n\n";
	if ( functionNode->returnType.bit == BIT_INT ) {
		llvm_code += "define i32 @";
	}
	if ( functionNode->returnType.bit == BIT_FLOAT ) {
		llvm_code += "define f32 @";
	}
	llvm_code += functionNode->name;
	llvm_code += "() {\n";

	int register_count = 0;

	const GenericNode_t * current_node = functionNode->next;

	while ( NULL != current_node ) {
		if ( current_node->expression ) {
			GenerateExpression_r( current_node->expression, llvm_code, register_count );
			llvm_code += "\n";
		}

		current_node = current_node->next;
	}

	llvm_code += indent;
	if ( functionNode->returnType.bit == BIT_INT ) {
		llvm_code += "ret i32 0\n";
	}
	if ( functionNode->returnType.bit == BIT_FLOAT ) {
		llvm_code += "ret f32 0\n";;
	}
	llvm_code += "}\n";
	llvm_code += "\n";
}

/*
====================================================
GenerateLLVM
Convert the AST to LLVM
LLVM reference manual: https://llvm.org/docs/LangRef.html
====================================================
*/
void GenerateLLVM() {
	// Globals start with an '@' and locals/registers start with a '%'
	// Comments start with ';'
	// Temporaries are generated for intermediary values
	// Temporaries are numbered, starting at 0
	// Examples:
	// %result = mul i32 %x, 8		; multiplies x by 8 and stores it in result
	// %0 = add i32 %x, %x
	// %1 = add i32 %0, %0
	// %result = add i32 %1, %1

	// This is an example of an llvm module
	/*
	; Declare the string constant as a global constant.
	@.str = private unnamed_addr constant [13 x i8] c"hello world\0A\00"

	; External declaration of the puts function
	declare i32 @puts(ptr nocapture) nounwind

	; Definition of main function
	define i32 @main() {
	  ; Call puts function to write out the string to stdout.
	  call i32 @puts(ptr @.str)
	  ret i32 0
	}

	; Named metadata
	!0 = !{i32 42, null, !"string"}
	!foo = !{!0}
	*/

	std::string llvm_code;

	for ( int i = 0; i < g_symbolTable.symbols.size(); i++ ) {
		const Symbol_t * sym = g_symbolTable.symbols[ i ];

		if ( ST_VARIABLE == sym->type ) {
			llvm_code += "declare i32 @";
			llvm_code += sym->name;
			llvm_code += "\n";
		}
	}

	const GenericNode_t * const head = g_ast;
	const GenericNode_t * current_node = head;

	while ( NULL != current_node ) {
		if ( current_node->function ) {
			GenerateFunction( current_node->function, llvm_code );
		}

		current_node = current_node->next;
	}

	TranslateLLVMtoX86( llvm_code.c_str() );
}