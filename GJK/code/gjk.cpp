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

/*
========================================================================================================

plane_t

========================================================================================================
*/

struct plane_t {
	Vec3d m_pts[ 3 ];
	Vec3d m_normal;

	plane_t() {}
	plane_t( Vec3d a, Vec3d b, Vec3d c ) {
		m_pts[ 0 ] = a;
		m_pts[ 1 ] = b;
		m_pts[ 2 ] = c;

		m_normal = Normal();
	}

	void FlipNormal() {
		std::swap( m_pts[ 0 ], m_pts[ 1 ] );
		m_normal = Normal();
	}

	Vec3d Normal() const {
		const Vec3d & a = m_pts[ 0 ];
		const Vec3d & b = m_pts[ 1 ];
		const Vec3d & c = m_pts[ 2 ];

		const Vec3d ab = b - a;
		const Vec3d ac = c - a;

		Vec3d normal = ab.Cross( ac );
		normal.Normalize();
		return normal;
	}	

	float SignedDistanceToPlane( const Vec3d & point ) const {
		const Vec3d dirToPoint = point - m_pts[ 0 ];
		float distance = Normal().DotProduct( dirToPoint );
		return distance;
	}

	float UnsignedDistanceToPlane( const Vec3d & point ) const {
		return fabsf( SignedDistanceToPlane( point ) );
	}

	bool IsCoPlanar( const Vec3d & point, const float epsilon = 0.00001f ) const {
		const float dist = UnsignedDistanceToPlane( point );
		return ( dist < epsilon );
	}

	bool IsCoPlanarOrBehind( const Vec3d & point, const float epsilon = 0.00001f ) const {
		const float dist = SignedDistanceToPlane( point );
		return ( dist < epsilon );
	}

	Vec3d ProjectPointOntoPlane( const Vec3d & point ) const {
		const float distance = SignedDistanceToPlane( point );
		const Vec3d ptOnPlane = point - m_normal * distance;
		return ptOnPlane;
	}
};


/*
====================================================
Support
The support function returns the point in the minkowski
"difference" that is furthest in a particular direction.
In this case, our minkowski sum is A - B
====================================================
*/
Vec3d Support( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, Vec3d dir ) {
	// Find the point in A furthest in direction
	Vec3d maxA = ptsA[ 0 ];
	float maxDist = dir.DotProduct( maxA );
	for ( int i = 0; i < numA; i++ ) {
		float dist = dir.DotProduct( ptsA[ i ] );

		if ( dist > maxDist ) {
			maxDist = dist;
			maxA = ptsA[ i ];
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
		}
	}

	// Return the point, in the minkowski sum, furthest in the direction
	return ( maxA - maxB );
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

	// These two algorithms should be equivalent. TODO: compare for speed and accuracy.
#if 0
	// I bet this is slower because of the sqrt
	dirAB.Normalize();
	Vec3d projection = dirAB * dirAB.DotProduct( dirAO );
	newDir = dirAO - projection;
#else
	Vec3d tmp = dirAO.Cross( dirAB );
	newDir = dirAB.Cross( tmp );
#endif

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
bool Simplex3( Vec3d * pts, Vec3d & newDir ) {
	const Vec3d & ptD = pts[ 0 ];
	const Vec3d & ptC = pts[ 1 ];
	const Vec3d & ptB = pts[ 2 ];
	const Vec3d & ptA = pts[ 3 ];

	const Vec3d dirAO = Vec3d( 0 ) - ptA;
	const Vec3d dirAB = ptB - ptA;
	const Vec3d dirAC = ptC - ptA;
	const Vec3d dirAD = ptD - ptA;

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
		Simplex2( ptC, ptB, ptA, newDir );
		// put A in D, we can throw away D
		pts[ 0 ] = ptA;
		return false;
	}

	if ( normalACD.DotProduct( dirAO ) > 0.0f ) {
		Simplex2( ptD, ptC, ptA, newDir );
		// put A in B, we can throw away B
		pts[ 2 ] = ptA;
		return false;
	}

	if ( normalADB.DotProduct( dirAO ) > 0.0f ) {
		Simplex2( ptB, ptD, ptA, newDir );
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
bool Simplex( Vec3d * pts, const int num, Vec3d & newDir ) {
	switch ( num ) {
		default:
		case 2: {
			return Simplex1( pts[ 0 ], pts[ 1 ], newDir );
		} break;
		case 3: {
			return Simplex2( pts[ 0 ], pts[ 1 ], pts[ 2 ], newDir );
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
bool GJK( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, Vec3d * simplexPoints ) {
	const Vec3d origin( 0 );

	int numPts = 1;
	simplexPoints[ 0 ] = Support( ptsA, numA, ptsB, numB, Vec3d( 1, 1, 1 ) );

	Vec3d newDir = simplexPoints[ 0 ] * -1.0f;
	do {
		if ( numPts > 3 ) {
			numPts = 3;
		}

		// Get the new point to check on
		Vec3d newPt = Support( ptsA, numA, ptsB, numB, newDir );

		// If this new point hasn't moved passed the origin, then the origin cannot be in the set.
		// And therefore there is no collision.
		if ( newDir.DotProduct( newPt - origin ) < 0.0f ) {
			return false;
		}

		simplexPoints[ numPts ] = newPt;
		numPts++;
		printf( "Num points: %i\n", numPts );
	} while ( !Simplex( simplexPoints, numPts, newDir ) );

	return true;
}
