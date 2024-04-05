//
//  main.cpp
//
#include "threading/JobSystem/JobSystem.h"

#include "sorting/bubblesort.h"
#include "sorting/heapsort.h"
#include "sorting/insertionsort.h"
#include "sorting/mergesort.h"
#include "sorting/quicksort.h"
#include "sorting/selectionsort.h"
#include "sorting/shellsort.h"
#include "sorting/radixsort.h"

#include "allocators/SimpleTracker.h"
#include "allocators/MemoryPool.h"
#include "allocators/StackAllocator.h"
#include "allocators/BuddyAllocator.h"

#include "networking/Sockets.h"

#include "interviews/ProducerConsumer.h"

#include <string>

typedef int TestFunction_t( int argc, char * argv[] );

struct functions_t {
	const char * name;
	TestFunction_t * functor;
};

functions_t g_functions[] = {
	{ "TestSockets", TestSockets },

	{ "TestJobSystem", TestJobSystem },

	{ "TestBubbleSort", TestBubbleSort },
	{ "TestHeapSort", TestHeapSort },
	{ "TestInsertionSort", TestInsertionSort },
	{ "TestMergeSort", TestMergeSort },
	{ "TestQuickSort", TestQuickSort },
	{ "TestSelectionSort", TestSelectionSort },
	{ "TestShellSort", TestShellSort },
	{ "TestRadixSort", TestRadixSort },

	{ "TestSimpleTracker", TestSimpleTracker },
	{ "TestMemoryPool", TestMemoryPool },
	{ "TestStackAllocator", TestStackAllocator },
	{ "TestBuddyAllocator", TestBuddyAllocator },

	{ "TestProducerConsumer", TestProducerConsumer },
};

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
	if ( argc > 1 ) {
		int numFunctions = sizeof( g_functions ) / sizeof( functions_t );

		for ( int i = 0; i < numFunctions; i++ ) {
			if ( 0 == strcmp( argv[ 1 ], g_functions[ i ].name ) ) {
				g_functions[ i ].functor( argc, argv );
				break;
			}
		}
	} else {
		TestProducerConsumer( argc, argv );

		TestSockets( argc, argv );

		TestJobSystem( argc, argv );

		TestBubbleSort( argc, argv );
		TestHeapSort( argc, argv );
		TestInsertionSort( argc, argv );
		TestMergeSort( argc, argv );
		TestQuickSort( argc, argv );
		TestSelectionSort( argc, argv );
		TestShellSort( argc, argv );
		TestRadixSort( argc, argv );

		TestSimpleTracker( argc, argv );
		TestMemoryPool( argc, argv );
		TestStackAllocator( argc, argv );
		TestBuddyAllocator( argc, argv );
	}

	return 0;
}
