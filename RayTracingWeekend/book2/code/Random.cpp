/*
 *  Random.cpp
 *
 */

#include "Random.h"
#include "Vector.h"

std::mt19937 Random::m_generator( 0 );
std::uniform_real_distribution< float > Random::m_distribution( 0.0f, 1.0f );

/*
====================================================
Random::RandomInUnitSphere
====================================================
*/
Vec3d Random::RandomInUnitSphere() {
	Vec3d p;
	do {
		p = Vec3d( Get(), Get(), Get() ) * 2.0f - Vec3d( 1.0f, 1.0f, 1.0f );
	} while ( p.DotProduct( p ) >= 1.0f );
	return p;
}

/*
====================================================
Random::RandomOnSphereSurface
====================================================
*/
Vec3d Random::RandomOnSphereSurface() {
	Vec3d p;
	do {
		p = Vec3d( Get(), Get(), Get() ) * 2.0f - Vec3d( 1.0f, 1.0f, 1.0f );
	} while ( p.DotProduct( p ) >= 1.0f );
	p.Normalize();
	return p;
}

/*
====================================================
Random::RandomCosineDirection
====================================================
*/
Vec3d Random::RandomCosineDirection() {
	const float pi = acosf( -1.0f );

	float r1 = Get();
	float r2 = Get();
	float z = sqrtf( 1.0f - r2 );
	float phi = 2.0f * pi * r1;
	float x = cosf( phi ) * 2.0f * sqrtf( r2 );
	float y = sinf( phi ) * 2.0f * sqrtf( r2 );

	return Vec3d( x, y, z );
}

/*
====================================================
Random::RandomToSphere
====================================================
*/
Vec3d Random::RandomToSphere( float radius, float distSquared ) {
	const float pi = acosf( -1.0f );

	float r1 = Get();
	float r2 = Get();
	float z = 1.0f + r2 * ( sqrtf( 1.0f - radius * radius / distSquared ) - 1.0f );
	float phi = 2.0f * pi * r1;
	float x = cosf( phi ) * sqrtf( 1.0f - z * z );
	float y = sinf( phi ) * sqrtf( 1.0f - z * z );
	return Vec3d( x, y, z );
}