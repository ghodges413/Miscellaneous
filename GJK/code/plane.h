/*
 *  Plane.h
 *
 */
#pragma once
#include "Vector.h"

struct point_t {
	Vec3d xyz;
	int idxA;	// the index of the point in polytope A
	int idxB;	// the index of the point in polytope B

	bool operator == ( const point_t & rhs ) const {
		return ( ( idxA == rhs.idxA ) && ( idxB == rhs.idxB ) && ( xyz == rhs.xyz ) );
	}
};

/*
========================================================================================================

plane_t

========================================================================================================
*/

struct plane_t {
	point_t m_pts[ 3 ];
	Vec3d m_normal;

	plane_t() {}
	plane_t( point_t a, point_t b, point_t c ) {
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
		const Vec3d & a = m_pts[ 0 ].xyz;
		const Vec3d & b = m_pts[ 1 ].xyz;
		const Vec3d & c = m_pts[ 2 ].xyz;

		const Vec3d ab = b - a;
		const Vec3d ac = c - a;

		Vec3d normal = ab.Cross( ac );
		normal.Normalize();
		return normal;
	}	

	float SignedDistanceToPlane( const Vec3d & point ) const {
		const Vec3d dirToPoint = point - m_pts[ 0 ].xyz;
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
