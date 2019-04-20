//
//  BaseAllocator.cpp
//
//
#include <malloc.h>
#include <stdlib.h>
#include "BaseAllocator.h"

BaseAllocator g_baseAllocator;
BaseAllocator * g_allocator = &g_baseAllocator;

/*
====================================================
BaseAllocator::Allocate
====================================================
*/
void * BaseAllocator::Allocate( size_t size ) {
	size = ( size + 15 ) & ~15;
	return _aligned_malloc( size, 16 );
}

/*
====================================================
BaseAllocator::Free
====================================================
*/
void BaseAllocator::Free( void * ptr ) {
	if ( NULL == ptr ) {
		return;
	}

	_aligned_free( ptr );
}

/*
====================================================
new/delete overloads
====================================================
*/

void * operator new( size_t size ) {
	return g_allocator->Allocate( size );
}
void operator delete( void * ptr ) {
	g_allocator->Free( ptr );
}
void *operator new[]( size_t size ) {
	return g_allocator->Allocate( size );
}
void operator delete[]( void * ptr ) {
	g_allocator->Free( ptr );
}

