/*
 *  Perlin.cpp
 *
 */

#include "Random.h"
#include "Perlin.h"

float * Perlin::s_randomFloat = NULL;
int * Perlin::s_permX = NULL;
int * Perlin::s_permY = NULL;
int * Perlin::s_permZ = NULL;

/*
====================================================
Perlin::Initialize
====================================================
*/
void Perlin::Initialize( Random & rnd ) {
	s_randomFloat = PerlinGenerate( rnd );
	s_permX = PerlinGeneratePerm( rnd );
	s_permY = PerlinGeneratePerm( rnd );
	s_permZ = PerlinGeneratePerm( rnd );
}

/*
====================================================
Perlin::Noise
====================================================
*/
float Perlin::Noise( const Vec3d & p ) {
	float u = p.x - floor( p.x );
	float v = p.y - floor( p.y );
	float w = p.z - floor( p.z );
	int i = int( 4.0f * p.x ) & 255;
	int j = int( 4.0f * p.y ) & 255;
	int k = int( 4.0f * p.z ) & 255;
	return s_randomFloat[ s_permX[ i ] ^ s_permY[ j ] ^ s_permZ[ k ] ];
}

/*
====================================================
Perlin::PerlinGenerate
====================================================
*/
float * Perlin::PerlinGenerate( Random & rnd ) {
	float * p = new float[ 256 ];
	for ( int i = 0; i < 256; i++ ) {
		p[ i ] = rnd.Get();
	}
	return p;
}

/*
====================================================
Perlin::Permute
====================================================
*/
void Perlin::Permute( int * p, int n, Random & rnd ) {
	for ( int i = n - 1; i > 0; i-- ) {
		int target = int( rnd.Get() * ( i + 1 ) );
		int tmp = p[ i ];
		p[ i ] = p[ target ];
		p[ target ] = tmp;
	}
}

/*
====================================================
Perlin::PerlinGeneratePerm
====================================================
*/
int * Perlin::PerlinGeneratePerm( Random & rnd ) {
	int * p = new int[ 256 ];
	for ( int i = 0; i < 256; i++ ) {
		p[ i ] = i;
	}
	Permute( p, 256, rnd );
	return p;
}