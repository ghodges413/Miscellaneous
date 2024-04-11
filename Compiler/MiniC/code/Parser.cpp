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

AST Parsing
Convert the list of tokens into an Abstract Syntax Tree

========================================================================================================
*/


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

			if ( tok1.type == TT_ASSIGNMENT ) {
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

	// It was not an assignment, parse on
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
			Symbol_t * sym = FindConstantInt( tok.valueInt );
			expression->symbol = sym;
		} break;
		case TT_FLOAT: {
			Symbol_t * sym = FindConstantFloat( tok.valueFloat );
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
	if ( !IsType( idx ) ) {
		Token_t tok = g_tokens[ idx ];
		ReportError( "Error parsing function, expected type before function name", tok.coord.col, tok.coord.ln );
	}
// 	Token_t tok = g_tokens[ idx ];
// 	if ( tok.type != TT_KEYWORD_INT ) {
// 		ReportError( "Error parsing function", tok.coord.col, tok.coord.ln );
// 	}

	// Next token should be the name of the function
	idx++;
	Token_t tok = g_tokens[ idx ];
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
			default: {
				ReportError( "Parsing Error", tok.coord.col, tok.coord.ln );
			} break;
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

		if ( IsType( idx ) ) {
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
}





