//
//  MemoryPool.cpp
//

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include "MemoryPool.h"
#include "BaseAllocator.h"

typedef unsigned char byte_t;

struct PoolNode_t {
	PoolNode_t * m_next;
};

/*
====================================================
MemoryPool
====================================================
*/
class MemoryPool : public BaseAllocator {
public:
	MemoryPool() : BaseAllocator() {
		m_data = malloc( m_blockSize * m_numBlocks );
		m_freeList = NULL;
		
		// Build the free list
		if ( NULL != m_data ) {
			for ( int i = 0; i < m_numBlocks; i++ ) {
				Insert( (byte_t *)m_data + m_blockSize * i );
			}			
		}
	}
	~MemoryPool() {
		free( m_data );
		m_data = NULL;
		m_freeList = NULL;
	}

	void * Allocate( size_t size ) override;
	void Free( void * ptr ) override;

private:
	void Insert( void * ptr );

	static const int m_blockSize = 4096;
	static const int m_numBlocks = 128;

	void * m_data;				// The chunk of memory
	PoolNode_t * m_freeList;	// The linked list of free blocks
};

/*
====================================================
MemoryPool::Allocate
====================================================
*/
void * MemoryPool::Allocate( size_t size ) {
	if ( (int)size > m_blockSize + (int)sizeof( PoolNode_t ) ) {
		printf( "Unable to allocate size %i\n", (int)size );
		return NULL;
	}

	if ( NULL == m_freeList ) {
		printf( "No free blocks available\n" );
		return NULL;
	}

	// Get a pointer to the data
	void * ptr = (void*)( m_freeList + 1 );

	// Remove this block from the linked list of free blocks
	m_freeList = m_freeList->m_next;

	return ptr;
}

/*
====================================================
MemoryPool::Free
====================================================
*/
void MemoryPool::Free( void * ptr ) {
	if ( NULL == ptr ) {
		return;
	}

	PoolNode_t * node = ((PoolNode_t *)ptr) - 1;
	Insert( node );
}

/*
====================================================
MemoryPool::Insert
====================================================
*/
void MemoryPool::Insert( void * ptr ) {
	if ( NULL == m_freeList ) {
		m_freeList = (PoolNode_t*)ptr;
		m_freeList->m_next = NULL;
		return;
	}

	PoolNode_t * node = (PoolNode_t *)ptr;
	PoolNode_t * next = m_freeList->m_next;
	m_freeList->m_next = node;
	node->m_next = next;
}

MemoryPool g_memoryPool;

/*
====================================================
TestMemoryPool
====================================================
*/
int TestMemoryPool( int argc, char * argv[] ) {
	printf( "Testing Memory Pool\n" );

	g_allocator = &g_memoryPool;

	int * data_ptr = new int[ 100 ];
	int * data2 = new int;

	delete[] data_ptr;
	delete data2;
	return 0;
}
