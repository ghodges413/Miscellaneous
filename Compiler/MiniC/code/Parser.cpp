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
#include <vector>
#include <cctype>
#include <string>

/*
========================================================================================================

AST Parsing

========================================================================================================
*/
enum NodeType_t {
	NT_NULL,
	NT_FUNCTION,
	NT_EXPRESSION,
	NT_STATEMENT,
	NT_GENERIC,	// only used for the starting node?
};




struct FunctionDeclaration_t {
	FunctionDeclaration_t() { next = NULL; }
	std::string name;

	// TODO: add parameter list and return type
	// parameters should probably be pointers to symbols
	// the symbols of parameters should probably be considered local variables of the function

	struct GenericNode_t * next;
};

enum OperationType_t {
	OT_NULL,		// When an expression node is only a sigle variable or constant
	OT_ASSIGNMENT,
	OT_LESSTHAN,
	OT_GREATERTHAN,
	OT_LESSEQUAL,
	OT_GREATEREQUAL,
	OT_ISEQUAL,
	OT_NOTEQUAL,
	OT_ADDITION,
	OT_SUBTRACTION,
	OT_MULTIPLICATION,
	OT_DIVISION,
	OT_MODULUS,
};

struct ExpressionNode_t {
	ExpressionNode_t() { memset( this, 0, sizeof( ExpressionNode_t ) ); }

	Symbol_t * symbol;	// Null, unless terminating node

	// These are not Null, unless terminating node
	ExpressionNode_t * expressionLeft;
	ExpressionNode_t * expressionRight;
	OperationType_t operation;
};

enum StatementType_t {
	ST_STATEMENT_IF,
	ST_STATEMENT_WHILE,
};
struct StatementNode_t {
	StatementType_t type;
	ExpressionNode_t * expression;
};

// Generic nodes are for when we don't know what the next node will be.
// An example is the first node in the tree.  As that might be a function or a variable declaration.
struct GenericNode_t {
	GenericNode_t() { memset( this, 0, sizeof( GenericNode_t ) ); }
	StatementNode_t * statement;
	ExpressionNode_t * expression;
	FunctionDeclaration_t * function;

	GenericNode_t * next;
};
GenericNode_t * g_ast = NULL;



ExpressionNode_t * ParseExpression1( int & idx );
ExpressionNode_t * ParseExpression2( int & idx );
ExpressionNode_t * ParseExpression3( int & idx );
ExpressionNode_t * ParseExpression4( int & idx );
ExpressionNode_t * ParseExpression5( int & idx );
ExpressionNode_t * ParseTerminator( int & idx );

/*
====================================================
ParseExpression0
====================================================
*/
ExpressionNode_t * ParseExpression0( int & idx ) {
	// First, check for an assignment expression
	Token_t tok0 = g_tokens[ idx ];

	// Check on Identifiers.  Identifiers can be variables or functions.
	if ( tok0.type == TT_IDENTIFIER ) {
		// Is this a variable?
		//int varIdx = FindLocalVariable( tok0.str, localVarStrt, localVarEnd );
		Symbol_t * sym = FindSymbol( tok0.str );
		if ( NULL == sym ) {
			ReportError( "Unidentified symbol", tok0.coord.col, tok0.coord.ln );
		}

		if ( sym->type == ST_VARIABLE ) {
			Token_t tok1 = g_tokens[ idx + 1 ];

			if ( tok1.type == TT_ASSIGNEMNT ) {
				ExpressionNode_t * expression = new ExpressionNode_t;
				expression->operation = OT_ASSIGNMENT;

				expression->expressionLeft = new ExpressionNode_t;
				expression->expressionLeft->symbol = sym;
				
 				idx += 2;
 				expression->expressionRight = ParseExpression0( idx );
 				return expression;
			}
		}
	}

	// T'was not an assignment, parse on
	return ParseExpression1( idx );
}

/*
====================================================
IsRelationalOperator
====================================================
*/
bool IsRelationalOperator2( const Token_t & tok ) {
	if ( tok.type == TT_BRA ) {	// less than
		return true;
	}
	if ( tok.type == TT_KET ) {	// greater than
		return true;
	}
	if ( tok.type == TT_LESSEQUAL ) {
		return true;
	}
	if ( tok.type == TT_GREATEQUAL ) {
		return true;
	}
	if ( tok.type == TT_COMPARISON ) {
		return true;
	}
	if ( tok.type == TT_NOTEQUAL ) {
		return true;
	}

	return false;
}

/*
====================================================
ParseExpression1
This will check for relational operator, <=, >=, <, >, ==, !=
====================================================
*/
ExpressionNode_t * ParseExpression1( int & idx ) {
	ExpressionNode_t * expression = ParseExpression2( idx );

	Token_t tok = g_tokens[ idx ];
	if ( IsRelationalOperator2( tok ) ) {
		idx++;

		ExpressionNode_t * tmp = expression;
		expression = new ExpressionNode_t;
		expression->expressionLeft = tmp;
		
		switch ( tok.type ) {
			case TT_BRA: expression->operation = OT_LESSTHAN; break;
			case TT_KET: expression->operation = OT_GREATERTHAN; break;
			case TT_LESSEQUAL: expression->operation = OT_LESSEQUAL; break;
			case TT_GREATEQUAL: expression->operation = OT_GREATEREQUAL; break;
			case TT_COMPARISON: expression->operation = OT_ISEQUAL; break;
			case TT_NOTEQUAL: expression->operation = OT_NOTEQUAL; break;
			default: ReportError( "Bad type", tok.coord.col, tok.coord.ln ); break;
		}

		expression->expressionRight = ParseExpression2( idx );
	}

	return expression;
}

/*
====================================================
ParseExpression2
Check addition/subtraction
====================================================
*/
ExpressionNode_t * ParseExpression2( int & idx ) {
	// First check on expressions that have a higher priority order
	ExpressionNode_t * expression = ParseExpression3( idx );

	Token_t tok = g_tokens[ idx ];
	while ( tok.type == TT_PLUS || tok.type == TT_MINUS ) {
		idx++;

		ExpressionNode_t * tmp = expression;
		expression = new ExpressionNode_t;
		expression->expressionLeft = tmp;

		switch ( tok.type ) {
			case TT_PLUS: expression->operation = OT_ADDITION; break;
			case TT_MINUS: expression->operation = OT_SUBTRACTION; break;
			default: ReportError( "Bad type", tok.coord.col, tok.coord.ln ); break;
		}

		expression->expressionRight = ParseExpression3( idx );

		tok = g_tokens[ idx ];
	}

	return expression;
}

/*
====================================================
ParseExpression3
multiplication/division/modulus
====================================================
*/
ExpressionNode_t * ParseExpression3( int & idx ) {
	ExpressionNode_t * expression = ParseExpression4( idx );

	Token_t tok = g_tokens[ idx ];
	while ( tok.type == TT_STAR || tok.type == TT_DIVISION || tok.type == TT_MODULUS ) {
		idx++;

		ExpressionNode_t * tmp = expression;
		expression = new ExpressionNode_t;
		expression->expressionLeft = tmp;
		
		switch ( tok.type ) {
			case TT_STAR: expression->operation = OT_MULTIPLICATION; break;
			case TT_DIVISION: expression->operation = OT_DIVISION; break;
			case TT_MODULUS: expression->operation = OT_MODULUS; break;
			default: ReportError( "Bad type", tok.coord.col, tok.coord.ln ); break;
		}

		expression->expressionRight = ParseExpression4( idx );

		tok = g_tokens[ idx ];
	}

	return expression;
}


/*
====================================================
ParseExpression4
unary +/- or ++/--
====================================================
*/
ExpressionNode_t * ParseExpression4( int & idx ) {
// 	Token_t tok0 = g_tokens[ idx ];
// 	if ( tok0.type == TT_PLUS || tok0.type == TT_MINUS || tok0.type == TT_INCREMENT || tok0.type == TT_DECREMENT ) {
// 		idx++;
// 		Token_t tok1 = g_tokens[ idx ];
// 
// 		if ( tok0.type == TT_INCREMENT ) {
// 			int varIdx = FindLocalVariable( tok1.str, localVarStrt, localVarEnd );
// 			//Variable_t * var = FindVariable( tok1.str, localStack );
// 			var->value++;
// 		}
// 		if ( tok0.type == TT_DECREMENT ) {
// 			int varIdx = FindLocalVariable( tok1.str, localVarStrt, localVarEnd );
// 			//Variable_t * var = FindVariable( tok1.str, localStack );
// 			var->value--;
// 		}
// 	}

	return ParseExpression5( idx );

// 	if ( tok0.type == TT_MINUS ) {
// 		value = -value;
// 	}
}

/*
====================================================
ParseExpression5
parenthesis
====================================================
*/
ExpressionNode_t * ParseExpression5( int & idx ) {
	ExpressionNode_t * expression = NULL;

	Token_t tok = g_tokens[ idx ];
	if ( tok.type == TT_PARENTHOPEN ) {
		idx++;
		expression = ParseExpression0( idx );

		//idx++;
		tok = g_tokens[ idx ];
		if ( tok.type != TT_PARENTHCLOSE ) {
			ReportError( "Expected )", tok.coord.col, tok.coord.ln );
		}

		idx++;
	} else {
		expression = ParseTerminator( idx );
	}

	return expression;
}

/*
====================================================
ParseTerminator
This is a terminating thing... either a literal value, variable, or function call
====================================================
*/
ExpressionNode_t * ParseTerminator( int & idx ) {
	ExpressionNode_t * expression = new ExpressionNode_t;

	Token_t tok = g_tokens[ idx ];
	switch ( tok.type ) {
		case TT_IDENTIFIER: {
			Symbol_t * sym = FindSymbol( tok.str );
			if ( NULL == sym ) {
				ReportError( "Unknown identifier", tok.coord.col, tok.coord.ln );
			}

			if ( ST_VARIABLE == sym->type ) {
				expression->symbol = sym;
			}
			
			// Check for function
// 			Token_t tok1 = g_tokens[ idx + 1 ];
// 			if ( tok1.type == TT_PARENTHOPEN ) {
// 				// Must be a function
// 				int funcid = FindFunction( tok.str );
// 				if ( funcid >= 0 ) {
// 					value = CallFunction( tok.str );
// 					// skip the parenth open/closed
// 					idx += 2;
// 					break;
// 				}
// 			}

//			ReportError( "Unknown identifier", tok.coord.col, tok.coord.ln );
		} break;
		case TT_INTEGER: {
			Symbol_t * sym = FindConstant( tok.valueInt );
			expression->symbol = sym;
		} break;
		//case TT_FLOAT: value = (int)tok.valueFloat; break;	// TODO: add support
		default: ReportError( "Unsupported token", tok.coord.col, tok.coord.ln ); break;
	}
	idx++;

	return expression;
}












void ParseFunctionParameterList( int & idx, FunctionDeclaration_t * nodePrev ) {
	// TODO: Parse the parameter list of the function declaration
}




/*
====================================================
ParseFunction
Add a function to the AST
====================================================
*/
void ParseFunction( int & idx, GenericNode_t * nodePrev ) {
	// First token should be the return type
	Token_t tok = g_tokens[ idx ];
	if ( tok.type != TT_KEYWORD_INT ) {
		ReportError( "Error parsing function", tok.coord.col, tok.coord.ln );
	}

	// Next token should be the name of the function
	idx++;
	tok = g_tokens[ idx ];
	if ( tok.type != TT_IDENTIFIER ) {
		ReportError( "Error parsing function", tok.coord.col, tok.coord.ln );
	}

	FunctionDeclaration_t * function = new FunctionDeclaration_t;
	//memset( function, 0, sizeof( FunctionDeclaration_t ) );
	function->name = tok.str;

	// Skip the parenthesis, in the future, we will parse the parameter list
	idx++;
	tok = g_tokens[ idx ];
	if ( tok.type != TT_PARENTHOPEN ) {
		ReportError( "Expected open parenthesis", tok.coord.col, tok.coord.ln );
	}

	ParseFunctionParameterList( idx, function );

	idx++;
	tok = g_tokens[ idx ];
	if ( tok.type != TT_PARENTHCLOSE ) {
		ReportError( "Expected closing parenthesis", tok.coord.col, tok.coord.ln );
	}
	idx++;
	tok = g_tokens[ idx ];
	if ( tok.type != TT_CURLYBRACKETOPEN ) {
		ReportError( "Expected open curly bracket", tok.coord.col, tok.coord.ln );
	}

	int scope = 1;
//	function->localVarStart = g_symbols.locals.size();
	nodePrev->function = function;

	// Parse local variables
	idx++;
	tok = g_tokens[ idx ];
	while ( IsType( tok ) ) {
		idx++;
		tok = g_tokens[ idx ];
		if ( tok.type != TT_IDENTIFIER ) {
			ReportError( "Expected variable identifier", tok.coord.col, tok.coord.ln );
		}

		//AddLocalVariable( tok, function->localVarStart, g_symbols.locals.size() );
		AddVariableToSymbolTable( tok.str );

		idx++;
		tok = g_tokens[ idx ];
		if ( tok.type != TT_SEMICOLON ) {
			ReportError( "Expected semicolon", tok.coord.col, tok.coord.ln );
		}

		idx++;
		tok = g_tokens[ idx ];
	}

// 	const int localVariableEnd = g_symbols.locals.size();
// 	function->numLocals = localVariableEnd - function->localVarStart;

	GenericNode_t * currentNode = new GenericNode_t;
	memset( currentNode, 0, sizeof( GenericNode_t ) );
	function->next = currentNode;

	while ( scope > 0 && idx < g_tokens.size() ) {
		switch ( tok.type ) {
			case TT_CURLYBRACKETOPEN: { scope++; } break;
			case TT_CURLYBRACKETCLOSE: { scope--; } break;
			case TT_IDENTIFIER: {
				currentNode->expression = ParseExpression0( idx );
				currentNode->next = new GenericNode_t;
				currentNode = currentNode->next;
				
				tok = g_tokens[ idx ];
				if ( tok.type != TT_SEMICOLON ) {
					ReportError( "expected semicolon", tok.coord.col, tok.coord.ln );
				}
				break;
			} break;
			default: ReportError( "Parsing Error", tok.coord.col, tok.coord.ln ); break;
		}

		idx++;
		if ( idx < g_tokens.size() ) {
			tok = g_tokens[ idx ];
		}		
	}
}

bool IsFunctionDeclaration( int idx ) {
	// TODO: Go until we hit an opening parenthesis... if never hit, then not a function
	return false;
}

bool IsVariableDeclaration( int idx ) {
	// TODO: Go until we hit a semicolon... if never hit, then not a variable
	return true;
}

bool IsEnumDeclaration( int idx ) {
	Token_t tok = g_tokens[ idx ];
	return ( tok.type == TT_KEYWORD_ENUM );
}

bool IsStructDeclaration( int idx ) {
	Token_t tok = g_tokens[ idx ];
	return ( tok.type == TT_KEYWORD_STRUCT );
}


/*
====================================================
Parser
Go token by token, creating the AST
====================================================
*/
void Parser() {
	int idx = 0;

	g_ast = new GenericNode_t;

	while ( idx < g_tokens.size() ) {
		Token_t tok = g_tokens[ idx ];

		if ( tok.type == TT_KEYWORD_INT ) {
			Token_t tok1 = g_tokens[ idx + 1 ];
			if ( tok1.type != TT_IDENTIFIER ) {
				ReportError( "Identifier expected after type", tok1.coord.col, tok1.coord.ln );
			}

			Token_t tok2 = g_tokens[ idx + 2 ];
			if ( tok2.type == TT_SEMICOLON ) {
				// This is a global variable, add it to the global world
				//AddGlobalVariable( tok1 );
				AddVariableToSymbolTable( tok1.str );
			} else if ( tok2.type == TT_PARENTHOPEN ) {
				// This is a function, parse the function
				ParseFunction( idx, g_ast );
			} else {
				ReportError( "Unexpected token after declaration", tok2.coord.col, tok2.coord.ln );
			}
		}
	}

	GenerateLLVM();
}














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
		if ( ST_CONSTANT == expresson_node->symbol->type ) {
			char tmp[ 32 ] = { 0 };
			sprintf( tmp, "%i", expresson_node->symbol->constantValue );
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
	llvm_code += "define i32 @";
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
	llvm_code += "ret i32 0\n";
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