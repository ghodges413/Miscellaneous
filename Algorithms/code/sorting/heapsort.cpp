//
//	heapsort.cpp
//
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "heapsort.h"

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
siftdown
====================================================
*/
void siftdown( int * heap, const int num, int i ) {
	while ( 2 * i <= num ) {
		int j = 2 * i;

		if ( j < num && heap[ j ] > heap[ j - 1 ] ) {
			j++;
		}

		if ( heap[ i - 1 ] > heap[ j - 1 ] ) {
			break;
		}

		swap( heap[ i - 1 ], heap[ j - 1 ] );
		i = j;
	}
}

/*
====================================================
heapify
====================================================
*/
void heapify( int * heap, const int num ) {
	for ( int i = num / 2; i >= 1; i-- ) {
		siftdown( heap, num, i );
	}
}

/*
====================================================
heapsort
====================================================
*/
void heapsort( int * data, const int num ) {
	// create the heap
	heapify( data, num );

	// sort
	for ( int n = num; n > 1; n-- ) {
		// swap the beginning and the end
		swap( data[ 0 ], data[ n - 1 ] );
		siftdown( data, n - 1, 1 );
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
TestHeapSort
====================================================
*/
int TestHeapSort( int argc, char * argv[] ) {
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

	printf( "Heap Sort---------------------------------------------------------\n" );
	printf( "data = " );
	PrintData( data, 16 );

	heapsort( data, 16 );

	printf( "sorted = " );
	PrintData( data, 16 );
	printf( "---------------------------------------------------------\n" );

	return 0;
}