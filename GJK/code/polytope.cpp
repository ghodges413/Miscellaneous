//
//  polytope.cpp
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
#include "epa.h"

/*
====================================================
Polytope::ContainsPoint
====================================================
*/
bool Polytope::ContainsPoint( const Vec3d & pt ) const {
	for ( int i = 0; i < m_triangles.size(); i++ ) {
		const triangle_t & tri = m_triangles[ i ];
		const Vec3d & a = m_verts[ tri.ptA ];
		const Vec3d & b = m_verts[ tri.ptB ];
		const Vec3d & c = m_verts[ tri.ptC ];

		if ( DistanceFromTri( a, b, c, pt ) > 0.0f ) {
			return false;
		}
	}

	return true;
}

/*
====================================================
Polytope::InvertTriangle
Swaps the ordering to reverse the normal
====================================================
*/
void Polytope::InvertTriangle( triangle_t & tri ) {
	std::swap( tri.ptA, tri.ptB );
}

/*
====================================================
Polytope::DistanceFromTri
Returns the projected distance of this point onto the plane of the triangle
====================================================
*/
float Polytope::DistanceFromTri( const triangle_t & tri, const Vec3d & pt ) {
	const Vec3d & a = m_verts[ tri.ptA ];
	const Vec3d & b = m_verts[ tri.ptB ];
	const Vec3d & c = m_verts[ tri.ptC ];
	return DistanceFromTri( a, b, c, pt );
}

/*
====================================================
Polytope::DistanceFromTri
Returns the projected distance of this point onto the plane of the triangle
====================================================
*/
float Polytope::DistanceFromTri( const Vec3d & a, const Vec3d & b, const Vec3d & c, const Vec3d & pt ) {
	const Vec3d ab = b - a;
	const Vec3d ac = c - a;
	const Vec3d normal = ab.Cross( ac );
	const Vec3d apt = pt - a;
	const float dist = normal.DotProduct( apt );
	return dist;
}

/*
====================================================
Polytope::FindExtrema
====================================================
*/
int Polytope::FindExtrema( const Vec3d * pts, const int num, const Vec3d & dir, const Vec3d & origin ) {
	int maxPt = 0;
	Vec3d tmp = pts[ 0 ] - origin;
	float maxDist = dir.DotProduct( tmp );

	for ( int i = 0; i < num; i++ ) {
		tmp = pts[ i ] - origin;
	
		float dist = dir.DotProduct( tmp );
		if ( dist > maxDist ) {
			maxDist = dist;
			maxPt = i;
		}
	}

	return maxPt;
}

/*
====================================================
Polytope::GetNormal
====================================================
*/
Vec3d Polytope::GetNormal( const triangle_t & tri ) {
	const Vec3d & a = m_verts[ tri.ptA ];
	const Vec3d & b = m_verts[ tri.ptB ];
	const Vec3d & c = m_verts[ tri.ptC ];

	const Vec3d ab = b - a;
	const Vec3d ac = c - a;
	Vec3d normal = ab.Cross( ac );
	normal.Normalize();
	return normal;
}

/*
====================================================
Polytope::Clear
====================================================
*/
void Polytope::Clear() {
	m_verts.clear();
	m_edges.clear();
	m_triangles.clear();
}

/*
====================================================
Polytope::Expand
====================================================
*/
void Polytope::Expand( const Vec3d & pt ) {
	if ( ContainsPoint( pt ) ) {
		return;
	}

	m_verts.push_back( pt );
	const int ptIdx = m_verts.size() - 1;

	// Get the triangles facing the point
	std::vector< int > trisFacingPt;
	for ( int i = 0; i < m_triangles.size(); i++ ) {
		float dist = DistanceFromTri( m_triangles[ i ], pt );
		if ( dist > 0.0f ) {
			trisFacingPt.push_back( i );
		}
	}

	// Sort from the high to low index order
	struct intCompare_t {
		static int compare( const void * a, const void * b ) {
			const int ai = *(int * )a;
			const int bi = *(int * )b;
			return bi - ai;
		}
	};
	qsort( &trisFacingPt[ 0 ], trisFacingPt.size(), sizeof( int ), intCompare_t::compare );

	// Replace the old triangles with the new
	if ( 1 == trisFacingPt.size() ) {
		triangle_t & tri = m_triangles[ trisFacingPt[ 0 ] ];

		// Calculate new triangles from this triangle
		triangle_t newTris[ 3 ];
		newTris[ 0 ].ptA = ptIdx;
		newTris[ 0 ].ptB = tri.ptA;
		newTris[ 0 ].ptC = tri.ptB;

		newTris[ 1 ].ptA = ptIdx;
		newTris[ 1 ].ptB = tri.ptB;
		newTris[ 1 ].ptC = tri.ptC;

		newTris[ 2 ].ptA = ptIdx;
		newTris[ 2 ].ptB = tri.ptC;
		newTris[ 2 ].ptC = tri.ptA;

		tri = newTris[ 0 ];
		m_triangles.push_back( newTris[ 1 ] );
		m_triangles.push_back( newTris[ 2 ] );
	} else {
		// Now this is the complicated thing that we've done before
		// We could find all the edges that aren't shared and use those
		std::vector< edge_t > uniqueEdges;
		for ( int i = 0; i < trisFacingPt.size(); i++ ) {
			const triangle_t & tri = m_triangles[ trisFacingPt[ i ] ];

			edge_t edges[ 3 ];
			edges[ 0 ] = edge_t( tri.ptA, tri.ptB );
			edges[ 1 ] = edge_t( tri.ptB, tri.ptC );
			edges[ 2 ] = edge_t( tri.ptC, tri.ptA );

			// now we need to check if these edges are unique
			for ( int e = 0; e < 3; e++ ) {
				const edge_t & edge = edges[ e ];

				bool isUnique = true;
				for ( int j = 0; j < trisFacingPt.size(); j++ ) {
					// Don't compare the edges of the same triangle
					if ( i == j ) {
						continue;
					}

					// Collect the edges of the comparison triangle
					const triangle_t & triOther = m_triangles[ trisFacingPt[ j ] ];
					edge_t edgesOther[ 3 ];
					edgesOther[ 0 ] = edge_t( triOther.ptA, triOther.ptB );
					edgesOther[ 1 ] = edge_t( triOther.ptB, triOther.ptC );
					edgesOther[ 2 ] = edge_t( triOther.ptC, triOther.ptA );

					// Compare this edge with the other edges
					for ( int newE = 0; newE < 3; newE++ ) {
						const edge_t & otherEdge = edgesOther[ newE ];

						if ( otherEdge.a == edge.a && otherEdge.b == edge.b ) {
							isUnique = false;
							break;
						}
						if ( otherEdge.a == edge.b && otherEdge.b == edge.a ) {
							isUnique = false;
							break;
						}
					}

					if ( !isUnique ) {
						break;
					}
				}

				if ( isUnique ) {
					uniqueEdges.push_back( edge );
				}
			}
		}

		// Okay, we now have our unique edges... and they're in a healthy winding order :)
		std::vector< triangle_t > newTris;
		for ( int i = 0; i < uniqueEdges.size(); i++ ) {
			const edge_t & edge = uniqueEdges[ i ];

			triangle_t newTri;
			newTri.ptA = ptIdx;
			newTri.ptB = edge.a;
			newTri.ptC = edge.b;

			newTris.push_back( newTri );
		}

		// Now we need to remove the old ones
		for ( int i = 0; i < trisFacingPt.size(); i++ ) {
			m_triangles.erase( m_triangles.begin() + trisFacingPt[ i ] );
		}

		// Append the new triangles
		for ( int i = 0; i < newTris.size(); i++ ) {
			m_triangles.push_back( newTris[ i ] );
		}
	}

	// Remove excess points
	RemoveExcessPoints();
}

/*
====================================================
Polytope::RemoveExcessPoints
====================================================
*/
void Polytope::RemoveExcessPoints() {
	for ( int v = 0; v < m_verts.size(); v++ ) {
		bool isVertUsed = false;
		for ( int t = 0; t < m_triangles.size(); t++ ) {
			const triangle_t & tri = m_triangles[ t ];

			if ( v == tri.ptA || v == tri.ptB || v == tri.ptC ) {
				isVertUsed = true;
				break;
			}
		}

		// remove the unused vert
		if ( !isVertUsed ) {
			m_verts.erase( m_verts.begin() + v );

			// correct all triangle indices
			for ( int t = 0; t < m_triangles.size(); t++ ) {
				triangle_t & tri = m_triangles[ t ];

				if ( tri.ptA > v ) {
					tri.ptA--;
				}
				if ( tri.ptB > v ) {
					tri.ptB--;
				}
				if ( tri.ptC > v ) {
					tri.ptC--;
				}
			}

			v--;
		}
	}
}

/*
====================================================
Polytope::Build
Builds a convex polytope from a cloud of points
====================================================
*/
bool Polytope::Build( const Vec3d * pts, const int num ) {
	Clear();
	const Vec3d origin( 0 );

	// Collect all the verts
	for ( int i = 0; i < num; i++ ) {
		m_verts.push_back( pts[ i ] );
	}

	// Build the first triangle
	triangle_t tri;
	tri.ptA = 0;

	// Choose the second vertex in the first triangle
	for ( int i = 1; i < num; i++ ) {
		const Vec3d & a = m_verts[ tri.ptA ];
		const Vec3d & b = m_verts[ i ];
		const Vec3d ab = b - a;
		if ( ab.GetLengthSqr() > 0.0001f ) {
			tri.ptB = i;
			break;
		}
	}

	// Choose the third vertex in the first triangle
	for ( int i = 1; i < num; i++ ) {
		if ( i == tri.ptB ) {
			continue;
		}

		const Vec3d & a = m_verts[ tri.ptA ];
		const Vec3d & b = m_verts[ tri.ptB ];
		const Vec3d & c = m_verts[ i ];

		const float area = TriangleArea( a, b, c );
		if ( area > 0.001f ) {
			tri.ptC = i;
			break;
		}
	}

	// Verify the triangle is valid
	float area = TriangleArea( m_verts[ tri.ptA ], m_verts[ tri.ptB ], m_verts[ tri.ptC ] );
	if ( area != area ) {
		return false;
	}
	if ( fabsf( area ) < 0.00001f ) {
		return false;
	}

	// Find another point to build the initial tetrahedron/simplex.  If only co-planar points are found, then return false.
	Vec3d normal = GetNormal( tri );
	int ptIdx = FindExtrema( pts, num, normal * -1.0f, m_verts[ tri.ptA ] );
	float dist = DistanceFromTri( m_verts[ tri.ptA ], m_verts[ tri.ptB ], m_verts[ tri.ptC ], m_verts[ ptIdx ] );
	if ( fabsf( dist ) < 0.0001f ) {
		ptIdx = FindExtrema( pts, num, normal, m_verts[ tri.ptA ] );
		dist = DistanceFromTri( m_verts[ tri.ptA ], m_verts[ tri.ptB ], m_verts[ tri.ptC ], m_verts[ ptIdx ] );
		if ( fabsf( dist ) < 0.0001f ) {
			return false;
		}
	}

	// Now build the initial tetrahedron
	{
		if ( dist < 0.0f ) {
			InvertTriangle( tri );
		}
		m_triangles.push_back( tri );

		triangle_t newTri;
		newTri.ptA = ptIdx;
		newTri.ptB = tri.ptB;
		newTri.ptC = tri.ptA;
		m_triangles.push_back( newTri );

		newTri.ptB = tri.ptC;
		newTri.ptC = tri.ptB;
		m_triangles.push_back( newTri );

		newTri.ptB = tri.ptA;
		newTri.ptC = tri.ptC;
		m_triangles.push_back( newTri );
	}

	// Now we need to expand this thing
	for ( int i = 0; i < m_verts.size(); i++ ) {
		const Vec3d & pt = m_verts[ i ];

		if ( ContainsPoint( pt ) ) {
			continue;
		}

		Expand( pt );
	}

	// Build a unique list of edges
	for ( int i = 0; i < m_triangles.size(); i++ ) {
		const triangle_t & tri = m_triangles[ i ];

		edge_t edges[ 3 ];
		edges[ 0 ] = edge_t( tri.ptA, tri.ptB );
		edges[ 1 ] = edge_t( tri.ptB, tri.ptC );
		edges[ 2 ] = edge_t( tri.ptC, tri.ptA );

		for ( int e = 0; e < 3; e++ ) {
			if ( std::find( m_edges.begin(), m_edges.end(), edges[ e ] ) == m_edges.end() ) {
				m_edges.push_back( edges[ e ] );
			}
		}
	}
}


