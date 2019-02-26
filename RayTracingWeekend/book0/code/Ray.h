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
	Ray() : pt( 0 ), dir( 0 ) {}
	Ray( const Vec3d & ptA, const Vec3d & ptB ) {
		pt = ptA;
		dir = ptB - ptA;
		dir.Normalize();
	}
	const Ray & operator = ( const Ray & rhs ) {
		pt = rhs.pt;
		dir = rhs.dir;
		return *this;
	}

	Vec3d PositionAtT( const float t ) const { return ( pt + dir * t ); }

	Vec3d pt;
	Vec3d dir;
};
