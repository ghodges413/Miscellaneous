//
//  StackAllocator.cpp
//	This is a simple stack allocator.  But something worth noting is from Jason Gregory's book Game Engine Architecture.
//	He mentioned that the old wave runner game used a double ended stack.  At level load it would allocate from the beginning
//	of the stack and hold onto that memory for the entire level.  But each frame, it would allocate from the other end of the
//	stack and release that memory at the end of each frame, when it is no longer needed.
//
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "StackAllocator.h"
#include "BaseAllocator.h"

typedef unsigned char byte_t;

/*
====================================================
StackAllocator
====================================================
*/
class StackAllocator : public BaseAllocator {
public:
	StackAllocator() : BaseAllocator() {
		m_basePtr = (byte_t*)malloc( m_size );
		m_stackPtr = m_basePtr;
		if ( NULL != m_basePtr ) {
			m_endPtr = m_basePtr + m_size;
		}
	}
	~StackAllocator() {
		if ( NULL != m_basePtr ) {
			free( m_basePtr );
		}

		m_basePtr = NULL;
		m_stackPtr = NULL;
		m_endPtr = NULL;
	}

	void * Allocate( size_t size ) override;
	void Free( void * ptr ) override;	

private:
	byte_t * m_basePtr;
	byte_t * m_stackPtr;
	byte_t * m_endPtr;

	static const int m_size = 65536;
};

/*
====================================================
StackAllocator::Allocate
====================================================
*/
void * StackAllocator::Allocate( size_t size ) {
	if ( NULL == m_basePtr ) {
		return NULL;
	}

	if ( m_stackPtr + size >= m_endPtr ) {
		return NULL;
	}

	// Store the size of this chunk at the start of it
	int * intPtr = (int*)m_stackPtr;
	*intPtr = (int)size;

	// Move the pointer to just after the stored size
	intPtr += 1;

	// Move the stack pointer to the first free spot
	m_stackPtr += size + (int)sizeof( int );

	// Return pointer
	return intPtr;
}

/*
====================================================
StackAllocator::Free
====================================================
*/
void StackAllocator::Free( void * ptr ) {
	if ( NULL == ptr ) {
		printf( "Attempted a free a null pointer\n" );
		return;
	}

	// Get the size of this chunk of memory
	const int size = (((int*)ptr) - 1 )[ 0 ];

	byte_t * headPtr = m_stackPtr - size;
	if ( headPtr != ptr ) {
		printf( "Unable to pop stack\n" );
		return;
	}

	// Pop this memory chunk and move the stack pointer
	m_stackPtr = (byte_t*)(((int*)ptr) - 1 );

	// calculate total allocated
	const int allocated = (int)(m_stackPtr - m_basePtr);

	printf( "Freed %i: Total allocated %i\n", size, allocated );
}

StackAllocator g_stackAllocator;

/*
====================================================
TestStackAllocator
====================================================
*/
int TestStackAllocator( int argc, char * argv[] ) {
	printf( "Testing Stack Allocator\n" );

	g_allocator = &g_stackAllocator;

	int * data_ptr = new int[ 100 ];
	int * data2 = new int;

	delete data2;
	delete[] data_ptr;
	return 0;
}
