/*
 *  Random.cpp
 *
 */

#include "Random.h"
#include "Vector.h"

/*
====================================================
Random::RandomInUnitSphere
====================================================
*/
Vec3d Random::RandomInUnitSphere() {
	Vec3d p = Vec3d( Get(), Get(), Get() ) * 2.0f - Vec3d( 1.0f, 1.0f, 1.0f );
	return p;
}
