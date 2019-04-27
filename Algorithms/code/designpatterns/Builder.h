//
//  Builder.h
//
#pragma once
#include <stdio.h>

/*
===============================
Product
===============================
*/
class Product {
private:
	Product( const int data ) : m_data( data ) {}	// Private constructor so only the builder can construct a product

public:
	friend class Builder;

private:
	const int m_data;
};

/*
===============================
Builder
===============================
*/
class Builder {
public:
	Builder() : m_productInitializer( 0 ) {}

	void SetInitializer ( const int i ) { m_productInitializer = i; }

	Product Build() const { return Product( m_productInitializer ); }

private:
	int m_productInitializer;
};