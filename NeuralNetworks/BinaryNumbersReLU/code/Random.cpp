//
//	Random.cpp
//
#include "Random.h"
#include "Vector.h"

std::mt19937 Random::m_generator( 0 );
std::uniform_real_distribution< float > Random::m_distribution( 0.0f, 1.0f );

/*
================================
hbRandom::Gaussian

// Generating gaussian random number with mean 0 and standard deviation 1.
// Basic Box-Muller transform
================================
*/
float Random::Gaussian( const float mean, const float stdDeviation, const float epsilon ) {
	const float pi = acosf( -1.0f );

	float u1 = Get();
	float u2 = Get();

	while ( u1 <= epsilon ) {
		u1 = Get();
		u2 = Get();
	}

	float z0 = sqrtf( -2.0f * logf( u1 ) ) * cosf( 2.0f * pi * u2 );
	z0 = z0 * stdDeviation + mean;

	return z0;
}
