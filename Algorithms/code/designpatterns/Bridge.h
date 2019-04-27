//
//  Bridge.h
//
#pragma once
#include <stdio.h>

/*
===============================
BridgeInterface
===============================
*/
class BridgeInterface {
public:
	virtual void Function() = 0;
};

/*
===============================
Bridge1
===============================
*/
class Bridge1 : public BridgeInterface {
public:
	void Function() override { printf( "Bridge1\n" ); }
};

/*
===============================
Bridge2
===============================
*/
class Bridge2 : public BridgeInterface {
public:
	void Function() override { printf( "Bridge2\n" ); }
};

/*
===============================
AbstractBridgeInterface
===============================
*/
class AbstractBridgeInterface {
public:
	virtual void CallMethod() = 0;
};

/*
===============================
AbstractBridge
===============================
*/
class AbstractBridge : public AbstractBridgeInterface {
public:
	AbstractBridge( BridgeInterface * bridge ) : m_bridge( bridge ) {}

	void CallMethod() override { m_bridge->Function(); }

private:
	BridgeInterface * m_bridge;
};