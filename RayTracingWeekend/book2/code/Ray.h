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
	Ray() : m_point( 0 ), m_direction( 0 ), m_time( 0.0f ) {}
	Ray( const Vec3d & ptA, const Vec3d & ptB, const float time ) {
		m_point = ptA;
		m_direction = ptB - ptA;
		m_direction.Normalize();
		m_time = time;
	}
	const Ray & operator = ( const Ray & rhs ) {
		m_point = rhs.m_point;
		m_direction = rhs.m_direction;
		m_time = rhs.m_time;
		return *this;
	}

	Vec3d PositionAtT( const float t ) const { return ( m_point + m_direction * t ); }

	Vec3d m_point;
	Vec3d m_direction;
	float m_time;
};
