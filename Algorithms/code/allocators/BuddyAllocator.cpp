//
//  BuddyAllocator.cpp
//
//
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "BuddyAllocator.h"
#include "BaseAllocator.h"


typedef unsigned char uint8;

struct BuddyNode_t {
	enum NodeState_t {
		NODE_UNUSED = 0,
		NODE_USED = 1,
		NODE_SPLIT = 2,
		NODE_FULL = 3,
	};

	int m_level;
	int m_id;	// the id for this level
	int m_state;
	int m_intptr;	// an intptr into into the heap
	BuddyNode_t * m_parent;
	BuddyNode_t * m_children[ 2 ];
};

/*
====================================================
BuddyAllocator
====================================================
*/
class BuddyAllocator : public BaseAllocator {
public:
	BuddyAllocator() {
		int maxSize = GetSizeForLevel( m_maxLevels );

		int numNodes = 1;
		for ( int i = 0; i < m_maxLevels; i++ ) {
			numNodes += 1 << ( i + 1 );
		}

		int headerSpace = sizeof( BuddyNode_t ) * numNodes;

		int size = 1 << m_maxLevels;
		m_root = (BuddyNode_t *)malloc( maxSize + headerSpace );
		m_heapPtr = m_root + numNodes;
		int index = 0;
		m_root->m_intptr = 0;
		m_root->m_parent = NULL;
		ConstructTree_r( m_root, m_maxLevels, index, true );
	}

	~BuddyAllocator() {
		free( m_root );
	}

	void * Allocate( size_t size ) override;
	void Free( void * ptr ) override;

	const char * StateToStr( const int state ) const;
	void Report() const;

private:
	static const int m_maxLevels = 5;
	static const int m_minBlockSize = 1024;	// in bytes

	BuddyNode_t * m_root;
	void * m_heapPtr;

	void ConstructTree_r( BuddyNode_t * node, int level, int & index, bool isLow );
	BuddyNode_t * FindNode_r( BuddyNode_t * node, int sizeRequest );
	void UpdateNodeState_r( BuddyNode_t * node );

	static int GetSizeForLevel( const int level ) { return ( m_minBlockSize << level ); }

	static const char * m_strUnused;
	static const char * m_strUsed;
	static const char * m_strSplit;
	static const char * m_strFull;

	void Report_r( BuddyNode_t * node ) const;
};

const char * BuddyAllocator::m_strUnused = "NODE_UNUSED";
const char * BuddyAllocator::m_strUsed = "NODE_USED";
const char * BuddyAllocator::m_strSplit = "NODE_SPLIT";
const char * BuddyAllocator::m_strFull = "NODE_FULL";

/*
====================================================
BuddyAllocator::Allocate
====================================================
*/
void * BuddyAllocator::Allocate( size_t size ) {
	// We need to find a free block for allocating
	int intptr = 0;
	BuddyNode_t * foundNode = FindNode_r( m_root, (int)size );
	if ( NULL == foundNode ) {
		printf( "No available node found!\n" );
		return foundNode;
	}

	// Now that we have a node, we need to locate it in the heap
	uint8 * bytePtr = ((uint8 *)m_heapPtr) + foundNode->m_intptr;
	int * poop = (int *)bytePtr;
	poop[ 0 ] = foundNode->m_id;
	return ( poop + 1 );
}

/*
====================================================
BuddyAllocator::Free
====================================================
*/
void BuddyAllocator::Free( void * ptr ) {
	if ( NULL == ptr ) {
		return;
	}

	// TODO: Use this ptr as an inverse lookup, maybe we should put some header data into the ptr's we give out to make it easier to do a reverse lookup
	// Maybe we should do a bounds check on the address to make sure it's in the valid space.
	// You know, there's a lot of things we should do with this allocator... but this is just an exercise for us.  So who cares right now?
	int index = (((int*)ptr) - 1)[ 0 ];
	BuddyNode_t * node = m_root + index;
	node->m_state = BuddyNode_t::NODE_UNUSED;

	// TODO: Parse up the parentals and update their states.
	UpdateNodeState_r( node->m_parent );
}

/*
====================================================
BuddyAllocator::StateToStr
====================================================
*/
const char * BuddyAllocator::StateToStr( const int state ) const {
	const char * text = m_strUnused;

	if ( state == BuddyNode_t::NODE_USED ) {
		text = m_strUsed;
	} else if ( state == BuddyNode_t::NODE_SPLIT ) {
		text = m_strSplit;
	} else if ( state == BuddyNode_t::NODE_FULL ) {
		text = m_strFull;
	}

	return text;
}

/*
====================================================
BuddyAllocator::Report_r
====================================================
*/
void BuddyAllocator::Report_r( BuddyNode_t * node ) const {
	if ( NULL == node ) {
		return;
	}

	printf( "Node %i: Level %i: State: %s\n", node->m_id, node->m_level, StateToStr( node->m_state ) );
	if ( node->m_children[ 0 ] != NULL && node->m_children[ 1 ] != NULL ) {
		printf( "    Child Node %i: Level %i: State: %s\n", node->m_children[ 0 ]->m_id, node->m_children[ 0 ]->m_level, StateToStr( node->m_children[ 0 ]->m_state ) );
		printf( "    Child Node %i: Level %i: State: %s\n", node->m_children[ 1 ]->m_id, node->m_children[ 1 ]->m_level, StateToStr( node->m_children[ 1 ]->m_state ) );
	} else {
		printf( "    Leaf\n" );
	}

	Report_r( node->m_children[ 0 ] );
	Report_r( node->m_children[ 1 ] );
}

/*
====================================================
BuddyAllocator::Report
====================================================
*/
void BuddyAllocator::Report() const {
	printf( "==========================================\n" );
	printf( "Buddy Allocator Report\n" );
	Report_r( m_root );
	printf( "==========================================\n" );
}

/*
====================================================
BuddyAllocator::ConstructTree_r
====================================================
*/
void BuddyAllocator::ConstructTree_r( BuddyNode_t * node, int level, int & index, bool isLow ) {
	node->m_level = level;
	node->m_id = index;
	node->m_state = BuddyNode_t::NODE_UNUSED;
	node->m_children[ 0 ] = NULL;
	node->m_children[ 1 ] = NULL;
	if ( NULL != node->m_parent ) {
		if ( isLow ) {
			node->m_intptr = node->m_parent->m_intptr;
		} else {
			node->m_intptr = node->m_parent->m_intptr + ( GetSizeForLevel( level ) >> 1 );
		}
	}

	// Check if we're done building children
	assert( level >= 0 );
	if ( 0 == level ) {
		return;
	}

	index++;
	node->m_children[ 0 ] = m_root + index;
	node->m_children[ 0 ]->m_parent = node;
	ConstructTree_r( node->m_children[ 0 ], level - 1, index, true );	// these aren't right yet

	index++;
	node->m_children[ 1 ] = m_root + index;
	node->m_children[ 1 ]->m_parent = node;
	ConstructTree_r( node->m_children[ 1 ], level - 1, index, false );	// these aren't right yet
}

/*
====================================================
BuddyAllocator::FindNode_r
====================================================
*/
BuddyNode_t * BuddyAllocator::FindNode_r( BuddyNode_t * node, int sizeRequest ) {
	if ( NULL == node ) {
		return NULL;
	}

	const int size = GetSizeForLevel( node->m_level );	// 64 bytes can be the smallest size, sure
	if ( sizeRequest > size ) {
		return NULL;
	}

	BuddyNode_t * foundNodeA = FindNode_r( node->m_children[ 0 ], sizeRequest );
	if ( foundNodeA != NULL ) {
		node->m_state = BuddyNode_t::NODE_SPLIT;
		return foundNodeA;
	}

	BuddyNode_t * foundNodeB = FindNode_r( node->m_children[ 1 ], sizeRequest );
	if ( foundNodeB != NULL ) {
		node->m_state = BuddyNode_t::NODE_SPLIT;
		return foundNodeB;
	}

	// No available node found
	// Check the size of this node, if it's free and not split then 
	if ( node->m_state == BuddyNode_t::NODE_UNUSED ) {
		node->m_state = BuddyNode_t::NODE_USED;
		return node;
	}

	return NULL;
}

/*
====================================================
BuddyAllocator::UpdateNodeState_r
====================================================
*/
void BuddyAllocator::UpdateNodeState_r( BuddyNode_t * node ) {
	if ( NULL == node ) {
		return;
	}

	const int oldState = node->m_state;

	if ( node->m_children[ 0 ]->m_state == BuddyNode_t::NODE_UNUSED && node->m_children[ 1 ]->m_state == BuddyNode_t::NODE_UNUSED ) {
		node->m_state = BuddyNode_t::NODE_UNUSED;
	} else if ( node->m_children[ 0 ]->m_state == BuddyNode_t::NODE_SPLIT || node->m_children[ 1 ]->m_state == BuddyNode_t::NODE_SPLIT ) {
		node->m_state = BuddyNode_t::NODE_SPLIT;
	} else if ( node->m_children[ 0 ]->m_state == BuddyNode_t::NODE_FULL && node->m_children[ 1 ]->m_state == BuddyNode_t::NODE_FULL ) {
		node->m_state = BuddyNode_t::NODE_FULL;
	} else if ( node->m_children[ 0 ]->m_state == BuddyNode_t::NODE_USED && node->m_children[ 1 ]->m_state == BuddyNode_t::NODE_FULL ) {
		node->m_state = BuddyNode_t::NODE_FULL;
	} else if ( node->m_children[ 0 ]->m_state == BuddyNode_t::NODE_FULL && node->m_children[ 1 ]->m_state == BuddyNode_t::NODE_USED ) {
		node->m_state = BuddyNode_t::NODE_FULL;
	} else if ( node->m_children[ 0 ]->m_state == BuddyNode_t::NODE_USED && node->m_children[ 1 ]->m_state == BuddyNode_t::NODE_USED ) {
		node->m_state = BuddyNode_t::NODE_FULL;
	}

	if ( oldState != node->m_state ) {
		UpdateNodeState_r( node->m_parent );
	}
}

BuddyAllocator g_buddyAllocator;

/*
====================================================
TestBuddyAllocator
====================================================
*/
int TestBuddyAllocator( int argc, char * argv[] ) {
	printf( "Testing Buddy Allocator\n" );

	g_allocator = &g_buddyAllocator;

	int * data_ptr = new int[ 100 ];
	int * data2 = new int;

	g_buddyAllocator.Report();

	delete[] data_ptr;
	delete data2;

	g_buddyAllocator.Report();

	return 0;
}
