//
//  gjk.cpp
//
#include <string>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <algorithm>
#include "Vector.h"
#include <vector>
#include "gjk.h"
#include "plane.h"

/*
====================================================
Support
The support function returns the point in the minkowski
"difference" that is furthest in a particular direction.
In this case, our minkowski sum is A - B
====================================================
*/
point_t Support( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, Vec3d dir ) {
	// Find the point in A furthest in direction
	point_t point;

	Vec3d maxA = ptsA[ 0 ];
	float maxDist = dir.DotProduct( maxA );
	for ( int i = 0; i < numA; i++ ) {
		float dist = dir.DotProduct( ptsA[ i ] );

		if ( dist > maxDist ) {
			maxDist = dist;
			maxA = ptsA[ i ];
			point.idxA = i;
		}
	}

	dir *= -1.0f;

	// Find the point in B furthest in the opposite direction
	Vec3d maxB = ptsB[ 0 ];
	maxDist = dir.DotProduct( maxB );
	for ( int i = 0; i < numB; i++ ) {
		float dist = dir.DotProduct( ptsB[ i ] );

		if ( dist > maxDist ) {
			maxDist = dist;
			maxB = ptsB[ i ];
			point.idxB = i;
		}
	}

	// Return the point, in the minkowski sum, furthest in the direction
	point.xyz = maxA - maxB;
	return point;
}

/*
====================================================
Simplex1
====================================================
*/
bool Simplex1( const Vec3d & ptB, const Vec3d & ptA, Vec3d & newDir ) {
	Vec3d dirAB = ptB - ptA;

	const Vec3d dirAO = Vec3d( 0 ) - ptA;

	// If the origin is "behind" A, then the new search direction is A->origin
	if ( dirAO.DotProduct( dirAB ) < 0.0f ) {
		newDir = dirAO;
		return false;
	}

	// We don't have to worry about the region "behind" B because the algorithm would've failed already.
	// So instead, we have something that's between [A,B]... so find the direction to the origin that is
	// also perpendicular to the line between [A,B].
	Vec3d tmp = dirAO.Cross( dirAB );
	newDir = dirAB.Cross( tmp );

	if ( newDir.GetLengthSqr() < 0.0001f ) {
		// It's co-linear, which means it intersects
		return true;
	}

	return false;
}

/*
====================================================
Simplex2

 B
|| \\
||   \\   1          *
||     \\          *
||       \\      *
||         \\  *
||  2/3      A		5
||         //  *
||       //      *
||     //          * 
||   //   4          *
|| //
 C

 We need to check regions 1,2,3,4,5 for the direction to the origin
====================================================
*/
bool Simplex2( const Vec3d & ptC, const Vec3d & ptB, const Vec3d & ptA, Vec3d & newDir ) {
	const Vec3d dirAO = Vec3d( 0 ) - ptA;
	const Vec3d dirAB = ptB - ptA;
	const Vec3d dirAC = ptC - ptA;

	const Vec3d normalABC = dirAB.Cross( dirAC );		// This is the normal of the triangle ABC
	const Vec3d normalAB = dirAB.Cross( normalABC );	// This is the normal of the line between A and B
	if ( dirAO.DotProduct( normalAB ) > 0.0f ) {
		// The origin could be in region 1 or 5

		if ( dirAO.DotProduct( dirAB ) > 0.0f ) {
			// origin is in region 1
			Vec3d tmp = dirAO.Cross( dirAB );
			newDir = dirAB.Cross( tmp );
			return false;
		}

		// Otherwise we are in region 5
		newDir = dirAO;
		return false;
	}

	const Vec3d normalAC = normalABC.Cross( dirAC );	// This is the normal of the line between A and C
	if ( dirAO.DotProduct( normalAC ) > 0.0f ) {
		// The origin could be in region 4 or 5

		if ( dirAO.DotProduct( dirAC ) > 0.0f ) {
			// origin is in region 4
			Vec3d tmp = dirAO.Cross( dirAC );
			newDir = dirAC.Cross( tmp );
			return false;
		}

		// Otherwise we are in region 5
		newDir = dirAO;
		return false;
	}

	// Now the origin can only be in region two or three
	if ( dirAO.DotProduct( normalABC ) > 0.0f ) {
		// The origin is on the front side of the triangle
		newDir = normalABC;
		return false;
	}

	// The origin is on the back side of the triangle
	newDir = normalABC * -1.0f;
	return false;
}

/*
====================================================
Simplex3
====================================================
*/
bool Simplex3( point_t * pts, Vec3d & newDir ) {
	const point_t & ptD = pts[ 0 ];
	const point_t & ptC = pts[ 1 ];
	const point_t & ptB = pts[ 2 ];
	const point_t & ptA = pts[ 3 ];

	const Vec3d dirAO = Vec3d( 0 ) - ptA.xyz;
	const Vec3d dirAB = ptB.xyz - ptA.xyz;
	const Vec3d dirAC = ptC.xyz - ptA.xyz;
	const Vec3d dirAD = ptD.xyz - ptA.xyz;

	Vec3d normalABC = dirAB.Cross( dirAC );
	if ( normalABC.DotProduct( dirAD ) > 0.0f ) {
		// make sure the normal faces away from ptD
		normalABC *= -1.0f;
	}

	Vec3d normalACD = dirAC.Cross( dirAD );
	if ( normalACD.DotProduct( dirAB ) > 0.0f ) {
		// make sure the normal faces away from ptB
		normalACD *= -1.0f;
	}

	Vec3d normalADB = dirAD.Cross( dirAB );
	if ( normalADB.DotProduct( dirAC ) > 0.0f ) {
		// make sure the normal faces away from ptC
		normalADB *= -1.0f;
	}

	// I don't think it can exist past A... I think it can only be inside it,
	// or outside one of the triangles containing A.  Actually, it probably
	// can exist in the region "above" A.
	if ( normalABC.DotProduct( dirAO ) > 0.0f ) {
		Simplex2( ptC.xyz, ptB.xyz, ptA.xyz, newDir );
		// put A in D, we can throw away D
		pts[ 0 ] = ptA;
		return false;
	}

	if ( normalACD.DotProduct( dirAO ) > 0.0f ) {
		Simplex2( ptD.xyz, ptC.xyz, ptA.xyz, newDir );
		// put A in B, we can throw away B
		pts[ 2 ] = ptA;
		return false;
	}

	if ( normalADB.DotProduct( dirAO ) > 0.0f ) {
		Simplex2( ptB.xyz, ptD.xyz, ptA.xyz, newDir );
		// put A in C, we can throw away C
		pts[ 1 ] = ptA;
		return false;
	}
	
	// Guess what!  The origin is inside the simplex!
	return true;
}

/*
====================================================
Simplex
====================================================
*/
bool Simplex( point_t * pts, const int num, Vec3d & newDir ) {
	switch ( num ) {
		default:
		case 2: {
			return Simplex1( pts[ 0 ].xyz, pts[ 1 ].xyz, newDir );
		} break;
		case 3: {
			return Simplex2( pts[ 0 ].xyz, pts[ 1 ].xyz, pts[ 2 ].xyz, newDir );
		} break;
		case 4: {
			return Simplex3( pts, newDir );
		} break;
	};
	
	return false;
}

/*
====================================================
GJK
Gilbert-Johnson-Keerthi
====================================================
*/
bool GJK( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, point_t * simplexPoints ) {
	const Vec3d origin( 0 );

	int numPts = 1;
	simplexPoints[ 0 ] = Support( ptsA, numA, ptsB, numB, Vec3d( 1, 1, 1 ) );

	Vec3d newDir = simplexPoints[ 0 ].xyz * -1.0f;
	do {
		if ( numPts > 3 ) {
			numPts = 3;
		}

		// Get the new point to check on
		point_t newPt = Support( ptsA, numA, ptsB, numB, newDir );

		// If this new point hasn't moved passed the origin, then the origin cannot be in the set.
		// And therefore there is no collision.
		if ( newDir.DotProduct( newPt.xyz - origin ) < 0.0f ) {
			return false;
		}

		simplexPoints[ numPts ] = newPt;
		numPts++;
		printf( "Num points: %i\n", numPts );
	} while ( !Simplex( simplexPoints, numPts, newDir ) );

	return true;
}
