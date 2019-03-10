/*
 *  Random.h
 *
 */

#pragma once
#include <random>
#include "Vector.h"

/*
====================================================
Random
====================================================
*/
class Random {
public:
	static float Get() { return m_distribution( m_generator ); }	// Gets a uniform distribution in the range [ 0, 1 )
	static Vec3d RandomInUnitSphere();
	static Vec3d RandomOnSphereSurface();
	static Vec3d RandomCosineDirection();

private:
	static std::mt19937 m_generator;
	static std::uniform_real_distribution< float > m_distribution;
};