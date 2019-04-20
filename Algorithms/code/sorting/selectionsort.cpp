//
//	selectionsort.cpp
//
#include <stdio.h>
#include "selectionsort.h"

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
selectionsort
====================================================
*/
void selectionsort( int * data, int num ) {
	for ( int i = 0; i < num; i++ ) {
		int min = i;

		for ( int j = i + 1; j < num; j++ ) {
			if ( data[ j ] < data[ min ] ) {
				min = j;
			}
		}

		swap( data[ i ], data[ min ] );
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
TestSelectionSort
====================================================
*/
int TestSelectionSort( int argc, char * argv[] ) {
	int data[ 10 ];
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

	printf( "Selection Sort---------------------------------------------------------\n" );
	printf( "data = " );
	PrintData( data, 10 );

	selectionsort( data, 10 );

	printf( "sorted = " );
	PrintData( data, 10 );
	printf( "---------------------------------------------------------\n" );

	return 0;
}