/*
 *  HashTable.h
 *
 */
#pragma once
#include <stddef.h>

/*
 ================================
 HashNode
 ================================
 */
template < class T >
struct HashNode {
	HashNode();
	HashNode( const T & element );
	
	T m_data;
	HashNode * m_next;
};

/*
 ================================
 HashNode::HashNode
 ================================
 */
template < class T >
inline HashNode< T >::HashNode() :
m_data(),
m_next( NULL ) {
}

/*
 ================================
 HashNode::HashNode
 ================================
 */
template < class T >
inline HashNode< T >::HashNode( const T & element ) :
m_data( element ),
m_next( NULL ) {
}


/*
 ================================
 HashTable
 ================================
 */
template < class T >
class HashTable {
public:
	HashTable();
	~HashTable() {}
	
	void Add( const T & element );
	void Remove( const T & element );

	int GetHash( int key ) const;
	
private:
	static const int p = 7;
	static const int m_max = 1 << p;
	HashNode< T > * m_nodes[ m_max ];
};

/*
================================
HashTable::HashTable
================================
*/
template < class T >
HashTable< T >::HashTable() {
	for ( int i = 0; i < m_max; i++ ) {
		m_nodes[ i ] = NULL;
	}
}

/*
 ================================
 HashTable::Add
 ================================
 */
template < class T >
inline void HashTable< T >::Add( const T & element ) {
	int key = element.GetKey();
	int hash = GetHash( key );

	HashNode< T > * newNode = HashNode< T >( element );
	newNode->m_next = m_nodes[ hash ];
	m_nodes[ hash ] = newNode;
}

/*
================================
HashTable::Remove
================================
*/
template < class T >
inline void HashTable< T >::Remove( const T & element ) {
	int key = element.GetKey();
	int hash = GetHash( key );

	HashNode< T > * parent = m_nodes[ hash ];
	HashNode< T > * node = m_nodes[ hash ];
	while ( NULL != node ) {
		if ( node->m_data == element ) {
			parent->m_next = node->m_next;
			delete node;
			break;
		}

		parent = node;
		node = node->m_next;
	}
}

/*
================================
HashTable::GetHash
Simple multiplicative hash function
================================
*/
template < class T >
inline int HashTable< T >::GetHash( int key ) const {
#if 0
	// Modular hashing
	// h = k mod m
	int h = key & ( m_max - 1 );
	return h;
#else
	// Multiplicative hashing
	// int h = ( ( a * k ) % w ) / ( w / m );

	// s = a * 2^w
	unsigned int s = 2654435769;
	unsigned int w = 32;
	int h = ( s * key ) >> ( w - p );
    return h;
#endif
}

