//
//	bubblesort.cpp
//
#include <stdio.h>
#include "bubblesort.h"

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
bubblesort
====================================================
*/
void bubblesort( int * data, int num ) {
	int n = num;
	do {
		int next = 0;
		for ( int i = 1; i < num; i++ ) {
			if ( data[ i - 1 ] > data[ i ] ) {
				swap( data[ i - 1], data[ i ] );
				next = i;
			}
		}
		n = next;
	} while ( n > 0 );
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
TestBubbleSort
====================================================
*/
int TestBubbleSort( int argc, char * argv[] ) {
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

	printf( "Bubble Sort---------------------------------------------------------\n" );
	printf( "data = " );
	PrintData( data, 10 );

	bubblesort( data, 10 );

	printf( "sorted = " );
	PrintData( data, 10 );
	printf( "---------------------------------------------------------\n" );

	return 0;
}