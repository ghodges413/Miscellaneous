//
//  Endianess.cpp
//
#include "Endianess.h"

#include <iostream>



/*
int x = 0x76543210;
char *c = (char*) &x;

Big endian format: (aka network order)
------------------
Byte address  | 0x01 | 0x02 | 0x03 | 0x04 | 
              +++++++++++++++++++++++++++++
Byte content  | 0x76 | 0x54 | 0x32 | 0x10 |
			 
Little endian format: (aka host order)
---------------------
Byte address  | 0x01 | 0x02 | 0x03 | 0x04 | 
              +++++++++++++++++++++++++++++
Byte content  | 0x10 | 0x32 | 0x54 | 0x76 |
*/

// 0 = big endian
// 1 = little endian
int CheckEndianness() {
	unsigned int x = 1;
	char * c = (char *)&x;
	return (int)*c;
}

bool IsLittleEndian() {
	return ( 1 == CheckEndianness() );
}

bool IsBigEndian() {
	return ( 0 == CheckEndianness() );
}

struct testHost_t {
	unsigned long long a;
	unsigned long long b;
};


/*
===============================
TestEndianess
===============================
*/
int TestEndianess( int argc, char * argv[] ) {
	if ( IsBigEndian() ) {
		printf( "Big Endian detected\n" );
	}
	if ( IsLittleEndian() ) {
		printf( "Little Endian detected\n" );
	}

    return 0;
}


