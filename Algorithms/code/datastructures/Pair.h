/*
 *  Pair.h
 *
 */
#pragma once

/*
 ================================
 Pair
 ================================
 */
template < class T, class R >
class Pair {
public:
	Pair() {}
    Pair< T, R >( const Pair< T, R > & rhs );
    Pair< T, R >( const T & first, const R & second );
    Pair< T, R > & operator=( const Pair< T, R > & rhs );
	~Pair() {}
	
public:
    T   m_first;
	R   m_second;
};

/*
 ================================
 Pair::Pair
 ================================
 */
template < class T, class R >
Pair< T, R >::Pair( const Pair< T, R > & rhs ) :
m_first( rhs.m_first ),
m_second( rhs.m_second ) {
}

/*
 ================================
 Pair::Pair
 ================================
 */
template < class T, class R >
Pair< T, R >::Pair( const T & first, const R & second ) :
m_first( first ),
m_second( second ) {
}

/*
 ================================
 Pair::operator =
 ================================
 */
template < class T, class R >
Pair< T, R > & Pair< T, R >::operator=( const Pair< T, R > & rhs ) {
    m_first  = rhs.m_first;
    m_second = rhs.m_second;
    return *this;
}
