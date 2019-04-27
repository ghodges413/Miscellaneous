//
//  Prototype.h
//
#pragma once

/*
===============================
Prototype
===============================
*/
class Prototype {
public:
	virtual Prototype * Clone() = 0;
};

/*
===============================
JangoFett
===============================
*/
class JangoFett : public Prototype {
public:
	JangoFett( const JangoFett & rhs ) {}

	Prototype * Clone() override {
		return new JangoFett( *this );
	}
};