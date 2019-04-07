//
//  epa.cpp
//
#include <string>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <algorithm>
#include "Vector.h"
#include <vector>
#include "epa.h"
#include "gjk.h"
#include "plane.h"

/*
====================================================
Tetrahedron
====================================================
*/
void Tetrahedron( const point_t & ptA, const point_t & ptB, const point_t & ptC, const point_t & ptD, plane_t planes[ 4 ] ) {
	planes[ 0 ] = plane_t( ptA, ptB, ptC );
	planes[ 1 ] = plane_t( ptA, ptB, ptD );
	planes[ 2 ] = plane_t( ptA, ptD, ptC );
	planes[ 3 ] = plane_t( ptD, ptB, ptC );

	// If this plane points towards the other point... then flip the normal
	if ( planes[ 0 ].Normal().DotProduct( ptD.xyz - planes[ 0 ].m_pts[ 0 ].xyz ) > 0.0f ) {
		planes[ 0 ].FlipNormal();
	}

	// If this plane points towards the other point... then flip the normal
	if ( planes[ 1 ].Normal().DotProduct( ptC.xyz - planes[ 1 ].m_pts[ 0 ].xyz ) > 0.0f ) {
		planes[ 1 ].FlipNormal();
	}

	// If this plane points towards the other point... then flip the normal
	if ( planes[ 2 ].Normal().DotProduct( ptB.xyz - planes[ 2 ].m_pts[ 0 ].xyz ) > 0.0f ) {
		planes[ 2 ].FlipNormal();
	}

	// If this plane points towards the other point... then flip the normal
	if ( planes[ 3 ].Normal().DotProduct( ptA.xyz - planes[ 3 ].m_pts[ 0 ].xyz ) > 0.0f ) {
		planes[ 3 ].FlipNormal();
	}
}

/*
====================================================
ExpandPolytope
ExpandingPolytopeAlgorithm

This is currently only built for intersecting polytopes
====================================================
*/
int ExpandPolytope( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, plane_t * polytope, int numPlanes, int planeIdx, const int maxPlanes ) {
	if ( numPlanes + 3 > maxPlanes ) {
		printf( "Warning!  Ran out of planes!\n" );
		return numPlanes;
	}

	plane_t & plane = polytope[ planeIdx ];

	point_t newPt = Support( ptsA, numA, ptsB, numB, plane.m_normal );
	float newPtDist = plane.SignedDistanceToPlane( newPt.xyz );
	printf( "newPtDist: %f\n", newPtDist );
	if ( plane.IsCoPlanarOrBehind( newPt.xyz ) ) {
		// If the new point is coplanar, then we can't expand this face further
		printf( "Plane termination.  Plane is an edge on the convex set!\n" );
		return numPlanes;
	}

	// In order to keep our shit properly convex, we have to remove all faces that face this new point.
	// That's where we're fucking up.  Because we aren't doing that check and handling it, we end up with a convex
	// mother fucker.  And then we just keep on expanding and expanding over and over and over.
	int faceIndices[ 32 ];	// doubt we need it this high
	int numFaces = 0;
	for ( int i = 0; i < numPlanes; i++ ) {
		float dist = polytope[ i ].SignedDistanceToPlane( newPt.xyz );
		if ( dist > 0 ) {
			faceIndices[ numFaces ] = i;
			numFaces++;
		}
	}

	if ( 0 == numFaces ) {
		assert( 0 );
		return numPlanes;
	}

	if ( 1 == numFaces ) {
		plane_t newPlanes[ 3 ];
		newPlanes[ 0 ] = plane_t( newPt, plane.m_pts[ 0 ], plane.m_pts[ 1 ] );
		newPlanes[ 1 ] = plane_t( newPt, plane.m_pts[ 1 ], plane.m_pts[ 2 ] );
		newPlanes[ 2 ] = plane_t( newPt, plane.m_pts[ 2 ], plane.m_pts[ 0 ] );

		for ( int i = 0; i < 3; i++ ) {
			plane_t & plane = newPlanes[ i ];
			if ( plane.Normal().DotProduct( plane.m_pts[ 0 ].xyz ) < 0.0f ) {
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
	} else if ( numFaces > 1 ) {
		// Now, we need to get the vertices of these faces
		point_t * faceVerts = (point_t *)alloca( 3 * numFaces * sizeof( point_t ) );
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
			point_t point;
			int count;
		};

		triangle_t * tris = (triangle_t *)alloca( numFaces * sizeof( triangle_t ) );
		vertex_t * collapsedVerts = (vertex_t *)alloca( 3 * numFaces * sizeof( vertex_t ) );
		int numCollapsed = 0;
		for ( int faceIdx = 0; faceIdx < numFaces; faceIdx++ ) {
			for ( int i = 0; i < 3; i++ ) {
				const point_t & vert = faceVerts[ faceIdx * 3 + i ];

				// Try to find a previously existing one
				int newVertIdx = 0;
				bool appendNew = true;
				for ( int j = 0; j < numCollapsed; j++ ) {
					if ( collapsedVerts[ j ].point == vert ) {
						collapsedVerts[ j ].count++;
						appendNew = false;
						newVertIdx = j;
						break;
					}
				}

				// If it wasn't found, append a new one
				if ( appendNew ) {
					newVertIdx = numCollapsed;
					collapsedVerts[ numCollapsed ].point = vert;
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
		point_t * windingVerts = (point_t *)alloca( numCollapsed * sizeof( point_t ) );
		const int startWindingIdx = lowestIdx;
		int nextVertIdx = lowestIdx;
		int numWindingPoints = 1;
		for ( int i = 0; i < numCollapsed; i++ ) {
			const vertex_t & collapsedVert = collapsedVerts[ nextVertIdx ];
			windingVerts[ i ] = collapsedVert.point;

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
			if ( plane.Normal().DotProduct( plane.m_pts[ 0 ].xyz ) < 0.0f ) {
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
	}

	return numPlanes;
}

float TriangleArea( const Vec3d & a, const Vec3d & b, const Vec3d & c ) {
	const Vec3d ab = b - a;
	const Vec3d ac = c - a;
	const Vec3d norm = ab.Cross( ac );
	return norm.GetMagnitude() * 0.5f;
}

Vec3d BarycentricCoordinates( const Vec3d & a, const Vec3d & b, const Vec3d & c, const Vec3d & pt ) {
	float areaABC = TriangleArea( a, b, c );
	float areaPBC = TriangleArea( pt, b, c );
	float areaPCA = TriangleArea( pt, c, a );

	Vec3d coords;
	coords.x = areaPBC / areaABC;
	coords.y = areaPCA / areaABC;
	coords.z = 1.0f - coords.x - coords.y;
	return coords;
}

// From Real-time collision detection
Vec3d BarycentricCoordinates2( const Vec3d & a, const Vec3d & b, const Vec3d & c, const Vec3d & pt ) {
	Vec3d v0 = b - a;
	Vec3d v1 = c - a;
	Vec3d v2 = pt - a;
	float d00 = v0.DotProduct( v0 );
	float d01 = v0.DotProduct( v1 );
	float d11 = v1.DotProduct( v1 );
	float d20 = v2.DotProduct( v0 );
	float d21 = v2.DotProduct( v1 );
	float denom = d00 * d11 - d01 * d01;

	Vec3d uvw;
	uvw.y = ( d11 * d20 - d01 * d21 ) / denom;
	uvw.z = ( d00 * d21 - d01 * d20 ) / denom;
	uvw.x = 1.0f - uvw.y - uvw.z;
	return uvw;
}

/*
====================================================
EPA
ExpandingPolytopeAlgorithm

This is currently only built for intersecting polytopes
====================================================
*/
float EPA( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, int numB, const point_t * simplex ) {
	const int maxPlanes = 128;
	plane_t planes[ maxPlanes ];

	Tetrahedron( simplex[ 0 ], simplex[ 1 ], simplex[ 2 ], simplex[ 3 ], planes );
	int numPlanes = 4;

	int closestPlaneIdx = 0;
	float minDist = 0;
	while ( 1 ) {
		closestPlaneIdx = 0;
		minDist = planes[ 0 ].UnsignedDistanceToPlane( Vec3d( 0 ) );
		for ( int i = 1; i < numPlanes; i++ ) {
			float dist = planes[ i ].UnsignedDistanceToPlane( Vec3d( 0 ) );
			if ( dist < minDist ) {
				closestPlaneIdx = i;
				minDist = dist;
			}
		}

		int newNumPlanes = ExpandPolytope( ptsA, numA, ptsB, numB, planes, numPlanes, closestPlaneIdx, maxPlanes );
		if ( newNumPlanes == numPlanes ) {
			// no new planes, therefore we are done expanding because we have found the closestPlane
			numPlanes = newNumPlanes;
			break;
		}

		numPlanes = newNumPlanes;
	}

	for ( int i = 0; i < numPlanes; i++ ) {
		const plane_t & plane = planes[ i ];
		float dist = plane.SignedDistanceToPlane( Vec3d( 0 ) );
		printf( "Distance to origin: %i %f\n", i, dist );
	}

	// Now that we have the closest plane, we need to determine the point on the plane.
	// And then we need to calculate the barycentric coordinates of that point
	const plane_t & closestPlane = planes[ closestPlaneIdx ];
	Vec3d pt = closestPlane.ProjectPointOntoPlane( Vec3d( 0 ) );

	// Get the barycentric coordinates of the point.  And then use that to get the points on A and B.
	// pt = A * a + B * b + C * c;
	// a + b + c = 1
	//
	const Vec3d abc = BarycentricCoordinates2( closestPlane.m_pts[ 0 ].xyz, closestPlane.m_pts[ 1 ].xyz, closestPlane.m_pts[ 2 ].xyz, pt );

	const Vec3d & ptA0 = ptsA[ closestPlane.m_pts[ 0 ].idxA ];
	const Vec3d & ptA1 = ptsA[ closestPlane.m_pts[ 1 ].idxA ];
	const Vec3d & ptA2 = ptsA[ closestPlane.m_pts[ 2 ].idxA ];

	const Vec3d & ptB0 = ptsB[ closestPlane.m_pts[ 0 ].idxB ];
	const Vec3d & ptB1 = ptsB[ closestPlane.m_pts[ 1 ].idxB ];
	const Vec3d & ptB2 = ptsB[ closestPlane.m_pts[ 2 ].idxB ];

	const Vec3d ptOnA = ptA0 * abc.x + ptA1 * abc.y + ptA2 * abc.z;
	const Vec3d ptOnB = ptB0 * abc.x + ptB1 * abc.y + ptB2 * abc.z;

	const Vec3d delta = ptOnB - ptOnA;
	printf( "pt on A: %f %f %f\n", ptOnA.x, ptOnA.y, ptOnA.z );
	printf( "pt on B: %f %f %f\n", ptOnB.x, ptOnB.y, ptOnB.z );
	printf( "delta: %f %f %f\n", delta.x, delta.y, delta.z );
	printf( "delta Length: %f\n", delta.GetMagnitude() );

	return minDist;
}


/*
====================================================
TestEPA
ExpandingPolytopeAlgorithm

This is currently only built for intersecting polytopes
====================================================
*/
void TestEPA() {
#if 0
	const int maxPlanes = 128;
	plane_t planes[ maxPlanes ];

	int numA = 8;
	Vec3d ptsA[ 8 ];
	ptsA[ 0 ] = Vec3d( -1, -1, -1 );
	ptsA[ 1 ] = Vec3d( -1,  1, -1 );
	ptsA[ 2 ] = Vec3d(  1, -1, -1 );
	ptsA[ 3 ] = Vec3d(  1,  1, -1 );

	ptsA[ 4 ] = Vec3d( -1, -1,  1 );
	ptsA[ 5 ] = Vec3d( -1,  1,  1 );
	ptsA[ 6 ] = Vec3d(  1, -1,  1 );
	ptsA[ 7 ] = Vec3d(  1,  1,  1 );

	Vec3d origin( 0 );
	Vec3d * ptsB = &origin;
	int numB = 1;

	// Enable this to test the algorithm with a simple dataset
	Vec3d pts[ 4 ];
	pts[ 0 ] = Vec3d( 1, 1, 1 );
	pts[ 1 ] = Vec3d( -1, 1, 1 );
	pts[ 2 ] = Vec3d( -1, -1, 1 );
	pts[ 3 ] = Vec3d( 1, -1, -1 );

	Tetrahedron( pts[ 0 ], pts[ 1 ], pts[ 2 ], pts[ 3 ], planes );
	int numPlanes = 4;

	float minDist = 0;
	while ( 1 ) {
		int closestPlane = 0;
		minDist = planes[ 0 ].UnsignedDistanceToPlane( Vec3d( 0 ) );
		for ( int i = 1; i < numPlanes; i++ ) {
			float dist = planes[ i ].UnsignedDistanceToPlane( Vec3d( 0 ) );
			if ( dist < minDist ) {
				closestPlane = i;
				minDist = dist;
			}
		}

		int newNumPlanes = ExpandPolytope( ptsA, numA, ptsB, numB, planes, numPlanes, closestPlane, maxPlanes );
		if ( newNumPlanes == numPlanes ) {
			// no new planes, therefore we are done expanding because we have found the closestPlane
			numPlanes = newNumPlanes;
			break;
		}

		numPlanes = newNumPlanes;
	}

	for ( int i = 0; i < numPlanes; i++ ) {
		const plane_t & plane = planes[ i ];
		float dist = plane.SignedDistanceToPlane( Vec3d( 0 ) );
		printf( "Distance to origin: %i %f\n", i, dist );
	}
#endif
}