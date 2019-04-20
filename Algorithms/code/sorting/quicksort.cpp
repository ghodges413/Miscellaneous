//
//	quicksort.cpp
//
#include <stdio.h>
#include "quicksort.h"

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
quicksort_r
====================================================
*/
void quicksort_r( int * data, int lo, int hi ) {
	if ( lo >= hi ) {
		return;
	}

	int i = lo;
	int j = hi - 1;
	int pivot = hi;
	while ( i < j ) {
		if ( data[ j ] < data[ i ] ) {
			swap( data[ j ], data[ i ] );
		}
		if ( data[ pivot ] < data[ j ] ) {
			swap( data[ pivot ], data[ j ] );
			pivot = j;
			j--;
		}

		if ( data[ i ] < data[ pivot ] ) {
			i++;
		}
	}

	if ( data[ pivot ] < data[ j ] ) {
		swap( data[ pivot ], data[ j ] );
		pivot = j;
	}

	quicksort_r( data, lo, pivot - 1 );
	quicksort_r( data, pivot + 1, hi );
}

/*
====================================================
quicksort
====================================================
*/
void quicksort( int * data, int num ) {
	quicksort_r( data, 0, num - 1 );
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
CompareIntegers
====================================================
*/
int TestQuickSort( int argc, char * argv[] ) {
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

	printf( "Quick Sort---------------------------------------------------------\n" );
	printf( "data = " );
	PrintData( data, 10 );

	quicksort( data, 10 );

	printf( "sorted = " );
	PrintData( data, 10 );
	printf( "---------------------------------------------------------\n" );

	return 0;
}