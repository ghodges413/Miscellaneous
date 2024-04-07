//
//  Atoi.cpp
//
#include "Atoi.h"

#include <iostream>



int MyATOI( const char * str ) {
	if ( NULL == str ) {
		return 0;
	}

	int idx = 0;
	int accumulator = 0;

	// Remove leading white space
	while ( ' ' == str[ idx ] ) {
		idx++;
	}

	// Check for plus/minus
	int mul = 1;
	if ( '-' == str[ idx ] || '+' == str[ idx ] ) {
		if ( '-' == str[ idx ] ) {
			mul = -1;
		}
		idx++;
	}

	// Get the integer
	while ( 0 != str[ idx ] ) {
		int val = str[ idx ] - '0';
		if ( val < 0 || val > 9 ) {
			break;
		}

		accumulator = accumulator * 10 + val;
		idx++;
	}

	return accumulator * mul;
}

char * MyITOA( int value, char * buffer, int base ) {
	if ( base < 2 || base > 32 ) {
		return NULL;
	}

	if ( NULL == buffer ) {
		return NULL;
	}

	int offset = -1;
	int idx = 0;
	if ( value < 0 ) {
		value = abs( value );
		buffer[ idx ] = '-';
		idx++;
		offset = 0;
	}

	int length = 0;
	int tmp = value;
	while ( tmp > 0 ) {
		tmp = tmp / base;
		length++;
	}
	length += idx;
	

	while ( value > 0 ) {
		char c = '0' + ( value % base );
		if ( c > '9' ) {
			c = 'A' + ( value % base ) - 10;
		}

		buffer[ length - idx + offset ] = c;
		value = value / base;
		idx++;
	}
	buffer[ idx ] = 0;

	return buffer;
}



/*
===============================
TestATOI
===============================
*/
int TestATOI( int argc, char * argv[] ) {
	char buffer[ 64 ] = { 0 };
	MyITOA( 255, buffer, 16 );
	printf( buffer );
	printf( "\n" );

	const char * str = " -109234abc";
	int val = atoi( str );
	int val2 = MyATOI( str );
	printf( "atoi: %i   %i\n", val, val2 );

    return 0;
}
