//
//  main.cpp
//
#include <string>
#include <stdio.h>
#include <math.h>
#include <malloc.h>

/*
====================================================
complex
====================================================
*/
struct complex {
	float r, i;

	complex() : r( 0 ), i( 0 ) {}

	complex( float _r, float _i ) {
		r = _r;
		i = _i;
	}

	complex operator * ( complex rhs ) {
		complex c;
		c.r = r * rhs.r - i * rhs.i;
		c.i = r * rhs.i + i * rhs.r;
		return c;
	}
	const complex & operator *= ( complex rhs ) {
		complex c = *this;
		r = c.r * rhs.r - c.i * rhs.i;
		i = c.r * rhs.i + c.i * rhs.r;
		return *this;
	}
	const complex & operator += ( complex rhs ) {
		r += rhs.r;
		i += rhs.i;
		return *this;
	}
};

/*
====================================================
DFT
====================================================
*/
void DFT( complex * data, const int N ) {
	complex * tmp = (complex * )alloca( N * sizeof( complex ) );

	for ( int i = 0; i < N; i++ ) {
		tmp[ i ].r = 0;
		tmp[ i ].i = 0;
	}

	const float pi = acos( -1.0f );

	for ( int k = 0; k < N; k++ ) {    
		for ( int n = 0; n < N; n++ ) {
			complex w;
			w.r = cos( -2 * pi * n * k / N );
			w.i = sin( -2 * pi * n * k / N );

			tmp[ k ] += data[ n ] * w;
		}
	}

	for ( int i = 0; i < N; i++ ) {
		data[ i ] = tmp[ i ];
	}
}

/*
====================================================
Transpose
====================================================
*/
void Transpose( complex * data, const int N1, const int N2 ) {
	const int N = N1 * N2;
	complex * tmp = (complex *)alloca( N * sizeof( complex ) );

	for ( int i = 0; i < N; i++ ) {
		tmp[ i ] = data[ i ];
	}

	for ( int n1 = 0; n1 < N1; n1++ ) {
		for ( int n2 = 0; n2 < N2; n2++ ) {
			data[ n2 + N2 * n1 ] = tmp[ n1 + N1 * n2 ];
		}
	}
}

/*
====================================================
FFT
====================================================
*/
void FFT( complex * data, const int N ) {
	if ( ( N & ( N - 1 ) != 0 ) ) {
		return DFT( data, N );
	}

	const int N2 = N / 2;
	const int N1 = 2;
	if ( N2 == 1 ) {
		return DFT( data, N );
	}

	// Transpose the data so we can perform FFT's over the "columns"
	Transpose( data, N1, N2 );

	for ( int n1 = 0; n1 < N1; n1++ ) {
		FFT( data + n1 * N2, N2 );
	}

	// Transpose the data back to the original order
	Transpose( data, N2, N1 );

	// twiddles
	const float pi = acos( -1.0f );
	for ( int col = 1; col < N1; col++ ) {
		float angle = -2.0f * pi * float( col ) / float( N );

		for ( int row = 1; row < N2; row++ ) {
			int idx = row * N1 + col;
			
			complex w;
			w.r = cos( float( row ) * angle );
			w.i = sin( float( row ) * angle );

			data[ idx ] = data[ idx ] * w;
		}
	}

	// Perform FFT's over the "rows"
	for ( int n2 = 0; n2 < N2; n2++ ) {
		FFT( data + n2 * N1, N1 );
	}

	// Transpose the data into the final order
	Transpose( data, N1, N2 );
}

/*
====================================================
PrintData
====================================================
*/
void PrintData( complex * data, const int N, const int r ) {
	if ( r != 0 ) {
		return;
	}

	printf( "{" );
	for ( int i = 0; i < N; i++ ) {
		printf( " ( %.2f, %.2f )", data[ i ].r, data[ i ].i );
	}
	printf( " }\n" );
}

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
	const int N = 32;
	complex dataFFT[ N ];
	complex dataDFT[ N ];
	for ( int i = 0; i < N; i++ ) {
		dataFFT[ i ].r = i;
		dataFFT[ i ].i = 0;

		dataDFT[ i ].r = i;
		dataDFT[ i ].i = 0;
	}

	FFT( dataFFT, N );	
	DFT( dataDFT, N );

	printf( "\n------------\n" );
	printf( "FFT:\n" );
	PrintData( dataFFT, N, 0 );
	printf( "\n------------\n" );
	printf( "DFT:\n" );
	PrintData( dataDFT, N, 0 );

	return 0;
}
