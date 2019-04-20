//
//	insertionsort.cpp
//
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "insertionsort.h"

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
insertionsort
====================================================
*/
void insertionsort( int * data, int num ) {
	for ( int i = 1; i < num; i++ ) {
		for ( int j = i; j > 0; j-- ) {
			if ( data[ j - 1 ] <= data[ j ] ) {
				break;
			}

			swap( data[ j ], data[ j - 1 ] );
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
TestInsertionSort
====================================================
*/
int TestInsertionSort( int argc, char * argv[] ) {
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

	printf( "Insertion Sort---------------------------------------------------------\n" );
	printf( "data = " );
	PrintData( data, 16 );

	insertionsort( data, 16 );

	printf( "sorted = " );
	PrintData( data, 16 );
	printf( "---------------------------------------------------------\n" );

	return 0;
}