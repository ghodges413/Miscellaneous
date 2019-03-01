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
		m_min[ axis ] = ffmin( boundsA.m_min[ axis ], boundsB.m_max[ axis ] );
		m_max[ axis ] = ffmax( boundsA.m_max[ axis ], boundsB.m_max[ axis ] );
	}
}

/*
====================================================
AABB::Hit
====================================================
*/
bool AABB::Hit( const Ray & ray, float tmin, float tmax ) const {
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
}