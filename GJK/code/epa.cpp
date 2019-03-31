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
Tetrahedron
====================================================
*/
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

	Vec3d newPt = Support( ptsA, numA, ptsB, numB, plane.m_normal );
	float newPtDist = plane.SignedDistanceToPlane( newPt );
	printf( "newPtDist: %f\n", newPtDist );
	if ( plane.IsCoPlanarOrBehind( newPt ) ) {
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
		float dist = polytope[ i ].SignedDistanceToPlane( newPt );
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
	} else if ( numFaces > 1 ) {
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
	}

	return numPlanes;
}

/*
====================================================
EPA2_r
ExpandingPolytopeAlgorithm

This is currently only built for intersecting polytopes
====================================================
*/
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

		std::vector< triangle_t > tris;
		tris.resize( numFaces );

		//triangle_t * tris = (triangle_t *)alloca( numFaces * sizeof( triangle_t ) );

		std::vector< vertex_t > collapsedVerts;
		collapsedVerts.resize( 3 * numFaces );

		//vertex_t * collapsedVerts = (vertex_t *)alloca( 3 * numFaces * sizeof( vertex_t ) );
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
		std::vector< Vec3d > windingVerts;
		windingVerts.resize( numCollapsed );
		//Vec3d * windingVerts = (Vec3d *)alloca( numCollapsed * sizeof( Vec3d ) );
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

		std::vector< plane_t > newPlanes;
		newPlanes.resize( numWindingPoints );
		//plane_t * newPlanes = (plane_t *)alloca( numWindingPoints * sizeof( plane_t ) );
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

		std::vector< int > indices;
		indices.resize( numWindingPoints );
		//int * indices = (int *)alloca( numWindingPoints * sizeof( int ) );
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

/*
====================================================
EPA2
ExpandingPolytopeAlgorithm

This is currently only built for intersecting polytopes
====================================================
*/
float EPA2( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, int numB, const Vec3d * polytope0 ) {
	const int maxPlanes = 128;
	plane_t planes[ maxPlanes ];

#if 1	// Enable this to test the algorithm with a simple dataset
	Vec3d pts[ 4 ];
	pts[ 0 ] = Vec3d( 1, 1, 1 );
	pts[ 1 ] = Vec3d( -1, 1, 1 );
	pts[ 2 ] = Vec3d( -1, -1, 1 );
	pts[ 3 ] = Vec3d( 1, -1, -1 );

	Vec3d origin( 0 );
	ptsB = &origin;
	numB = 1;

	Tetrahedron( pts[ 0 ], pts[ 1 ], pts[ 2 ], pts[ 3 ], planes );
#else
	Tetrahedron( polytope0[ 0 ], polytope0[ 1 ], polytope0[ 2 ], polytope0[ 3 ], planes );
#endif
	int numPlanes = 4;

	EPA2_r( ptsA, numA, ptsB, numB, planes, numPlanes, 0, maxPlanes );
	EPA2_r( ptsA, numA, ptsB, numB, planes, numPlanes, 1, maxPlanes );
	EPA2_r( ptsA, numA, ptsB, numB, planes, numPlanes, 2, maxPlanes );
	EPA2_r( ptsA, numA, ptsB, numB, planes, numPlanes, 3, maxPlanes );

	float minDist = planes[ 0 ].SignedDistanceToPlane( Vec3d( 0 ) );

	for ( int i = 0; i < numPlanes; i++ ) {
		const plane_t & plane = planes[ i ];
		float dist = plane.SignedDistanceToPlane( Vec3d( 0 ) );
		printf( "Distance to origin: %i %f\n", i, dist );
	}

	return 0.0f;
}

/*
====================================================
EPA3
ExpandingPolytopeAlgorithm

This is currently only built for intersecting polytopes
====================================================
*/
float EPA3( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, int numB, const Vec3d * polytope0 ) {
	const int maxPlanes = 128;
	plane_t planes[ maxPlanes ];

	Tetrahedron( polytope0[ 0 ], polytope0[ 1 ], polytope0[ 2 ], polytope0[ 3 ], planes );
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

	return minDist;
}
