//
//  AVLTree.h
//	Named after the russian inventors Adelson, Velsky, Landis
//	The AVL tree is a self balancing binary tree.
//
#pragma once
#include <stddef.h>

/*
 ===============================
 AVLNode
 ===============================
 */
template < class T >
struct AVLNode {
    AVLNode( T & data ) : m_left( NULL ), m_right( NULL ), m_data( data ) {}
    
    AVLNode * m_left;
    AVLNode * m_right;
    
    T m_data;
};

/*
 ===============================
 AVLTree
 ===============================
 */
template < class T >
class AVLTree {
public:
	AVLTree() : m_root( NULL ) {}
	~AVLTree() {}
    
    void Insert( T & rhs );
    void Delete( T & rhs );

private:
	void Rebalance();
	static void Rebalance_r( AVLNode< T > * node, AVLNode< T > * parent );


	static void RotateLeft( AVLNode< T > * node, AVLNode< T > * parent );
	static void RotateRight( AVLNode< T > * node, AVLNode< T > * parent );

	static int Balance( AVLNode< T > * node );
	static void Depth_r( AVLNode< T > * node, int & depth );

private:
    AVLNode< T > * m_root;
};

/*
 ===============================
 AVLTree::Insert
 ===============================
 */
template < class T >
void AVLTree< T >::Insert( T & rhs ) {
	AVLNode< T > * node = m_root;
	while ( NULL != node ) {
		if ( rhs < node->m_data ) {
			node = node->m_left;
		} else {
			node = node->m_right;
		}
	}

	node = new AVLNode< T >( rhs );
	Rebalance();
}

/*
 ===============================
 AVLTree::Delete
 ===============================
 */
template < class T >
void AVLTree< T >::Delete( T & rhs ) {
	AVLNode< T > * parent = m_root;
	AVLNode< T > * node = m_root;
	while ( NULL != node ) {
		if ( rhs == node->m_data ) {
			break;
		}

		if ( rhs < node->m_data ) {
			node = node->m_left;
		} else {
			node = node->m_right;
		}
	}

	// Node containing data wasn't found, return early
	if ( NULL == node ) {
		return;
	}

	// If this node has no children, then just set the pointer from the parent to null
	if ( NULL == node->m_left && NULL == node->m_right ) {
		if ( parent->m_left == node ) {
			parent->m_left = NULL;
		}
		if ( parent->m_right == node ) {
			parent->m_right = NULL;
		}
		delete node;
		Rebalance();
		return;
	}

	// If this node is only right-sided, then set its right child to the parent's child
	if ( NULL == node->m_left ) {
		if ( parent->m_left == node ) {
			parent->m_left = node->m_right;
		}
		if ( parent->m_right == node ) {
			parent->m_right = node->m_right;
		}
		delete node;
		Rebalance();
		return;
	}

	// If this node is only left-sided, then set its left child to the parent's child
	if ( NULL == node->m_right ) {
		if ( parent->m_left == node ) {
			parent->m_left = node->m_left;
		}
		if ( parent->m_right == node ) {
			parent->m_right = node->m_left;
		}
		delete node;
		Rebalance();
		return;
	}

	// Now we need to deal with when this node has two children of its own
	if ( parent->m_left == node ) {
		// if this node was on the left, then all its children are on the left, and thus the left child becomes the new left
		parent->m_left = node->m_left;

		// Every node in left is guaranteed to be left of right, so the new right has to go to the right of all its rights
		AVLNode< T > * newCheck = node->m_left;
		AVLNode< T > * newParent = node->m_left;
		while ( NULL != newCheck ) {
			newParent = newCheck;
			newCheck = newCheck->m_right;
		}

		newParent->m_right = node->m_right;
		delete node;
		Rebalance();
		return;
	}

	// Now we need to deal with when this node has two children of its own
	if ( parent->m_right == node ) {
		// if this node was on the left, then all its children are on the left, and thus the left child becomes the new left
		parent->m_right = node->m_right;

		// Every node in left is guaranteed to be left of right, so the new right has to go to the right of all its rights
		AVLNode< T > * newCheck = node->m_right;
		AVLNode< T > * newParent = node->m_right;
		while ( NULL != newCheck ) {
			newParent = newCheck;
			newCheck = newCheck->m_left;
		}

		newParent->m_left = node->m_left;
		delete node;
		Rebalance();
		return;
	}
}

/*
===============================
AVLTree::Rebalance
===============================
*/
template < class T >
void AVLTree< T >::Rebalance() {
	// This should be possible in a recursive fashion.
	// We calculate the heights of the left and right side of the tree.
	// If the difference is greater than one or less than negative one,
	// then we rotate the node in the appropriate direction.

	// The question is, do we do this in a bottom up or top down approach?
	// Do it bottom up.  Go down to the leaves of the tree and work your way back up,
	// balancing the tree as you go.
	Rebalance_r( m_root, NULL );
}

/*
===============================
AVLTree::Rebalance_r
===============================
*/
template < class T >
void AVLTree< T >::Rebalance_r( AVLNode< T > * node, AVLNode< T > * parent ) {
	if ( NULL == node ) {
		return;
	}

	// Go to the bottom and let the bottom do itself first
	Rebalance_r( node->m_left, node );
	Rebalance_r( node->m_right, node );

	// Now, figure out the balance of this node
	int balance = Balance( node );

	// if the balance is negative, then it's left heavy and needs to rotate right
	if ( balance < -1 ) {
		RotateRight( node, parent );
	} else if ( balance > 1 ) {
		RotateLeft( node, parent );
	}
}

/*
===============================
AVLTree::Balance
===============================
*/
template < class T >
int AVLTree< T >::Balance( AVLNode< T > * node ) {
	if ( NULL == node ) {
		return 0;
	}

	int depthLeft = 0;
	int depthRight = 0;

	Depth_r( node->m_left, depthLeft );
	Depth_r( node->m_right, depthRight );

	int balance = depthRight - depthLeft;
	return balance;
}

/*
===============================
AVLTree::Depth_r
===============================
*/
template < class T >
void AVLTree< T >::Depth_r( AVLNode< T > * node, int & depth ) {
	if ( NULL == node ) {
		return;
	}

	depth++;

	int depthLeft = 0;
	int depthRight = 0;

	Depth_r( node->m_left, depthLeft );
	Depth_r( node->m_right, depthRight );

	if ( depthLeft > depthRight ) {
		depth += depthLeft;
	} else {
		depth += depthRight;
	}	
}

/*
===============================
AVLTree::RotateLeft
===============================
*/
template < class T >
void AVLTree< T >::RotateLeft( AVLNode< T > * node, AVLNode< T > * parent ) {
	// Rotate left means that we take the child on the right of the node and make it the new parent
	// While making the node the left child of the that node.  Here's a visual depiction:
	//     N                               C
	//       \                           /   \
	//        C                        N       R
	//      /   \                       \
	//     L     R                        L
	AVLNode< T > * child = node->m_right;
	node->m_right = child->m_left;
	child->m_left = node;

	// Fix up the parent if it exists
	if ( NULL != parent ) {
		if ( parent->m_left == node ) {
			parent->m_left = child;
		}
		if ( parent->m_right == node ) {
			parent->m_right = child;
		}
	}
}

/*
===============================
AVLTree::RotateRight
===============================
*/
template < class T >
void AVLTree< T >::RotateRight( AVLNode< T > * node, AVLNode< T > * parent ) {
	// Rotate left means that we take the child on the left of the node and make it the new parent
	// While making the node the right child of the that node.  Here's a visual depiction:
	//          N                          C
	//         /                         /   \
	//        C                        L       N
	//      /   \                            /
	//     L     R                          R
	AVLNode< T > * child = node->m_left;
	node->m_left = child->m_right;
	child->m_right = node;

	// Fix up the parent if it exists
	if ( NULL != parent ) {
		if ( parent->m_left == node ) {
			parent->m_left = child;
		}
		if ( parent->m_right == node ) {
			parent->m_right = child;
		}
	}
}