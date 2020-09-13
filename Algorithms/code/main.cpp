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

typedef int TestFunction_t( int argc, char * argv[] );

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
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

	return 0;
}
