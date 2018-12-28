//
//  main.cpp
//
#include <string>
#include <stdio.h>
#include <math.h>

/*
========================================================================================================

Calculates the spherical harmonic components from the paper

"An Efficient Representation for Irradiance Environment Maps" by Ravi Ramamoorthi & Pat Hanrahan.

========================================================================================================
*/

struct texel_t {
	texel_t() : r( 0 ), g( 0 ), b( 0 ) {}

	float & operator[]( const int i ) { return ( ( ( float * ) this )[ i ] ); }

	float r;
	float g;
	float b;
};

struct legendrePolynomials_t {
	texel_t	mL00;
	texel_t	mL11;
	texel_t	mL10;
	texel_t	mL1n1;
	texel_t	mL22;
	texel_t	mL21;
	texel_t	mL20;
	texel_t	mL2n1;
	texel_t	mL2n2;
};

void PrintTexel( const texel_t & texel ) {
	printf( "{ %f %f %f }\n", texel.r , texel.g, texel.b );
}
void PrintLegendrePolynomials( const legendrePolynomials_t & legendrePolynomials ) {
	printf( "L00 = " );
	PrintTexel( legendrePolynomials.mL00 );
	printf( "L11 = " );
	PrintTexel( legendrePolynomials.mL11 );
	printf( "L10 = " );
	PrintTexel( legendrePolynomials.mL10 );
	printf( "L1n1 = " );
	PrintTexel( legendrePolynomials.mL1n1 );
	printf( "L22 = " );
	PrintTexel( legendrePolynomials.mL22 );
	printf( "L21 = " );
	PrintTexel( legendrePolynomials.mL21 );
	printf( "L20 = " );
	PrintTexel( legendrePolynomials.mL20 );
	printf( "L2n1 = " );
	PrintTexel( legendrePolynomials.mL2n1 );
	printf( "L2n2 = " );
	PrintTexel( legendrePolynomials.mL2n2 );
}

/*
====================================================
GnomonicProjectedArea
Returns the gnomonic projected area of the section
of the plane in the range [0,x] u [0,y]
====================================================
*/
float GnomonicProjectedArea( const float x, const float y ) {
	const float numerator = x * y;
	const float denominator = sqrtf( x * x + y * y + 1.0f );
	const float area = atanf( numerator / denominator );
	return area;
}

/*
====================================================
AreaElement
====================================================
*/
float AreaElement( float x, float y, float dx, float dy ) {
	x = fabs( x );
	y = fabs( y );
	dx = fabs( dx );
	dy = fabs( dy );

	const float Area0 = GnomonicProjectedArea( x, y );
	const float Area1 = GnomonicProjectedArea( x, y + dx );
	const float Area2 = GnomonicProjectedArea( x + dx, y );
	const float Area3 = GnomonicProjectedArea( x + dx, y + dx );

	const float strip1 = Area1 - Area0;
	const float strip2 = Area2 - Area0;

	const float dA = Area3 - Area0 - strip1 - strip2;
	return dA;
}

/*
====================================================
NormalizeVector
====================================================
*/
void NormalizeVector( float * dir ) {
	float lengthSqr = dir[ 0 ] * dir[ 0 ] + dir[ 1 ] * dir[ 1 ] + dir[ 2 ] * dir[ 2 ];
	dir[ 0 ] = dir[ 0 ] / sqrtf( lengthSqr );
	dir[ 1 ] = dir[ 1 ] / sqrtf( lengthSqr );
	dir[ 2 ] = dir[ 2 ] / sqrtf( lengthSqr );
}

/*
====================================================
RotateDir
====================================================
*/
void RotateDir( const float * matOrient, float * dir ) {
	float dirIn[ 3 ];
	dirIn[ 0 ] = dir[ 0 ];
	dirIn[ 1 ] = dir[ 1 ];
	dirIn[ 2 ] = dir[ 2 ];

	dir[ 0 ] = matOrient[ 0 ] * dirIn[ 0 ] + matOrient[ 1 ] * dirIn[ 1 ] + matOrient[ 2 ] * dirIn[ 2 ];
	dir[ 1 ] = matOrient[ 3 ] * dirIn[ 0 ] + matOrient[ 4 ] * dirIn[ 1 ] + matOrient[ 5 ] * dirIn[ 2 ];
	dir[ 2 ] = matOrient[ 6 ] * dirIn[ 0 ] + matOrient[ 7 ] * dirIn[ 1 ] + matOrient[ 8 ] * dirIn[ 2 ];
}

/*
====================================================
LegendrePolynomialsForImage
Assumes square image
====================================================
*/
void LegendrePolynomialsForImage( const texel_t * rgb, const int imageWidth, const float * matrixOrient, legendrePolynomials_t & legendrePolynomials ) {
	for ( int j = 0; j < imageWidth; j++ ) {
		for ( int i = 0; i < imageWidth; i++ ) {		
			const int idx = i + imageWidth * j;

			const float r = rgb[ idx ].r;
			const float g = rgb[ idx ].g;
			const float b = rgb[ idx ].b;

			const float ds = 1.0f / float( imageWidth );
			const float dt = 1.0f / float( imageWidth );

			const float s = float( i - imageWidth / 2 ) / float( imageWidth / 2 ); // [-1, 1]
			const float t = float( j - imageWidth / 2 ) / float( imageWidth / 2 ); // [-1, 1]

			const float dA = AreaElement( s, t, ds, dt );

			float dir[ 3 ];
			dir[ 0 ] = 1.0f;
			dir[ 1 ] = s;
			dir[ 2 ] = t;

			NormalizeVector( dir );
			RotateDir( matrixOrient, dir );

			const float x2 = dir[ 0 ] * dir[ 0 ];
			const float y2 = dir[ 1 ] * dir[ 1 ];
			const float z2 = dir[ 2 ] * dir[ 2 ];

			const float xz = dir[ 0 ] * dir[ 2 ];
			const float xy = dir[ 0 ] * dir[ 1 ];
			const float yz = dir[ 1 ] * dir[ 2 ];

			const float azimuth = sqrtf( 1.0f - dir[ 2 ] * dir[ 2 ] );

			texel_t normalizedColorAzimuth;
			normalizedColorAzimuth.r = r * azimuth * dA;
			normalizedColorAzimuth.g = g * azimuth * dA;
			normalizedColorAzimuth.b = b * azimuth * dA;

			for ( int i = 0; i < 3; ++i ) {
				legendrePolynomials.mL00[ i ] +=	0.282095f * normalizedColorAzimuth[ i ];
				legendrePolynomials.mL11[ i ] +=	0.488603f * dir[ 0 ] * normalizedColorAzimuth[ i ];
				legendrePolynomials.mL10[ i ] +=	0.488603f * dir[ 2 ] * normalizedColorAzimuth[ i ];
				legendrePolynomials.mL1n1[ i ] +=	0.488603f * dir[ 1 ] * normalizedColorAzimuth[ i ];
				legendrePolynomials.mL22[ i ] +=	0.546274f * ( x2 - y2 ) * normalizedColorAzimuth[ i ];
				legendrePolynomials.mL21[ i ] +=	1.092548f * xz * normalizedColorAzimuth[ i ];
				legendrePolynomials.mL20[ i ] +=	0.315392f * ( 3.0f * z2 - 1.0f ) * normalizedColorAzimuth[ i ];
				legendrePolynomials.mL2n1[ i ] +=	1.092548f * yz * normalizedColorAzimuth[ i ];
				legendrePolynomials.mL2n2[ i ] +=	1.092548f * xy * normalizedColorAzimuth[ i ];
			}			
		}
	}
}

/*
====================================================
SetMatrix
====================================================
*/
void SetMatrix( float * matrix, float fwdX, float fwdY, float fwdZ, float leftX, float leftY, float leftZ, float upX, float upY, float upZ ) {
	matrix[ 0 ] = fwdX;
	matrix[ 1 ] = fwdY;
	matrix[ 2 ] = fwdZ;

	matrix[ 3 ] = leftX;
	matrix[ 4 ] = leftY;
	matrix[ 5 ] = leftZ;

	matrix[ 6 ] = upX;
	matrix[ 7 ] = upY;
	matrix[ 8 ] = upZ;
}

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
	legendrePolynomials_t legendrePolynomials;

	// Generate texel data
	const int numImages = 6;
	const int imageWidth = 64;
	texel_t cubeImages[ numImages ][ imageWidth * imageWidth ];

	// Cube image layout ( px, nx, py, ny, pz, nz )
	for ( int y = 0; y < imageWidth; y++ ) {
		for ( int x = 0; x < imageWidth; x++ ) {
			const int idx = x + imageWidth * y;

			cubeImages[ 0 ][ idx ].r = 1;
			cubeImages[ 0 ][ idx ].g = 0;
			cubeImages[ 0 ][ idx ].b = 0;

			cubeImages[ 1 ][ idx ].r = 0;
			cubeImages[ 1 ][ idx ].g = 0;
			cubeImages[ 1 ][ idx ].b = 0;

			cubeImages[ 2 ][ idx ].r = 0;
			cubeImages[ 2 ][ idx ].g = 1;
			cubeImages[ 2 ][ idx ].b = 0;

			cubeImages[ 3 ][ idx ].r = 0;
			cubeImages[ 3 ][ idx ].g = 0;
			cubeImages[ 3 ][ idx ].b = 1;

			cubeImages[ 4 ][ idx ].r = 1;
			cubeImages[ 4 ][ idx ].g = 1;
			cubeImages[ 4 ][ idx ].b = 1;

			cubeImages[ 5 ][ idx ].r = 1;
			cubeImages[ 5 ][ idx ].g = 1;
			cubeImages[ 5 ][ idx ].b = 0;
		}
	}

	float matrices[ 6 ][ 9 ];
	SetMatrix( matrices[ 0 ], 1, 0, 0,  0, 1, 0,  0, 0, 1 );
	SetMatrix( matrices[ 1 ],-1, 0, 0,  0,-1, 0,  0, 0, 1 );

	SetMatrix( matrices[ 2 ], 0, 1, 0, -1, 0, 0,  0, 0, 1 );
	SetMatrix( matrices[ 3 ], 0,-1, 0,  1, 0, 0,  0, 0, 1 );

	SetMatrix( matrices[ 4 ], 0, 0, 1,  0, 1, 0, -1, 0, 0 );
	SetMatrix( matrices[ 5 ], 0, 0,-1,  0, 1, 0,  1, 0, 0 );

	for ( int i = 0; i < 6; i++ ) {
		LegendrePolynomialsForImage( cubeImages[ i ], imageWidth, matrices[ i ], legendrePolynomials );
	}

	PrintLegendrePolynomials( legendrePolynomials );
	return 0;
}
