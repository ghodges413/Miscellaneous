/*
 *  LinkedList.h
 *
 */
#pragma once
#include <stddef.h>

/*
 ================================
 LinkNode_t
 ================================
 */
template < class T >
struct LinkNode_t {
	LinkNode_t();
	LinkNode_t( const T & element );
	
	T m_data;
	LinkNode_t * m_next;
};

/*
 ================================
 LinkNode_t::LinkNode_t
 ================================
 */
template < class T >
inline LinkNode_t< T >::LinkNode_t() :
m_data(),
m_next( NULL ) {
}

/*
 ================================
 LinkNode_t::LinkNode_t
 ================================
 */
template < class T >
inline LinkNode_t< T >::LinkNode_t( const T & element ) :
m_data( element ),
m_next( NULL ) {
}


/*
 ================================
 LinkedList
 ================================
 */
template < class T >
class LinkedList {
public:
	LinkedList();
	~LinkedList();
	
	void Insert( const T & element );
	void Remove( const T & element );
	
private:
	LinkNode_t< T > * m_list;
};

/*
 ================================
 LinkedList::LinkedList
 ================================
 */
template < class T >
inline LinkedList< T >::LinkedList() :
m_list( NULL ) {
}

/*
 ================================
 LinkedList::~LinkedList
 ================================
 */
template < class T >
inline LinkedList< T >::~LinkedList() {
}

/*
 ================================
 LinkedList::Insert
 ================================
 */
template < class T >
inline void LinkedList< T >::Insert( const T & element ) {
	LinkNode_t< T > * node = new LinkNode_t< T >( element );
	node->m_next = m_list;
	m_list = node; 
}

/*
================================
LinkedList::Remove
================================
*/
template < class T >
inline void LinkedList< T >::Remove( const T & element ) {
	if ( NULL == m_list ) {
		return;
	}

	LinkNode_t< T > * prevNode = m_list;
	LinkNode_t< T > * node = m_list;
	while ( NULL != node ) {
		if ( node->m_data == element ) {
			if ( m_list == node ) {
				m_list = node->m_next;
			} else {
				prevNode->m_next = node->m_next;
			}

			delete node;
			break;
		}

		prevNode = node;
		node = node->m_next;
	}
}
