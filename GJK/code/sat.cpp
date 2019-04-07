//
//  sat.cpp
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
#include "polytope.h"

class Bounds2D {
public:
	Vec2d min;
	Vec2d max;

	void Clear() {
		const float inf = 10000000;
		max = Vec2d( -inf, -inf );
		min = Vec2d( inf, inf );
	}

	void Expand( const Vec2d & pt ) {
		if ( pt.x < min.x ) {
			min.x = pt.x;
		}
		if ( pt.y < min.y ) {
			min.y = pt.y;
		}
		
		if ( pt.x > max.x ) {
			max.x = pt.x;
		}
		if ( pt.y > max.y ) {
			max.y = pt.y;
		}
	}

	bool Intersect( const Bounds2D & rhs ) const {
		if ( rhs.min.x > max.x ) {
			return false;
		}
		if ( rhs.min.y > max.y ) {
			return false;
		}

		if ( min.x > rhs.max.x ) {
			return false;
		}
		if ( min.y > rhs.max.y ) {
			return false;
		}
	}
};

/*
====================================================
ProjectPoint
Converts a 3D point into a 2D point in the plane
====================================================
*/
Vec2d ProjectPoint( const Vec3d & axisX, const Vec3d & axisY, const Vec3d & pt ) {
	float u = axisX.DotProduct( pt );
	float v = axisY.DotProduct( pt );
	return Vec2d( u, v );
}

/*
====================================================
ProjectAxis
Project the polytopes onto the plane defined by the axis.
The axis is the normal of the plane, and the coaxis is an edge.
====================================================
*/
bool ProjectAxis( const Polytope & a, const Polytope & b, const Vec3d & axis, const Vec3d & coaxis, Vec3d & dv, float & dist ) {
	Bounds2D boundsA;
	Bounds2D boundsB;

	// Create an orthnormal basis for this axis
	Vec3d orthoY = axis.Cross( coaxis );
	orthoY.Normalize();
	Vec3d orthoX = orthoY.Cross( axis );	// this shouldn't be necessary, should just be coaxis
	orthoX.Normalize();

	// Expand A bounds
	boundsA.Clear();
	for ( int i = 0; i < a.m_verts.size(); i++ ) {
		const Vec3d & v = a.m_verts[ i ];

		// Project the point into the verts
		Vec2d pt = ProjectPoint( orthoX, orthoY, v );
		boundsA.Expand( pt );
	}

	// Expand B bounds
	boundsB.Clear();
	for ( int i = 0; i < b.m_verts.size(); i++ ) {
		const Vec3d & v = b.m_verts[ i ];

		// Project the point into the verts
		Vec2d pt = ProjectPoint( orthoX, orthoY, v );
		boundsB.Expand( pt );
	}

	if ( !boundsA.Intersect( boundsB ) ) {
		return false;
	}

	// Check if A is inside B
	if ( boundsA.min.x > boundsB.min.x && boundsA.max.x < boundsB.max.x ) {
		if ( boundsA.min.y > boundsB.min.y && boundsA.max.y < boundsB.max.y ) {
			float dx0 = boundsB.max.x - boundsA.min.x;
			float dx1 = boundsA.max.x - boundsB.min.x;
			float dx = dx0 < dx1 ? dx0 : dx1;

			float dy0 = boundsB.max.y - boundsA.min.y;
			float dy1 = boundsA.max.y - boundsB.min.y;
			float dy = dy0 < dy1 ? dy0 : dy1;

			if ( dx < dy ) {
				dist = dx;
				dv = orthoX * dx;
			} else {
				dist = dy;
				dv = orthoY * dy;
			}

			return true;
		}
	}

	// Check if B is inside A
	if ( boundsB.min.x > boundsA.min.x && boundsB.max.x < boundsA.max.x ) {
		if ( boundsB.min.y > boundsA.min.y && boundsB.max.y < boundsA.max.y ) {
			float dx0 = boundsA.max.x - boundsB.min.x;
			float dx1 = boundsB.max.x - boundsA.min.x;
			float dx = dx0 < dx1 ? dx0 : dx1;

			float dy0 = boundsA.max.y - boundsB.min.y;
			float dy1 = boundsB.max.y - boundsA.min.y;
			float dy = dy0 < dy1 ? dy0 : dy1;

			if ( dx < dy ) {
				dist = dx;
				dv = orthoX * dx;
			} else {
				dist = dy;
				dv = orthoY * dy;
			}

			return true;
		}
	}

	// Let's assume a shallow penetration for now... if it's a deep penetration we'll need to handle it in a different way
	float dx0 = boundsA.max.x - boundsB.min.x;
	float dx1 = boundsB.max.x - boundsA.min.x;
	float dx = dx0 < dx1 ? dx0 : dx1;

	float dy0 = boundsA.max.y - boundsB.min.y;
	float dy1 = boundsB.max.y - boundsA.min.y;
	float dy = dy0 < dy1 ? dy0 : dy1;

	assert( dx >= 0.0f );
	assert( dy >= 0.0f );

	if ( dx < dy ) {
		dist = dx;
		dv = orthoX * dx;
	} else {
		dist = dy;
		dv = orthoY * dy;
	}

	return true;
}

/*
====================================================
Separating Axis Theorem

Also calculates the minimum translation vector.  AKA, minimum displacement vector.
====================================================
*/
bool SeparatingAxisTheorem( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, point_t * simplexPoints, Vec3d & mtv ) {
	Polytope polyA;
	Polytope polyB;

	polyA.Build( ptsA, numA );
	polyB.Build( ptsB, numB );

	float minDist = 100000000;

	// Loop through each plane and check
	for ( int t = 0; t < polyA.m_triangles.size(); t++ ) {
		const Polytope::triangle_t & tri = polyA.m_triangles[ t ];
		const Vec3d normal = polyA.GetNormal( tri );

		Vec3d edges[ 3 ];
		edges[ 0 ] = polyA.m_verts[ tri.ptB ] - polyA.m_verts[ tri.ptA ];
		edges[ 1 ] = polyA.m_verts[ tri.ptC ] - polyA.m_verts[ tri.ptB ];
		edges[ 2 ] = polyA.m_verts[ tri.ptA ] - polyA.m_verts[ tri.ptC ];

		for ( int e = 0; e < 3; e++ ) {
			Vec3d edge = edges[ e ];
			edge.Normalize();

			float dist;
			Vec3d dv;
			bool doesIntersect = ProjectAxis( polyA, polyB, normal, edge, dv, dist );
			if ( !doesIntersect ) {
				return false;
			}

			if ( dist < minDist ) {
				minDist = dist;
				mtv = dv;
			}
		}
	}
	
	// Loop through each plane and check
	for ( int t = 0; t < polyB.m_triangles.size(); t++ ) {
		const Polytope::triangle_t & tri = polyB.m_triangles[ t ];
		const Vec3d normal = polyA.GetNormal( tri );

		Vec3d edges[ 3 ];
		edges[ 0 ] = polyB.m_verts[ tri.ptB ] - polyB.m_verts[ tri.ptA ];
		edges[ 1 ] = polyB.m_verts[ tri.ptC ] - polyB.m_verts[ tri.ptB ];
		edges[ 2 ] = polyB.m_verts[ tri.ptA ] - polyB.m_verts[ tri.ptC ];

		for ( int e = 0; e < 3; e++ ) {
			Vec3d edge = edges[ e ];
			edge.Normalize();

			float dist;
			Vec3d dv;
			bool doesIntersect = ProjectAxis( polyA, polyB, normal, edge, dv, dist );
			if ( !doesIntersect ) {
				return false;
			}

			if ( dist < minDist ) {
				minDist = dist;
				mtv = dv;
			}
		}
	}

	// The final thing that we need to try is an axis defined by the cross product of edges
	for ( int i = 0; i < polyA.m_edges.size(); i++ ) {
		const Polytope::edge_t & edgeA = polyA.m_edges[ i ];
		const Vec3d & edgeA0 = polyA.m_verts[ edgeA.a ];
		const Vec3d & edgeA1 = polyA.m_verts[ edgeA.b ];
		Vec3d edgeADir = edgeA1 - edgeA0;
		edgeADir.Normalize();

		for ( int j = 0; j < polyB.m_edges.size(); j++ ) {
			const Polytope::edge_t & edgeB = polyB.m_edges[ j ];
			const Vec3d & edgeB0 = polyB.m_verts[ edgeB.a ];
			const Vec3d & edgeB1 = polyB.m_verts[ edgeB.b ];
			Vec3d edgeBDir = edgeB1 - edgeB0;
			edgeBDir.Normalize();

			// The axis of projection is the cross product of the edges
			Vec3d axis = edgeADir.Cross( edgeBDir );
			if ( axis.GetLengthSqr() < 0.0001f ) {
				continue;
			}

			axis.Normalize();

			float dist;
			Vec3d dv;
			bool doesIntersect = ProjectAxis( polyA, polyB, axis, edgeADir, dv, dist );
			if ( !doesIntersect ) {
				return false;
			}

			if ( dist < minDist ) {
				minDist = dist;
				mtv = dv;
			}
		}
	}

	mtv.Normalize();
	mtv *= minDist;

	// We should probably go ahead and build polytopes out of the points
	return false;
}

