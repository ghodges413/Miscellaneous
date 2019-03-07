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
	Vec3d p = Vec3d( Get(), Get(), Get() ) * 2.0f - Vec3d( 1.0f, 1.0f, 1.0f );
	return p;
}
