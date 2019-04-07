//
//  main.cpp
//
#include <string>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <algorithm>
#include "Vector.h"
#include "gjk.h"
#include "epa.h"

/*
====================================================
Orient
====================================================
*/
Vec3d Orient( const Vec3d & fwd, const Vec3d & left, const Vec3d & up, const Vec3d & pt ) {
	Vec3d rotated;
	rotated.x = fwd.DotProduct( pt );
	rotated.y = left.DotProduct( pt );
	rotated.z = up.DotProduct( pt );
	return rotated;
}

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
	Vec3d ptsA[ 8 ];
	Vec3d ptsB[ 8 ];

	ptsA[ 0 ] = Vec3d( -1, -1, -1 );
	ptsA[ 1 ] = Vec3d( -1,  1, -1 );
	ptsA[ 2 ] = Vec3d(  1, -1, -1 );
	ptsA[ 3 ] = Vec3d(  1,  1, -1 );
	
	ptsA[ 4 ] = Vec3d( -1, -1,  1 );
	ptsA[ 5 ] = Vec3d( -1,  1,  1 );
	ptsA[ 6 ] = Vec3d(  1, -1,  1 );
	ptsA[ 7 ] = Vec3d(  1,  1,  1 );

	Vec3d fwd = Vec3d( 1, 1, 1 );
	fwd.Normalize();
	Vec3d up = Vec3d( 0, 0, 1 );
	Vec3d left = up.Cross( fwd );
	left.Normalize();
	up = fwd.Cross( left );
	up.Normalize();

//	Vec3d offset = Vec3d( 1, 1, 1 );	// ends up colinear/coplanar with our simplexes, so we need special checks for that
//	Vec3d offset = Vec3d( 1.0f, 1.5f, 1.5f );
	Vec3d offset = Vec3d( 0, 0, 2 );
	for ( int i = 0; i < 8; i++ ) {
		ptsB[ i ] = Orient( fwd, left, up, ptsA[ i ] );
		ptsB[ i ] += offset;
	}

	point_t simplex[ 4 ];
	bool doesIntersect = GJK( ptsA, 8, ptsB, 8, simplex );
	if ( doesIntersect ) {
		printf( "Intersects!!\n" );
		float distance = EPA( ptsA, 8, ptsB, 8, simplex );
		printf( "Closest distance = %f\n", distance );
	} else {
		printf( "No intersects!!\n" );
	}

	return 0;
}
