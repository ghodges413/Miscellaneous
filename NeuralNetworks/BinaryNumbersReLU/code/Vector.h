//
//	Vector.h
//
#pragma once
#include <math.h>
#include <assert.h>
#include <stdio.h>

/*
 ================================
 VecN
 ================================
 */
class VecN {
public:
	VecN() : N( 0 ), data( NULL ) {}
	VecN( int _N );
	VecN( const VecN & rhs );
	VecN & operator = ( const VecN & rhs );
	~VecN() { delete[] data; }

	float			operator[] ( const int idx ) const { return data[ idx ]; }
	float &			operator[] ( const int idx ) { return data[ idx ]; }
	const VecN &	operator *= ( float rhs );
	VecN			operator * ( float rhs ) const;
	VecN			operator + ( const VecN & rhs ) const;
	VecN			operator - ( const VecN & rhs ) const;
	const VecN &	operator += ( const VecN & rhs );
	const VecN &	operator -= ( const VecN & rhs );

	VecN Hadamard( const VecN & rhs ) const;
	float Dot( const VecN & rhs ) const;
	void Zero();
	
public:
	int		N;
	float *	data;
};

inline VecN::VecN( int _N ) {
	N = _N;
	data = new float[ _N ];
}

inline VecN::VecN( const VecN & rhs ) {
	N = rhs.N;
	data = new float[ N ];
	for ( int i = 0; i < N; i++ ) {
		data[ i ] = rhs.data[ i ];
	}
}

inline VecN & VecN::operator = ( const VecN & rhs ) {
	delete[] data;

	N = rhs.N;
	data = new float[ N ];
	for ( int i = 0; i < N; i++ ) {
		data[ i ] = rhs.data[ i ];
	}
	return *this;
}

inline const VecN & VecN::operator *= ( float rhs ) {
	for ( int i = 0; i < N; i++ ) {
		data[ i ] *= rhs;
	}
	return *this;
}

inline VecN VecN::operator * ( float rhs ) const {
	VecN tmp = *this;
	tmp *= rhs;
	return tmp;
}

inline VecN VecN::operator + ( const VecN & rhs ) const {
	VecN tmp = *this;
	for ( int i = 0; i < N; i++ ) {
		tmp.data[ i ] += rhs.data[ i ];
	}
	return tmp;
}

inline VecN VecN::operator - ( const VecN & rhs ) const {
	VecN tmp = *this;
	for ( int i = 0; i < N; i++ ) {
		tmp.data[ i ] -= rhs.data[ i ];
	}
	return tmp;
}

inline const VecN & VecN::operator += ( const VecN & rhs ) {
	for ( int i = 0; i < N; i++ ) {
		data[ i ] += rhs.data[ i ];
	}
	return *this;
}

inline const VecN & VecN::operator -= ( const VecN & rhs ) {
	for ( int i = 0; i < N; i++ ) {
		data[ i ] -= rhs.data[ i ];
	}
	return *this;
}

inline VecN VecN::Hadamard( const VecN & rhs ) const {
	assert( N == rhs.N );
	if ( N != rhs.N ) {
		return *this;
	}

	VecN result = rhs;
	for ( int i = 0; i < N; i++ ) {
		result[ i ] *= data[ i ];
	}

	return result;
}

inline float VecN::Dot( const VecN & rhs ) const {
	float sum = 0;
	for ( int i = 0; i < N; i++ ) {
		sum += data[ i ] * rhs.data[ i ];
	}
	return sum;
}

inline void VecN::Zero() {
	for ( int i = 0; i < N; i++ ) {
		data[ i ] = 0.0f;
	}
}