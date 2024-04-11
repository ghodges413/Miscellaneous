/*
========================================================================================================

AST.h

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
struct Symbol_t;

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

	Type_t returnType;

	// TODO: add parameter list and return type
	// parameters should probably be pointers to symbols
	// the symbols of parameters should probably be considered local variables of the function

	struct GenericNode_t * next;
};

enum OperationType_t {
	OT_NULL,		// When an expression node is only a single variable or constant
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

extern GenericNode_t * g_ast;