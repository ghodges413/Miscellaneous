/*
 *  List.h
 *
 */
#pragma once
#include <assert.h>

/*
 ================================
 List
 ================================
 */
template < class T >
class List {
public:
	List( const int expansionStep = 16 );
    List< T > & operator = ( const List< T > & rhs );
	~List();
	
	void	Clear() { m_numElements = 0; }
	void	Empty() { delete[] m_data; m_data = NULL; m_size = 0; m_numElements = 0; }
	int		Num() const { return m_numElements; }
	
	size_t	Allocated()		const { return sizeof( T ) * m_size; }						// Total memory reserved
	size_t	Size()			const { return sizeof( T ) * m_numElements; }				// memory used by the elements
	size_t	UnusedMemory()	const { return sizeof( T ) * ( m_size - m_numElements ); }	// Extra space that's currently not in use
	
	void	Append( const T & element );
	void	AppendUnique( const T & element );
	void	AppendArray( const List< T > & rhs );
	void	Expand( const int newSize );
	void	Shrink( const int newSize );
	void	Resize( const int newSize );
	
	const   T & operator[]( const int idx ) const;
            T & operator[]( const int idx );
	
	const   T * ToPtr() const   { return m_data; }
            T * ToPtr()         { return m_data; }
	
	void	Remove( const int idx );
	bool	RemoveElement( const T & element );
	void	Insert( const T & element, const int idx );
	
	int		Find( const T & element ) const;
	
private:
	int m_numElements;	// number of elements in the array
	int m_size;			// number of elements that can be stored
	int m_expansionSize;	// number of elements to add when auto-expansion is required
	T * m_data;
};

/*
 ================================
 List::List
 ================================
 */
template < class T >
List< T >::List( const int expansionStep ) :
m_data( NULL ),
m_size( 0 ),
m_numElements( 0 ),
m_expansionSize( expansionStep ) {
}

/*
 ================================
 List::operator =
 ================================
 */
template < class T >
List< T > & List< T >::operator=( const List< T > & rhs ) {
    m_numElements = rhs.m_numElements;
    m_size = rhs.m_size;
    m_expansionSize = rhs.m_expansionSize;
    m_data = new T[ m_size ];
    for ( int i = 0; i < m_numElements; ++i ) {
        m_data[ i ] = rhs.m_data[ i ];
    }
    return *this;
}

/*
 ================================
 List::~List
 ================================
 */
template < class T >
List< T >::~List() {
	if ( m_data ) {
		delete[] m_data;
		m_data = NULL;
	}
	m_size = 0;
	m_numElements = 0;
}

/*
 ================================
 List::Append
 ================================
 */
template < class T >
void List< T >::Append( const T & element ) {
	if ( m_numElements == m_size ) {
		// Expand array
		Expand( m_size + m_expansionSize + 1 );
	}
	
	// copy element into array
	m_data[ m_numElements ] = element;
	++m_numElements;
}

/*
 ================================
 List::AppendUnique
 * same as above, but does not allow duplicates
 ================================
 */
template < class T >
void List< T >::AppendUnique( const T & element ) {
	for ( int i = 0; i < m_numElements; ++i ) {
		if ( element == m_data[ i ] ) {
			// if this element already exists, do not add
			return;
		}
	}
	
	Append( element );
}

/*
 ================================
 List::AppendArray
 ================================
 */
template < class T >
void List< T >::AppendArray( const List< T > & rhs ) {
	for ( int i = 0; i < rhs.Num(); ++i ) {
		Append( rhs[ i ] );
	}
}

/*
 ================================
 List::Expand
 ================================
 */
template < class T >
void List< T >::Expand( const int newSize ) {
	assert( newSize > m_size );
	
	// create a new array
	T* tmpArray = new T[ newSize ];
	
	// copy elements
	for ( int i = 0; i < m_numElements; ++i ) {
		tmpArray[ i ] = m_data[ i ];
	}
	
	// delete old array and set to the new one
	delete[] m_data;
	m_data = tmpArray;
	m_size = newSize;
}

/*
 ================================
 List::Shrink
 ================================
 */
template < class T >
void List< T >::Shrink( const int newSize ) {
	assert( newSize < m_size );
	if ( newSize < 0 ) {
		Empty();
		return;
	}
	
	// create a new array
	T* tmpArray = new T[ newSize ];
	
	// copy elements
//	memcpy( tmpArray, mArray, sizeof( T ) * newSize );
	for ( int i = 0; i < m_numElements; ++i ) {
		tmpArray[ i ] = m_data[ i ];
	}
	
	// delete old array and set to the new one
	delete[] m_data;
	m_data = tmpArray;
	m_size = newSize;
}

/*
 ================================
 List::Resize
 ================================
 */
template < class T >
void List< T >::Resize( const int newSize ) {
	if ( newSize > m_size ) {
		Expand( newSize );
	} else if ( newSize < m_size ) {
		Shrink( newSize );
	}
}

/*
 ================================
 List::operator[]
 ================================
 */
template < class T >
const T & List< T >::operator[]( const int idx ) const {
	assert( idx >= 0 ); 
//	assert( idx < mNumElements ); 
    assert( idx < m_size );
	
	return m_data[ idx ];
}

/*
 ================================
 List::operator[]
 ================================
 */
template < class T >
T & List< T >::operator[]( const int idx ) {
	assert( idx >= 0 ); 
//	assert( idx < mNumElements ); 
    assert( idx < m_size );
	
	return m_data[ idx ]; 
}

/*
 ================================
 List::Remove
 ================================
 */
template < class T >
void List< T >::Remove( const int idx ) {
	assert( idx >= 0 );
	assert( idx < m_numElements );
	
	--m_numElements;
	for ( int i = idx; i < m_numElements; ++i ) {
		m_data[ i ] = m_data[ i + 1 ];
	}
}

/*
 ================================
 List::RemoveElement
 ================================
 */
template < class T >
bool List< T >::RemoveElement( const T & element ) {
	for ( int i = 0; i < m_numElements; ++i ) {
		if ( m_data[ i ] == element ) {
			Remove( i );
			return true;
		}
	}
	
	return false;
}

/*
 ================================
 List::Insert
 ================================
 */
template < class T >
void List< T >::Insert( const T & element, const int idx ) {
	assert( idx >= 0 );
	assert( idx < m_numElements );
	
	// check if array needs to expand
	if ( m_numElements == m_size ) {
		Expand( m_size + m_expansionSize );
	}
	
	// shift elements down one
	++m_numElements;
	for ( int i = m_numElements; i > idx; --i ) {
		m_data[ i ] = m_data[ i - 1 ];
	}
	
	// insert element
	m_data[ idx ] = element;
}

/*
 ================================
 List::Find
 * returns the index of the element found at the location
 * returns -1 if the index was not found
 ================================
 */
template < class T >
int List< T >::Find( const T & element ) const {
	int idx = -1;
	for ( int i = 0; i < m_numElements; ++i ) {
		if ( m_data[ i ] == element ) {
			idx = i;
			break;
		}
	}
	
	return idx;
}

