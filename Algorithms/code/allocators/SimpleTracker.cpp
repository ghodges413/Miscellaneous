//
//  SimpleTracker.cpp
//
//
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "SimpleTracker.h"
#include "BaseAllocator.h"

/*
====================================================
SimpleTracker
====================================================
*/
class SimpleTracker : public BaseAllocator {
public:
	static size_t m_allocated;

	void * Allocate( size_t size ) override {
		void * ptr = malloc( size + sizeof( int ) );
		if ( NULL != ptr ) {
			m_allocated += size + sizeof( int );
		}

		printf( "Allocated %i: Total allocated %i\n", (int)size, (int)m_allocated );

		((int*)ptr)[ 0 ] = (int)size;
		return (void*)(((int*)ptr) + 1 );
	}
	void Free( void * ptr ) override {
		if ( NULL == ptr ) {
			return;
		}

		int size = (((int*)ptr) - 1 )[ 0 ];
		m_allocated -= size + sizeof( int );

		printf( "Freed %i: Total allocated %i\n", (int)size, (int)m_allocated );
		free( ((int*)ptr) - 1 );
	}
};

size_t SimpleTracker::m_allocated = 0;

SimpleTracker g_simpleTracker;

/*
====================================================
TestSimpleTracker
====================================================
*/
int TestSimpleTracker( int argc, char * argv[] ) {
	printf( "Testing Simple Tracker\n" );

	g_allocator = &g_simpleTracker;

	int * data_ptr = new int[ 100 ];
	int * data2 = new int;

	delete[] data_ptr;
	delete data2;
	return 0;
}
