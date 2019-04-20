//
//	shellsort.cpp
//
#include <stdio.h>
#include "shellsort.h"

/*
====================================================
swap
====================================================
*/
static void swap( int & a, int & b ) {
	int tmp = a;
	a = b;
	b = tmp;
}

/*
====================================================
shellsort
====================================================
*/
void shellsort( int * data, int num ) {
	for ( int h = num / 2 + 1; h >= 1; h /= 2 ) {
		// h-sort the array
		for ( int i = h; i < num; i++ ) {
			for ( int j = i; j >= h && data[ j ] < data[ j - h ]; j -= h ) {
				swap( data[ j ], data[ j - h ] );
			}
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
TestShellSort
====================================================
*/
int TestShellSort( int argc, char * argv[] ) {
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
	data[ 15 ] = -1;

	printf( "Shell Sort---------------------------------------------------------\n" );
	printf( "data = " );
	PrintData( data, 13 );

	shellsort( data, 13 );

	printf( "sorted = " );
	PrintData( data, 13 );
	printf( "---------------------------------------------------------\n" );

	return 0;
}