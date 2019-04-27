//
//  Flyweight.h
//
#pragma once
#include <string>

/*
 ===============================
 Beer
 ===============================
 */
class Beer {
public:
	Beer( const char * str ) {
		m_name = str;
	}

	const char * GetFlavor() const { return m_name.c_str(); }

	bool operator == ( const Beer & rhs ) const {
		return ( 0 == m_name.compare( rhs.m_name ) );
	}

	bool operator != ( const Beer & rhs ) const {
		return !( *this == rhs );
	}

private:
	std::string m_name;
};