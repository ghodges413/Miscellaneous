//
//	radixsort.cpp
//
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "radixsort.h"

// A youtuber with the channel titled "What's a Creel?"
// Does an excellent explanation of the radix sort in his video series
// "Why is Radix Sort so Fast?"

/*
====================================================
CountingSort
====================================================
*/
void CountingSort( int * data, int num, int maxValue ) {
	int digits = maxValue;

	int * counters = (int *)alloca( sizeof( int ) * digits );
	for ( int d = 0; d < digits; d++ ) {
		counters[ d ] = 0;
	}

	for ( int i = 0; i < num; i++ ) {
		int value = data[ i ];
		//TODO: value = value / digits;	// Do this the number of times for the max number of digit placement (reword confusing verbage)
		value = value % digits;
		counters[ value ]++;
	}

	int i = 0;
	for ( int d = 0; d < digits; d++ ) {
		for ( int k = 0; k < counters[ d ]; k++ ) {
			data[ i ] = d;
			//data[ i ] = counters[ d ];
			i++;
		}
	}
}

/*
====================================================
RadixSortBase10
====================================================
*/
void RadixSortBase10( int * input, int num ) {
	int * output = (int *)alloca( sizeof( int ) * num );
	for ( int i = 0; i < num; i++ ) {
		output[ i ] = 0;
	}

	static const int base = 10;

	int digitsToDo = 3;
	for ( int d = 0; d < digitsToDo; d++ ) {
		// Get the counts table
		int counts[ base ] = { 0 };
		for ( int i = 0; i < num; i++ ) {
			int value = input[ i ];
			for ( int shift = 0; shift < d; shift++ ) {
				value /= base; // shift the digits over by d
			}
			value %= base;

			counts[ value ]++;
		}

		// Build the prefix sum table
		int sum = 0;
		int prefixSum[ base ];
		for ( int i = 0; i < base; i++ ) {
			sum += counts[ i ];
			prefixSum[ i ] = sum;
		}

		for ( int i = num - 1; i >= 0; i-- ) {
			int value = input[ i ];
			for ( int shift = 0; shift < d; shift++ ) {
				value /= base; // shift the digits over by d
			}
			value %= base;

			int slot = prefixSum[ value ] - 1;
			prefixSum[ value ]--;
			output[ slot ] = input[ i ];
		}

		// Copy output back to input
		for ( int i = 0; i < num; i++ ) {
			input[ i ] = output[ i ];
		}
	}
}

/*
====================================================
RadixSortBase256
====================================================
*/
void RadixSortBase256( int * input, int num ) {
	int * output = (int *)alloca( sizeof( int ) * num );
	for ( int i = 0; i < num; i++ ) {
		output[ i ] = 0;
	}

	static const int base = 256;

	int digitsToDo = 4;
	for ( int d = 0; d < digitsToDo; d++ ) {
		// Get the counts table
		int counts[ base ] = { 0 };
		for ( int i = 0; i < num; i++ ) {
			int value = input[ i ];
			for ( int shift = 0; shift < d; shift++ ) {
				//value /= base; // shift the digits over by d
				value >>= 8;
			}
			//value %= base;
			value = value & 0xff;

			counts[ value ]++;
		}

		// Build the prefix sum table
		int sum = 0;
		int prefixSum[ base ];
		for ( int i = 0; i < base; i++ ) {
			sum += counts[ i ];
			prefixSum[ i ] = sum;
		}

		for ( int i = num - 1; i >= 0; i-- ) {
			int value = input[ i ];
			for ( int shift = 0; shift < d; shift++ ) {
				//value /= base; // shift the digits over by d
				value >>= 8;
			}
			//value %= base;
			value = value & 0xff;

			int slot = prefixSum[ value ] - 1;
			prefixSum[ value ]--;
			output[ slot ] = input[ i ];
		}

		// Copy output back to input
		for ( int i = 0; i < num; i++ ) {
			input[ i ] = output[ i ];
		}
	}
}



/*
====================================================
PrintData
====================================================
*/
static void PrintData( int * data, int num ) {
	for ( int i = 0; i < num; i++ ) {
		printf( " %i", data[ i ] );
	}
	printf( "\n" );
}

/*
====================================================
TestRadixSort
====================================================
*/
int TestRadixSort( int argc, char * argv[] ) {
	int data[ 16 ];
	data[ 0 ] = 3;
	data[ 1 ] = 9;
	data[ 2 ] = 1;
	data[ 3 ] = 7;
	data[ 4 ] = 4;
	data[ 5 ] = 8;
	data[ 6 ] = 10;
	data[ 7 ] = 2;
	data[ 8 ] = 8;
	data[ 9 ] = 0;
	data[ 10 ] = 3;
	data[ 11 ] = 6;
	data[ 12 ] = 13;
	data[ 13 ] = 2;
	data[ 14 ] = 15;
	data[ 15 ] = 16;//-1;

	printf( "Radix Sort---------------------------------------------------------\n" );
	printf( "data = " );
	PrintData( data, 16 );

//	CountingSort( data, 13, 20 );

#if 1
	data[ 0 ] = 277;
	data[ 1 ] = 806;
	data[ 2 ] = 681;
	data[ 3 ] = 462;
	data[ 4 ] = 787;
	data[ 5 ] = 163983648;
	data[ 6 ] = 284;
	data[ 7 ] = 166;
	data[ 8 ] = 905;
	data[ 9 ] = 518;
	data[ 10 ] = 263;
	data[ 11 ] = 395;
	data[ 12 ] = 988;
	data[ 13 ] = 307;
	data[ 14 ] = 779;
	data[ 15 ] = 721;
#endif
	//RadixSortBase10( data, 16 );
	RadixSortBase256( data, 16 );

	printf( "sorted = " );
	PrintData( data, 16 );
	printf( "---------------------------------------------------------\n" );

	return 0;
}