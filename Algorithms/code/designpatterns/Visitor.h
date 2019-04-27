//
//  Visitor.h
//
#pragma once
#include <stdio.h>

class AbstractDispatcher;

/*
===============================
AbstractType
===============================
*/
class AbstractType {
public:
	virtual void Accept( AbstractDispatcher * dispatcher ) = 0;
};

/*
===============================
ConcreteTypeA
===============================
*/
class ConcreteTypeA : public AbstractType {
public:
	void Accept( AbstractDispatcher * dispatcher ) override { dispatcher->Dispatch( *this ); }
};

/*
===============================
ConcreteTypeB
===============================
*/
class ConcreteTypeB : public AbstractType {
public:
	void Accept( AbstractDispatcher * dispatcher ) override { dispatcher->Dispatch( *this ); }
};

/*
===============================
AbstractDispatcher
===============================
*/
class AbstractDispatcher {
public:
	virtual void Dispatch( ConcreteTypeA & file ) = 0;
	virtual void Dispatch( ConcreteTypeB & file ) = 0;
};

/*
===============================
Dispatcher
===============================
*/
class Dispatcher : public AbstractDispatcher {
public:
	void Dispatch( ConcreteTypeA & ) override { printf( "Dispatching ConcreteTypeA\n" ); }
	void Dispatch( ConcreteTypeB & ) override { printf( "Dispatching ConcreteTypeB\n" ); }
};