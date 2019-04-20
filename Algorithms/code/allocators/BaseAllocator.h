//
//  BaseAllocator.h
//
#pragma once
#include <stdio.h>

/*
====================================================
BaseAllocator
====================================================
*/
class BaseAllocator {
public:
	virtual void * Allocate( size_t size );
	virtual void Free( void * ptr );
};

extern BaseAllocator * g_allocator;

