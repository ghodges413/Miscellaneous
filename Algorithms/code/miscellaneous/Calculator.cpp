//
//  Calculator.cpp
//
#include "Calculator.h"

#include <iostream>


// Two's compliment
// It works nicely because addition and subtraction work the same way for both positive and negative values
// The most significant bit is set to -2^31, the next is 2^30, 2^29... etc
// Here's a 4-bit example:
// 1 1 1 1 = -1   ( -8 + 4 + 2 + 1 = -1 )
// 1 0 0 0 = -8
// 1 0 0 1 = -7
// 0 0 0 1 = 1
// 0 1 1 1 = 7

bool IsNotEqual( int a, int b ) {
	return ( a ^ b );
}

bool IsEqual( int a, int b ) {
	return !IsNotEqual( a, b );
}

bool IsLessThanZero( int a ) {
	int neg = 0x80000000;
	return ( a & neg );
}

bool IsGreaterThanZero( int a ) {
	return ( !IsLessThanZero( a ) && a );
}

int Add( int a, int b ) {
	// Iterate till there is no carry
	while ( b != 0 ) {
		int carry = a & b;	// calculate the carry values
 		a = a ^ b;			// a = a xor b, performs the sum
 		b = carry << 1;		// store the carry values
	}
	return a;
}

int Sub( int a, int b ) {
    while ( b != 0 ) {
        int carry = (~a) & b;
        a = a ^ b;
        b = carry << 1;
    }
    return a;
}

int Negate( int val ) {
	val = ~val;
	val = Add( val, 1 );
	return val;
}

int Sub2( int a, int b ) {
	b = Negate( b );
	return Add( a, b );
}

// This multiply is just a big sum
int Mul2( int n, int m ) {
	bool negate = false;

	if ( m < 0 ) {
		m = Negate( m );
		negate = true;
	}

	int sum = 0;
	while ( m > 0 ) {
		sum = Add( sum, n );
		m = Sub( m, 1 );
	}

	if ( negate ) {
		sum = Negate( sum );
	}

	return sum;
}

// This multiply is kinda how it's done in "long" form (like how it's taught in grade school)
int Mul( int n, int m ) {
	bool negate = false;

	// Check for negative values
	if ( m < 0 ) {
		m = Negate( m );
		negate = true;
	}

	int count = 0;
	int result = 0;
	
	while ( m ) {
		if ( m & 1 ) {
			result = Add( result, n << count );
		}

		count++;
		m = m >> 1;
	}

	if ( negate ) {
		result = Negate( result );
	}
	return result;
}

// This division is pretty simple, just count the number of times we can subtract the denominator from the numerator
int Div2( int n, int m, int * remainder ) {
	if ( 0 == m ) {
		return n;
	}

	bool negN = false;
	bool negM = false;

	if ( n < 0 ) {
		n = Negate( n );
		negN = true;
	}
	if ( m < 0 ) {
		m = Negate( m );
		negM = true;
	}

	int q = 0;
	while ( n >= m ) {
		n = Sub( n, m );
		q = Add( q, 1 );
	}

	if ( negN ) {
		q = Negate( q );
	}
	if ( negM ) {
		q = Negate( q );
	}

	*remainder = n;
	return q;
}

// This recursive division is the way long division is taught
int Div_r( const int numerator, const int divisor, int * remainder ) {
	// Check for equality
	if ( numerator == divisor ) {
		*remainder = 0;
		return 1;
	}
	
	// Check for no more division
	if ( numerator < divisor ) {
		*remainder = numerator;
		return 0;
	}

	int q = 1;
	int d = divisor;	

	// Close the gap
	while ( d <= numerator ) {
		d = d << 1;
		q = q << 1;
	}

	// Check for overshoot
	if ( numerator < d ) {
		d >>= 1;
		q >>= 1;
	}

	q = q + Div_r( numerator - d, divisor, remainder );
	return q;
}

int Div( int numerator, int divisor, int * remainder ) {
	if ( 0 == divisor ) {
		*remainder = numerator;
		return 0;
	}

	bool negateN = false;
	bool negateD = false;

	// Check for negative values
	if ( numerator < 0 ) {
		negateN = true;
		numerator = Negate( numerator );
	}
	if ( divisor < 0 ) {
		negateD = true;
		divisor = Negate( divisor );
	}

	int q = Div_r( numerator, divisor, remainder );

	if ( negateN ) {
		q = Negate( q );
		*remainder = Negate( *remainder );
	}
	if ( negateD ) {
		q = Negate( q );
		*remainder = Negate( *remainder );
	}
	return q;
}



bool IsDigit( char c ) {
	if ( c < '0' ) {
		return false;
	}
	if ( c > '9' ) {
		return false;
	}
	return true;
}

enum operator_t {
	OP_NULL,
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
};

int PerformOperation( int a, int b, operator_t op ) {
	int result = b;
	switch ( op ) {
		default:
		case OP_NULL: break;
		case OP_ADD: result = a + b; break;
		case OP_SUB: result = a - b; break;
		case OP_MUL: result = a * b; break;
		case OP_DIV: result = a / b; break;
	}
	return result;
}

int Calculate( const char * str ) {
	if ( NULL == str ) {
		return 0;
	}

	const int len = strlen( str );

	int result = 0;
	int num = 0;
	operator_t op = OP_NULL;

	for ( int i = 0; i < len; i++ ) {
		const char c = str[ i ];
		if ( IsDigit( str[ i ] ) ) {
			num = num * 10 + ( c - '0' );
			continue;
		}

		// TOOD: Check for invalid character

		result = PerformOperation( result, num, op );
		num = 0;

		op = OP_NULL;
		if ( '+' == c ) {
			op = OP_ADD;
		}
		if ( '-' == c ) {
			op = OP_SUB;
		}
		if ( '*' == c ) {
			op = OP_MUL;
		}
		if ( '/' == c ) {
			op = OP_DIV;
		}
	}

	result = PerformOperation( result, num, op );
	return result;
}

/*
===============================
TestCalculator
===============================
*/
int TestCalculator( int argc, char * argv[] ) {
	int n2 = 128;
	int d = -3;
	int r = 0;
	int q = Div( n2, d, &r );

	for ( int i = -32; i < 32; i++ ) {
		int ni = Negate( i );
		printf( "Negate: %i   %i\n", i, ni );
	}

	for ( int i = -8; i < 8; i++ ) {
		printf( " %i ", i );
		if ( IsLessThanZero( i ) ) {
			printf( "Less Than Zero\n" );
		} else {
			printf( "Not less\n" );
		}
	}

	for ( int y = -10; y < 10; y++ ) {
		for ( int x = -10; x < 10; x++ ) {
			int a = Add( x, y );
			int s = Sub2( x, y );
			int m = Mul( x, y );
			int d = Div2( x, y, &r );
			printf( "   %i  %i   add: %i   sub: %i   mul: %i   div: %i   %i\n", x, y, a, s, m, d, r );
		}
	}

	const char * str = "128+5*2-1/11";
	int result = Calculate( str );
	printf( "%s = %i\n", str, result );

    return 0;
}