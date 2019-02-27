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
	Random() : m_generator( 0 ), m_distribution( 0.0f, 1.0f ) {}
	
	float Get() { return m_distribution( m_generator ); }	// Gets a uniform distribution in the range [ 0, 1 )
	Vec3d RandomInUnitSphere();

private:
	std::mt19937 m_generator;
	std::uniform_real_distribution< float > m_distribution;
};