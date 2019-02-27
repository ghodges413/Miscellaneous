/*
 *  Ray.h
 *
 */

#pragma once

#include "Vector.h"

/*
====================================================
Ray
====================================================
*/
class Ray {
public:
	Ray() : m_point( 0 ), m_direction( 0 ) {}
	Ray( const Vec3d & ptA, const Vec3d & ptB ) {
		m_point = ptA;
		m_direction = ptB - ptA;
		m_direction.Normalize();
	}
	const Ray & operator = ( const Ray & rhs ) {
		m_point = rhs.m_point;
		m_direction = rhs.m_direction;
		return *this;
	}

	Vec3d PositionAtT( const float t ) const { return ( m_point + m_direction * t ); }

	Vec3d m_point;
	Vec3d m_direction;
};
