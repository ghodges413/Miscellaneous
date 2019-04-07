/*
 *  polytope.h
 *
 */
#pragma once
#include "Vector.h"
#include "plane.h"
#include <vector>

class Polytope {
public:
	struct edge_t {
		edge_t() {}
		edge_t( int _a, int _b ) : a( _a ), b( _b ) {}
		int a;
		int b;

		bool operator == ( const edge_t & rhs ) const {
			return ( a == rhs.a && b == rhs.b ) || ( a == rhs.b && b == rhs.a );
		}
	};
	struct triangle_t {
		int ptA;
		int ptB;
		int ptC;
	};

	void Clear();
	bool ContainsPoint( const Vec3d & pt ) const;
	static void InvertTriangle( triangle_t & tri );
	float DistanceFromTri( const triangle_t & tri, const Vec3d & pt );
	static float DistanceFromTri( const Vec3d & a, const Vec3d & b, const Vec3d & c, const Vec3d & pt );
	static int FindExtrema( const Vec3d * pts, const int num, const Vec3d & dir, const Vec3d & origin );
	Vec3d GetNormal( const triangle_t & tri );
	bool Build( const Vec3d * pts, const int num );
	void Expand( const Vec3d & pt );

	std::vector< triangle_t >	m_triangles;
	std::vector< edge_t >		m_edges;		// TODO: Implement me
	std::vector< Vec3d >		m_verts;

private:
	void RemoveExcessPoints();
};