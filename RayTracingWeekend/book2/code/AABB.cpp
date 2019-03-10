/*
 *  AABB.h
 *
 */

#pragma once

#include "AABB.h"
#include "Ray.h"
#include <utility>

/*
====================================================
AABB::AABB
====================================================
*/
AABB::AABB( const AABB & boundsA, const AABB & boundsB ) {
	for ( int axis = 0; axis < 3; axis++ ) {
		m_min[ axis ] = ffmin( boundsA.m_min[ axis ], boundsB.m_min[ axis ] );
		m_max[ axis ] = ffmax( boundsA.m_max[ axis ], boundsB.m_max[ axis ] );
	}
	assert( IsValid() );
}

/*
====================================================
AABB::Expand
====================================================
*/
void AABB::Expand( const AABB & bounds ) {
	for ( int axis = 0; axis < 3; axis++ ) {
		m_min[ axis ] = ffmin( m_min[ axis ], bounds.m_min[ axis ] );
		m_max[ axis ] = ffmax( m_max[ axis ], bounds.m_max[ axis ] );
	}
	assert( IsValid() );
}

/*
====================================================
AABB::Expand
====================================================
*/
void AABB::Expand( const Vec3d & pt ) {
	for ( int axis = 0; axis < 3; axis++ ) {
		m_min[ axis ] = ffmin( m_min[ axis ], pt[ axis ] );
		m_max[ axis ] = ffmax( m_max[ axis ], pt[ axis ] );
	}
	assert( IsValid() );
}

/*
====================================================
AABB::Hit
====================================================
*/
bool AABB::Hit( const Ray & ray, float tmin, float tmax ) const {
	assert( IsValid() );

#if 1
	for ( int axis = 0; axis < 3; axis++ ) {
		float t0 = ffmin(	( m_min[ axis ] - ray.m_point[ axis ] ) / ray.m_direction[ axis ],
							( m_max[ axis ] - ray.m_point[ axis ] ) / ray.m_direction[ axis ] );
		float t1 = ffmax(	( m_min[ axis ] - ray.m_point[ axis ] ) / ray.m_direction[ axis ],
							( m_max[ axis ] - ray.m_point[ axis ] ) / ray.m_direction[ axis ] );

		tmin = ffmax( t0, tmin );
		tmax = ffmin( t1, tmax );
		if ( tmax <= tmin ) {
			return false;
		}
	}

	return true;
#else
	// From the book
	// In reviewing this intersection method, Andrew Kensler
	// at Pixar tried some experiments and has proposed this version of the code which works
	// extremely well on many compilers, and I have adopted it as my go-to meth
	for ( int axis = 0; axis < 3; axis++ ) {
		float invD = 1.0f / ray.m_direction[ axis ];
		float t0 = ( m_min[ axis ] - ray.m_point[ axis ] ) * invD;
		float t1 = ( m_max[ axis ] - ray.m_point[ axis ] ) * invD;
		if ( invD < 0.0f ) {
			std::swap( t0, t1 );
		}

		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;
		if ( tmax <= tmin ) {
			return false;
		}
	}

	return true;
#endif
}

/*
====================================================
AABB::IsValid
====================================================
*/
bool AABB::IsValid() const {
	if ( m_min.x >= m_max.x ) {
		return false;
	}
	if ( m_min.y >= m_max.y ) {
		return false;
	}
	if ( m_min.z >= m_max.z ) {
		return false;
	}

	if ( Volume() <= 0 ) {
		return false;
	}

	return true;
}

/*
====================================================
AABB::Volume
====================================================
*/
float AABB::Volume() const {
	const float volume = ( m_max.x - m_min.x ) * ( m_max.y - m_min.y ) * ( m_max.z - m_min.z );
	assert( volume > 0 );
	return volume;
}

/*
====================================================
AABB::Radius
====================================================
*/
float AABB::Radius() const {
	Vec3d ds = m_max - m_min;
	float radius = ds.GetMagnitude() * 0.5f;
	return radius;
}

/*
====================================================
AABB::Center
====================================================
*/
Vec3d AABB::Center() const {
	Vec3d center = ( m_max + m_min ) * 0.5f;
	return center;
}