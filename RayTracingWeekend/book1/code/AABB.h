/*
 *  AABB.h
 *
 */

#pragma once

#include "Vector.h"

class Ray;

inline float ffmin( const float a, const float b ) { return a < b ? a : b; }
inline float ffmax( const float a, const float b ) { return a > b ? a : b; }

/*
====================================================
AABB
====================================================
*/
class AABB {
public:
	AABB() : m_min( 10000 ), m_max( -10000 ) {}
	AABB( const Vec3d & min, const Vec3d & max ) {
		m_min = min;
		m_max = max;
	}
	AABB( const AABB & boundsA, const AABB & boudnsB );

	const AABB & operator = ( const AABB & rhs ) {
		m_min = rhs.m_min;
		m_max = rhs.m_max;
		return *this;
	}

	void Expand( const AABB & bounds );

	bool Hit( const Ray & ray, float tmin, float tmax ) const;
	bool IsValid() const;
	float Volume() const;

	Vec3d m_min;
	Vec3d m_max;
};