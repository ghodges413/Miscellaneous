//
//	Matrix.h
//
#pragma once
#include "Vector.h"

/*
====================================================
MatMN
====================================================
*/
class MatMN {
public:
	MatMN() : M( 0 ), N( 0 ) {}
	MatMN( int M, int N );
	MatMN( const MatMN & rhs ) {
		*this = rhs;
	}
	~MatMN() { delete[] rows; }

	const MatMN & operator = ( const MatMN & rhs );
	const MatMN & operator *= ( float rhs );
	VecN operator * ( const VecN & rhs ) const;
	MatMN operator * ( const MatMN & rhs ) const;
	MatMN operator * ( const float rhs ) const;
	const MatMN & operator += ( const MatMN & rhs );

	void Zero();
	MatMN Transpose() const;

public:
	int		M;	// M rows
	int		N;	// N columns
	VecN *	rows;
};

inline MatMN::MatMN( int _M, int _N ) {
	M = _M;
	N = _N;
	rows = new VecN[ M ];
	for ( int m = 0; m < M; m++ ) {
		rows[ m ] = VecN( N );
	}
}

inline const MatMN & MatMN::operator = ( const MatMN & rhs ) {
	M = rhs.M;
	N = rhs.N;
	rows = new VecN[ M ];
	for ( int m = 0; m < M; m++ ) {
		rows[ m ] = rhs.rows[ m ];
	}
	return *this;
}

inline const MatMN & MatMN::operator *= ( float rhs ) {
	for ( int m = 0; m < M; m++ ) {
		rows[ m ] *= rhs;
	}
	return *this;
}

inline VecN MatMN::operator * ( const VecN & rhs ) const {
	// Check that the incoming vector is of the correct dimension
	if ( rhs.N != N ) {
		return rhs;
	}

	VecN tmp( M );
	for ( int m = 0; m < M; m++ ) {
		tmp[ m ] = rhs.Dot( rows[ m ] );
	}
	return tmp;
}

inline MatMN MatMN::operator * ( const MatMN & rhs ) const {
	// Check that the incoming matrix of the correct dimension
	if ( rhs.M != N && rhs.N != M ) {
		return rhs;
	}

	MatMN tranposedRHS = rhs.Transpose();

	MatMN tmp( M, rhs.N );
	for ( int m = 0; m < M; m++ ) {
		for ( int n = 0; n < rhs.N; n++ ) {
			tmp.rows[ m ][ n ] = rows[ m ].Dot( tranposedRHS.rows[ n ] );
		}
	}
	return tmp;
}

inline MatMN MatMN::operator * ( const float rhs ) const {
	MatMN tmp = *this;
	for ( int m = 0; m < M; m++ ) {
		for ( int n = 0; n < N; n++ ) {
			tmp.rows[ m ][ n ] *= rhs;
		}
	}
	return tmp;
}

inline const MatMN & MatMN::operator += ( const MatMN & rhs ) {
	assert( M == rhs.M );
	assert( N == rhs.N );

	for ( int m = 0; m < M; m++ ) {
		for ( int n = 0; n < N; n++ ) {
			rows[ m ][ n ] += rhs.rows[ m ][ n ];
		}
	}
	return *this;
}

inline void MatMN::Zero() {
	for ( int m = 0; m < M; m++ ) {
		rows[ m ].Zero();
	}
}

inline MatMN MatMN::Transpose() const {
	MatMN tmp( N, M );
	for ( int m = 0; m < M; m++ ) {
		for ( int n = 0; n < N; n++ ) {
			tmp.rows[ n ][ m ] = rows[ m ][ n ];
		}		
	}
	return tmp;
}

/*
====================================================
MatN
====================================================
*/
class MatN {
public:
	MatN() : numDimensions( 0 ) {}
	MatN( int N );
	MatN( const MatN & rhs ) {
		*this = rhs;
	}
	MatN( const MatMN & rhs ) {
		*this = rhs;
	}
	~MatN() { delete[] rows; }

	const MatN & operator = ( const MatN & rhs );
	const MatN & operator = ( const MatMN & rhs );

	void Identity();
	void Zero();
	void Transpose();

	void operator *= ( float rhs );
	VecN operator * ( const VecN & rhs );
	MatN operator * ( const MatN & rhs );

public:
	int		numDimensions;
	VecN *	rows;
};

inline MatN::MatN( int N ) {
	numDimensions = N;
	rows = new VecN[ N ];
	for ( int i = 0; i < N; i++ ) {
		rows[ i ] = VecN( N );
	}
}

inline const MatN & MatN::operator = ( const MatN & rhs ) {
	numDimensions = rhs.numDimensions;
	rows = new VecN[ numDimensions ];
	for ( int i = 0; i < numDimensions; i++ ) {
		rows[ i ] = rhs.rows[ i ];
	}
	return *this;
}

inline const MatN & MatN::operator = ( const MatMN & rhs ) {
	if ( rhs.M != rhs.N ) {
		return *this;
	}

	numDimensions = rhs.N;
	rows = new VecN[ numDimensions ];
	for ( int i = 0; i < numDimensions; i++ ) {
		rows[ i ] = rhs.rows[ i ];
	}
	return *this;
}

inline void MatN::Zero() {
	for ( int i = 0; i < numDimensions; i++ ) {
		rows[ i ].Zero();
	}
}

inline void MatN::Identity() {
	for ( int i = 0; i < numDimensions; i++ ) {
		rows[ i ].Zero();
		rows[ i ][ i ] = 1.0f;
	}
}

inline void MatN::Transpose() {
	MatN tmp( numDimensions );

	for ( int i = 0; i < numDimensions; i++ ) {
		for ( int j = 0; j < numDimensions; j++ ) {
			tmp.rows[ i ][ j ] = rows[ j ][ i ];
		}
	}

	*this = tmp;
}

inline void MatN::operator *= ( float rhs ) {
	for ( int i = 0; i < numDimensions; i++ ) {
		rows[ i ] *= rhs;
	}
}

inline VecN MatN::operator * ( const VecN & rhs ) {
	VecN tmp( numDimensions );

	for ( int i = 0; i < numDimensions; i++ ) {
		tmp[ i ] = rows[ i ].Dot( rhs );
	}

	return tmp;
}

inline MatN MatN::operator * ( const MatN & rhs ) {
	MatN tmp( numDimensions );
	tmp.Zero();

	for ( int i = 0; i < numDimensions; i++ ) {
		for ( int j = 0; j < numDimensions; j++ ) {
			tmp.rows[ i ][ j ] += rows[ i ][ j ] * rhs.rows[ j ][ i ];
		}
	}

	return tmp;
}