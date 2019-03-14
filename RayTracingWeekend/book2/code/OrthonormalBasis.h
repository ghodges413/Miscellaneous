/*
 *  OrthonormalBasis.h
 *
 */

#pragma once

#include "Vector.h"

/*
====================================================
OrthonormalBasis
====================================================
*/
class OrthonormalBasis {
public:
	OrthonormalBasis() {}
	inline Vec3d operator[]( int i ) const { return axis[ i ]; }
	Vec3d u() const { return axis[ 0 ]; }
	Vec3d v() const { return axis[ 1 ]; }
	Vec3d w() const { return axis[ 2 ]; }
	Vec3d local( float a, float b, float c ) const { return a * u() + b * v() + c * w(); }
	Vec3d local( const Vec3d & a ) const { return a.x * u() + a.y * v() + a.z * w(); }
	void BuildFromW( const Vec3d & );

	Vec3d axis[ 3 ];	
};