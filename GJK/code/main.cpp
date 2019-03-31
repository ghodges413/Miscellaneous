//
//  main.cpp
//
#include <string>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <algorithm>
#include "Vector.h"
#include <vector>

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
MinkowskiSum
====================================================
*/
int MinkowskiSum( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, Vec3d ** output ) {
	int numSum = numA * numB;
	*output = new Vec3d[ numSum ];
	for ( int i = 0; i < numA; i++ ) {
		const Vec3d a = ptsA[ i ];

		for ( int j = 0; j < numB; j++ ) {
			const Vec3d b = ptsB[ j ];

			(*output)[ i * numA + j ] = a + b;
		}
	}

	return numSum;
}

/*
====================================================
MinkowskiDifference
====================================================
*/
int MinkowskiDifference( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, Vec3d ** output ) {
	int numSum = numA * numB;
	*output = new Vec3d[ numSum ];
	for ( int i = 0; i < numA; i++ ) {
		const Vec3d a = ptsA[ i ];

		for ( int j = 0; j < numB; j++ ) {
			const Vec3d b = ptsB[ j ];

			(*output)[ i * numA + j ] = a - b;
		}
	}

	return numSum;
}

/*
====================================================
FindFurthestPt
====================================================
*/
int FindFurthestPt( const Vec3d & start, const Vec3d & dir, const Vec3d * pts, const int numPoints, const int ignoreIdx ) {
	int index = 0;
	float maxDistSqr = dir.DotProduct( pts[ 0 ] - start );

	for ( int i = 0; i < numPoints; i++ ) {
		if ( ignoreIdx == i ) {
			continue;
		}

		float distSqr = dir.DotProduct( pts[ i ] - start );
		if ( distSqr > maxDistSqr ) {
			index = i;
		}
	}

	return index;
}
int FindFurthestPt( const Vec3d & dir, const Vec3d * pts, const int numPoints, const int ignoreIdx ) {
	int index = 0;
	float maxDistSqr = dir.DotProduct( pts[ 0 ] );

	for ( int i = 0; i < numPoints; i++ ) {
		if ( ignoreIdx == i ) {
			continue;
		}

		float distSqr = dir.DotProduct( pts[ i ] );
		if ( distSqr > maxDistSqr ) {
			index = i;
		}
	}

	return index;
}

/*
====================================================
PlaneFromPoints
====================================================
*/
bool PlaneFromPoints( const Vec3d & ptA, const Vec3d & ptB, const Vec3d & ptC, Vec3d & planePt, Vec3d & planeNormal ) {
	// Make sure these points aren't co-linear
	Vec3d ab = ptB - ptA;
	Vec3d ac = ptC - ptA;
	ab.Normalize();
	ac.Normalize();

	planeNormal = ab.Cross( ac );
	float area = planeNormal.GetLengthSqr();
	if ( area < 0.00001f ) {
		// It's very co-linear... pick a different third point
		printf( "Warning co-linear!  Learn to deal!" );
		return false;
	}

	planeNormal.Normalize();

	// Find the point on the plane closest to the origin
	const Vec3d origin = Vec3d( 0 );
	Vec3d aorigin = origin - ptA;
	float dist = planeNormal.DotProduct( aorigin );
	planePt = origin + planeNormal * dist;
// 	planeNormal = planePt * -1.0f;
// 	planeNormal.Normalize();
	return false;
}

/*
====================================================
DoesContainOrigin
====================================================
*/
bool DoesContainOrigin( const Vec3d * pts, const int numPoints ) {
	// Choose a random point in the set and start there
	Vec3d ptA = pts[ 0 ];
	Vec3d dir = ptA * -1.0f;	// point the direction towards the origin
	dir.Normalize();	// This isn't necessary

	Vec3d ptB;
	for ( int i = 1; i < numPoints; i++ ) {
		ptB = pts[ i ];
		if ( ( ptB - ptA ).GetLengthSqr() > 0.001f ) {
			break;
		}
	}

	int idx = FindFurthestPt( ptA, dir, pts, numPoints, 1 );
	Vec3d ptC = pts[ idx ];

	// Make sure these points aren't co-linear
	Vec3d ab = ptB - ptA;
	Vec3d ac = ptC - ptA;
	ab.Normalize();
	ac.Normalize();

	float area = ab.Cross( ac ).GetLengthSqr();
	if ( area < 0.00001f ) {
		// It's very co-linear... pick a different third point
		printf( "Warning co-linear!  Learn to deal!" );
		assert( 0 );
		return false;
	}

	Vec3d planePt;
	Vec3d planeNormal;
	bool success = PlaneFromPoints( ptA, ptB, ptC, planePt, planeNormal );
	if ( !success ) {
		printf( "we are failures!\n" );
		assert( 0 );
		return false;
	}

	// Find the next furthest pt and we should have a tetrahedron
	idx = FindFurthestPt( planePt, planeNormal, pts, numPoints, 0 );


	return false;
}

/*
====================================================
ClosestPointOnLine
====================================================
*/
Vec3d ClosestPointOnLine( const Vec3d & ptA, const Vec3d & ptB, const Vec3d & pt ) {
	Vec3d rayToPt = pt - ptA;
	Vec3d dir = ptB - ptA;
	dir.Normalize();

	// Project the rayToPt onto the dir
	Vec3d ptC = ptA + dir * dir.DotProduct( rayToPt );
	return ptC;
}


void Tetrahedron( const Vec3d & ptA, const Vec3d & ptB, const Vec3d & ptC, const Vec3d & ptD, plane_t planes[ 4 ] ) {
	planes[ 0 ] = plane_t( ptA, ptB, ptC );
	planes[ 1 ] = plane_t( ptA, ptB, ptD );
	planes[ 2 ] = plane_t( ptA, ptD, ptC );
	planes[ 3 ] = plane_t( ptD, ptB, ptC );

	// If this plane points towards the other point... then flip the normal
	if ( planes[ 0 ].Normal().DotProduct( ptD - planes[ 0 ].m_pts[ 0 ] ) > 0.0f ) {
		planes[ 0 ].FlipNormal();
	}

	// If this plane points towards the other point... then flip the normal
	if ( planes[ 1 ].Normal().DotProduct( ptC - planes[ 1 ].m_pts[ 0 ] ) > 0.0f ) {
		planes[ 1 ].FlipNormal();
	}

	// If this plane points towards the other point... then flip the normal
	if ( planes[ 2 ].Normal().DotProduct( ptB - planes[ 2 ].m_pts[ 0 ] ) > 0.0f ) {
		planes[ 2 ].FlipNormal();
	}

	// If this plane points towards the other point... then flip the normal
	if ( planes[ 3 ].Normal().DotProduct( ptA - planes[ 3 ].m_pts[ 0 ] ) > 0.0f ) {
		planes[ 3 ].FlipNormal();
	}
}

bool IsPointInPolyhedra( const Vec3d & pt, const plane_t * planes, const int numPlanes ) {
	for ( int i = 0; i < numPlanes; i++ ) {
		if ( planes[ i ].Normal().DotProduct( pt - planes[ i ].m_pts[ 0 ] ) > 0.0f ) {
			return false;
		}
	}

	return true;
}

/*
====================================================
BuildPolyhedra
====================================================
*/
// bool BuildPolyhedra( const Vec3d * pts, const int numPoints ) {
// 	// As we build the polyhedra check if origin is contained inside
// 	int idxA = FindFurthestPt( pts[ 0 ] * -1.0f, pts, numPoints, -1 );
// 
// 	Vec3d ptA = pts[ idxA ];
// 
// 	int idxB = FindFurthestPt( ptA * -1.0f, pts, numPoints, idxA );
// 
// 	Vec3d ptB = pts[ idxB ];
// 
// 	Vec3d ptC = ClosestPointOnLine( ptA, ptB, Vec3d( 0 ) );
// 	int idxC = FindFurthestPt( ptC * -1.0f, pts, numPoints, idxA );
// 	if ( idxC == idxB ) {
// 		idxC = FindFurthestPt( ptC * 1.0f, pts, numPoints, idxA );
// 	}
// 
// 	ptC = pts[ idxC ];
// 
// 	Vec3d ptD = ClosestPointOnPlane( ptA, ptB, ptC, Vec3d( 0 ) );
// 	int idxD = FindFurthestPt( ptD * -1.0f, pts, numPoints, idxA );
// 	ptD = pts[ idxD ];
// 
// 	plane_t planes[ 4 ];
// 	Tetrahedron( ptA, ptB, ptC, ptD, planes );
// 	IsPointInPolyhedra( Vec3d( 0 ), planes, 4 );
// 
// 	// We now have our first simplex.  We should check if the origin is inside this polytope.
// 	// If it is, then we're done!  If it isn't... well, fuck my asshole raw.
// 
// 	return false;
// }

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
	//Vec3d simplexPoints[ 4 ];	// 4 is the maximum number of simplex points we need at any given moment
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
		numPts++;// = std::min( numSimplexPts + 1, 3 );	// max points are 4, and we always put the new point on the top of the stack
		printf( "Num points: %i\n", numPts );
	} while ( !Simplex( simplexPoints, numPts, newDir ) );

	return true;
}

/*
====================================================
EPA
ExpandingPolytopeAlgorithm

This is currently only built for intersecting polytopes
====================================================
*/
float EPA( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, const Vec3d * polytope0 ) {
	// Polytope 0 is the initial polytope
	// Obviously, the origin is inside this polytope.
	// Here's what we need to do:
	// 1. Find the closest side of the polytope.
	// 2. Expand in that direction.
	// 3. If expansion fails, we have found the closest face.
	// 4. Otherwise, repeat

	const int maxPlanes = 128;
	plane_t planes[ maxPlanes ];

	Tetrahedron( polytope0[ 0 ], polytope0[ 1 ], polytope0[ 2 ], polytope0[ 3 ], planes );
	int numPlanes = 4;

	// First let's convert the polytope into a set of planes.
	// We're going to require some extra data in these planes too.
	float minDist;
	do {
		printf( "---------------------------------\n" );
		int closestIdx = 0;
		minDist = planes[ 0 ].UnsignedDistanceToPlane( Vec3d( 0 ) );

		// Find closest plane
		for ( int i = 0; i < numPlanes; i++ ) {
			float dist = planes[ i ].UnsignedDistanceToPlane( Vec3d( 0 ) );
			printf( "Distances: %i %f   n( %f %f %f )\n", i, dist, planes[ i ].m_normal.x, planes[ i ].m_normal.y, planes[ i ].m_normal.z );
			if ( dist < minDist ) {
				minDist = dist;
				closestIdx = i;
			}
		}
		printf( "MinDist: %i %f\n", closestIdx, minDist );

		// Expand in the direction of this plane
		Vec3d newPt = Support( ptsA, numA, ptsB, numB, planes[ closestIdx ].Normal() );

		// Check if this point is in the plane
		const plane_t & closest = planes[ closestIdx ];
		const float newPtDist = closest.SignedDistanceToPlane( newPt );
		printf( "newPtDist = %f\n", newPtDist );
		for ( int i = 0; i < numPlanes; i++ ) {
			float dist = planes[ i ].SignedDistanceToPlane( newPt );
			printf( "newPtDistances = %f\n", dist );
		}
		if ( closest.IsCoPlanar( newPt ) ) {
			// We've found the closest distance
			break;
		}

		// Delete the closest plane and generate three new planes
		// If the original winding order is preserved, then these will be normalized to point out.
		plane_t newPlanes[ 3 ];
		newPlanes[ 0 ] = plane_t( newPt, closest.m_pts[ 0 ], closest.m_pts[ 1 ] );
		newPlanes[ 1 ] = plane_t( newPt, closest.m_pts[ 1 ], closest.m_pts[ 2 ] );
		newPlanes[ 2 ] = plane_t( newPt, closest.m_pts[ 2 ], closest.m_pts[ 0 ] );

		// Make sure the normals are pointing away from the origin (careful, we need to worry about situations where the origin is coplanar).
		// We should work to make sure we don't need to do this... because we should be able to do it by winding order
		for ( int i = 0; i < 3; i++ ) {
			plane_t & plane = newPlanes[ i ];
			if ( plane.Normal().DotProduct( plane.m_pts[ 0 ] ) < 0.0f ) {
				printf( "Warning this should never happen!\n" );
				plane.FlipNormal();
			}
		}

		// Remove the old plane and add the new ones
		planes[ closestIdx ] = newPlanes[ 0 ];
		planes[ numPlanes + 0 ] = newPlanes[ 1 ];
		planes[ numPlanes + 1 ] = newPlanes[ 2 ];
		numPlanes += 2;
	} while ( ( numPlanes + 2 ) < maxPlanes );

	return minDist;
}

struct face_t {

};

struct polytope_t {

};

void EPA2_r( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, plane_t * polytope, int & numPlanes, int planeIdx, const int maxPlanes ) {
	if ( numPlanes + 3 > maxPlanes ) {
		printf( "Warning!  Ran out of planes!\n" );
		return;
	}

	plane_t & plane = polytope[ planeIdx ];

	Vec3d newPt = Support( ptsA, numA, ptsB, numB, plane.m_normal );
	float newPtDist = plane.SignedDistanceToPlane( newPt );
	printf( "newPtDist: %f\n", newPtDist );
	if ( plane.IsCoPlanarOrBehind( newPt ) ) {
		// If the new point is coplanar, then we can't expand this face further
		printf( "Plane termination.  Plane is an edge on the convex set!\n" );
		return;
	}

	// In order to keep our shit properly convex, we have to remove all faces that face this new point.
	// That's where we're fucking up.  Because we aren't doing that check and handling it, we end up with a convex
	// mother fucker.  And then we just keep on expanding and expanding over and over and over.
	int faceIndices[ 32 ];	// doubt we need it this high
	int numFaces = 0;
	for ( int i = 0; i < numPlanes; i++ ) {
		float dist = polytope[ i ].SignedDistanceToPlane( newPt );
		if ( dist > 0 ) {
			faceIndices[ numFaces ] = i;
			numFaces++;
		}
	}

	if ( numFaces > 1 ) {
		// Now, we need to get the vertices of these faces
		Vec3d * faceVerts = (Vec3d *)alloca( 3 * numFaces * sizeof( Vec3d ) );
		for ( int i = 0; i < numFaces; i++ ) {
			int faceIdx = faceIndices[ i ];
			const plane_t & face = polytope[ faceIdx ];
			faceVerts[ i * 3 + 0 ] = face.m_pts[ 0 ];
			faceVerts[ i * 3 + 1 ] = face.m_pts[ 1 ];
			faceVerts[ i * 3 + 2 ] = face.m_pts[ 2 ];
		}

		struct edge_t {
			int verts[ 2 ];
			int count;	// number of edge shares
		};

		// Now that we have all those, we need to remove the redundant ones while preserving winding order
		struct triangle_t {
			int verts[ 3 ];
			int edges[ 3 ];
		};

		struct vertex_t {
			Vec3d xyz;
			int count;
		};

		std::vector< Vec3d > vertList;

		triangle_t * tris = (triangle_t *)alloca( numFaces * sizeof( triangle_t ) );

		vertex_t * collapsedVerts = (vertex_t *)alloca( 3 * numFaces * sizeof( vertex_t ) );
		int numCollapsed = 0;
		for ( int faceIdx = 0; faceIdx < numFaces; faceIdx++ ) {
			for ( int i = 0; i < 3; i++ ) {
				const Vec3d & vert = faceVerts[ faceIdx * 3 + i ];

				// Try to find a previously existing one
				int newVertIdx = 0;
				bool appendNew = true;
				for ( int j = 0; j < numCollapsed; j++ ) {
					if ( collapsedVerts[ j ].xyz == vert ) {
						collapsedVerts[ j ].count++;
						appendNew = false;
						newVertIdx = j;
						break;
					}
				}

				// If it wasn't found, append a new one
				if ( appendNew ) {
					newVertIdx = numCollapsed;
					collapsedVerts[ numCollapsed ].xyz = vert;
					collapsedVerts[ numCollapsed ].count = 1;
					numCollapsed++;
				}

				tris[ faceIdx ].verts[ i ] = newVertIdx;
			}
		}

		// Now find the lowest count vertex as the starting vertex
		int lowestIdx = 0;
		int lowestCount = collapsedVerts[ 0 ].count;
		for ( int i = 1; i < numCollapsed; i++ ) {
			if ( collapsedVerts[ i ].count < lowestCount ) {
				lowestCount = collapsedVerts[ i ].count;
				lowestIdx = i;
			}
		}

		// Okay, now we have the starting index;
		Vec3d * windingVerts = (Vec3d *)alloca( numCollapsed * sizeof( Vec3d ) );
		const int startWindingIdx = lowestIdx;
		int nextVertIdx = lowestIdx;
		int numWindingPoints = 1;
		for ( int i = 0; i < numCollapsed; i++ ) {
			const vertex_t & collapsedVert = collapsedVerts[ nextVertIdx ];
			windingVerts[ i ] = collapsedVert.xyz;

			int nextVerts[ 32 ];
			int numPotentialNexts = 0;

			const int currentIdx = nextVertIdx;
			for ( int faceIdx = 0; faceIdx < numFaces; faceIdx++ ) {
				const triangle_t & tri = tris[ faceIdx ];

				nextVertIdx = -1;
				for ( int v = 0; v < 3; v++ ) {
					if ( currentIdx == tri.verts[ v ] ) {
						int v2 = ( v + 1 ) % 3;
						nextVertIdx = tri.verts[ v2 ];
					}
				}

				if ( -1 == nextVertIdx ) {
					continue;
				}

				nextVerts[ numPotentialNexts ] = nextVertIdx;
				numPotentialNexts++;
			}

			assert( numPotentialNexts == collapsedVert.count );

			// Now that we have our potential nexts in the list, we need to find the next with lowest count number
			nextVertIdx = nextVerts[ 0 ];
			int lowestCount = collapsedVerts[ nextVerts[ 0 ] ].count;
			for ( int nextIdx = 0; nextIdx < numPotentialNexts; nextIdx++ ) {
				const int idx = nextVerts[ nextIdx ];
				const int count = collapsedVerts[ idx ].count;
				if ( count < lowestCount ) {
					lowestCount = count;
					nextVertIdx = idx;
				}
			}

			// If we've wrapped around on the winding, then we're done
			if ( startWindingIdx == nextVertIdx ) {
				break;
			}

			numWindingPoints++;
		}

		
		plane_t * newPlanes = (plane_t *)alloca( numWindingPoints * sizeof( plane_t ) );
		for ( int i = 0; i < numWindingPoints; i++ ) {
			int idx0 = ( i + 0 ) % numWindingPoints;
			int idx1 = ( i + 1 ) % numWindingPoints;
			newPlanes[ i ] = plane_t( newPt, windingVerts[ idx0 ], windingVerts[ idx1 ] );

			plane_t & plane = newPlanes[ i ];
			if ( plane.Normal().DotProduct( plane.m_pts[ 0 ] ) < 0.0f ) {
				printf( "Warning this should never happen!\n" );
				plane.FlipNormal();
			}
		}

		int * indices = (int *)alloca( numWindingPoints * sizeof( int ) );
		for ( int i = 0; i < numFaces; i++ ) {
			indices[ i ] = faceIndices[ i ];
		}
		{
			int appendIdx = 0;
			for ( int i = numFaces; i < numWindingPoints; i++ ) {
				indices[ i ] = numPlanes + appendIdx;
				appendIdx++;
			}
		}

		for ( int i = 0; i < numWindingPoints; i++ ) {
			int idx = indices[ i ];
			polytope[ idx ] = newPlanes[ i ];
		}
		numPlanes += numWindingPoints - numFaces;

		for ( int i = 0; i < numWindingPoints; i++ ) {
			EPA2_r( ptsA, numA, ptsB, numB, polytope, numPlanes, indices[ i ], maxPlanes );
		}
	} else {
		plane_t newPlanes[ 3 ];
		newPlanes[ 0 ] = plane_t( newPt, plane.m_pts[ 0 ], plane.m_pts[ 1 ] );
		newPlanes[ 1 ] = plane_t( newPt, plane.m_pts[ 1 ], plane.m_pts[ 2 ] );
		newPlanes[ 2 ] = plane_t( newPt, plane.m_pts[ 2 ], plane.m_pts[ 0 ] );

		for ( int i = 0; i < 3; i++ ) {
			plane_t & plane = newPlanes[ i ];
			if ( plane.Normal().DotProduct( plane.m_pts[ 0 ] ) < 0.0f ) {
				printf( "Warning this should never happen!\n" );
				plane.FlipNormal();
			}
		}

		int indices[ 3 ];
		indices[ 0 ] = planeIdx;
		indices[ 1 ] = numPlanes + 0;
		indices[ 2 ] = numPlanes + 1;

		polytope[ planeIdx ] = newPlanes[ 0 ];
		polytope[ numPlanes + 0 ] = newPlanes[ 1 ];
		polytope[ numPlanes + 1 ] = newPlanes[ 2 ];
		numPlanes += 2;

		EPA2_r( ptsA, numA, ptsB, numB, polytope, numPlanes, indices[ 0 ], maxPlanes );
		EPA2_r( ptsA, numA, ptsB, numB, polytope, numPlanes, indices[ 1 ], maxPlanes );
		EPA2_r( ptsA, numA, ptsB, numB, polytope, numPlanes, indices[ 2 ], maxPlanes );
	}
}
float EPA2( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, int numB, const Vec3d * polytope0 ) {
	const int maxPlanes = 128;
	plane_t planes[ maxPlanes ];

	plane_t testPlane = plane_t( Vec3d( -1, 1, 1 ), Vec3d( 1, 1, 1 ), Vec3d( 1, -1, -1 ) );

	Vec3d pts[ 4 ];
	pts[ 0 ] = Vec3d( 1, 1, 1 );
	pts[ 1 ] = Vec3d( -1, 1, 1 );
	pts[ 2 ] = Vec3d( -1, -1, 1 );
	pts[ 3 ] = Vec3d( 1, -1, -1 );

	Vec3d origin( 0 );
	ptsB = &origin;
	numB = 1;

	Tetrahedron( pts[ 0 ], pts[ 1 ], pts[ 2 ], pts[ 3 ], planes );
//	Tetrahedron( polytope0[ 0 ], polytope0[ 1 ], polytope0[ 2 ], polytope0[ 3 ], planes );
	int numPlanes = 4;

	EPA2_r( ptsA, numA, ptsB, numB, planes, numPlanes, 0, maxPlanes );
	EPA2_r( ptsA, numA, ptsB, numB, planes, numPlanes, 1, maxPlanes );
	EPA2_r( ptsA, numA, ptsB, numB, planes, numPlanes, 2, maxPlanes );
	EPA2_r( ptsA, numA, ptsB, numB, planes, numPlanes, 3, maxPlanes );

	for ( int i = 0; i < numPlanes; i++ ) {
		const plane_t & plane = planes[ i ];
		float dist = plane.SignedDistanceToPlane( Vec3d( 0 ) );
		printf( "Distance to origin: %i %f\n", i, dist );
	}

	return 0.0f;
}

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

	Vec3d * minkowski = NULL;
// 	int numPoints = MinkowskiDifference( ptsA, 8, ptsB, 8, &minkowski );
// 	BuildPolyhedra( minkowski, numPoints );
	Vec3d polytope[ 4 ];
	bool doesIntersect = GJK( ptsA, 8, ptsB, 8, polytope );
	if ( doesIntersect ) {
		printf( "Intersex!!\n" );
		float distance = EPA2( ptsA, 8, ptsB, 8, polytope );
		printf( "Closest distance = %f\n", distance );
	} else {
		printf( "No intersex!!\n" );
	}
	if ( NULL != minkowski ) {
		delete[] minkowski;
		minkowski = NULL;
	}

	return 0;
}
