//
//	mergesort.cpp
//
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "mergesort.h"

/*
====================================================
binning
====================================================
*/
void binning( int * groupA, int * groupB, int numA, int numB ) {
	// Create a sorted binning buffer
	const int numElements = numA + numB;
	int * bin = (int *)alloca( numElements * sizeof( int ) );

	// Sort into the bin
	int idxA = 0;
	int idxB = 0;
	for ( int i = 0; i < numElements; i++ ) {
		if ( idxB == numB ) {
			bin[ i ] = groupA[ idxA ];
			idxA++;
			continue;
		}

		if ( idxA == numA ) {
			bin[ i ] = groupB[ idxB ];
			idxB++;
			continue;
		}

		if ( groupA[ idxA ] < groupB[ idxB ]) {
			bin[ i ] = groupA[ idxA ];
			idxA++;
			continue;
		} else if ( idxB < numB ) {
			bin[ i ] = groupB[ idxB ];
			idxB++;
			continue;
		}

		assert( false );
	}

	// Copy data back into the original
	for ( int i = 0; i < numElements; i++ ) {
		groupA[ i ] = bin[ i ];
	}
}

/*
====================================================
mergesort
====================================================
*/
void mergesort( int * data, int num ) {
	int numElements = 1;
	while ( numElements < num ) {
		int numGroups = num / numElements;

		for ( int groupID = 0; groupID < numGroups; groupID += 2 ) {	// Process two adjacent groups at a time
			int * groupA = data + ( groupID + 0 ) * numElements;
			int * groupB = data + ( groupID + 1 ) * numElements;

			binning( groupA, groupB, numElements, numElements );
		}

		numElements *= 2;
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
TestMergeSort
====================================================
*/
int TestMergeSort( int argc, char * argv[] ) {
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

	printf( "Merge Sort---------------------------------------------------------\n" );
	printf( "data = " );
	PrintData( data, 16 );

	mergesort( data, 16 );

	printf( "sorted = " );
	PrintData( data, 16 );
	printf( "---------------------------------------------------------\n" );

	return 0;
}