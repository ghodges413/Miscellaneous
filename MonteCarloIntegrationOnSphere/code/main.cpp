//
//  main.cpp
//
#include <string>
#include <stdio.h>
#include <math.h>
#include <random>

/*
====================================================
RandomFloatRange01
Returns a uniformly distributed random number in the range [0,1]
====================================================
*/
double RandomFloatRange01() {
	return ( double( rand() ) / double( RAND_MAX ) );
}

/*
====================================================
GetRandomThetaPhi
Returns uniformly distributed random spherical coordinates
theta is the polar angle away from the zenith [ 0, pi ]
phi is the azimuthal angle [ 0, 2 * pi ]
====================================================
*/
void GetRandomThetaPhi( double & theta, double & phi ) {
	const double pi = acos( -1.0 );
	const double r1 = RandomFloatRange01();
	const double r2 = RandomFloatRange01();

	theta = acos( 1.0f - 2.0 * r2 );
	phi = 2.0 * pi * r1;
}

/*
====================================================
Sampling functions
====================================================
*/
typedef double SamplingFunction_t( const double theta, const double phi );

double SamplingUniform( const double theta, const double phi ) {
	return 1.0;
}
double SamplingPolarized( const double theta, const double phi ) {
	return cos( theta );
}
double SamplingRing( const double theta, const double phi ) {
	return sin( theta );
}
double SamplingHalf( const double theta, const double phi ) {
	const double pi = acos( -1.0 );
	if ( theta > pi / 2.0 ) {
		return 1.0;
	}
	return 0.0;
}
double Sampling8( const double theta, const double phi ) {
	const double pi = acos( -1.0 );
	const double halfPi = 0.5 * pi;
	if ( theta > halfPi && phi < halfPi ) {
		return 1.0;
	}
	return 0.0;
}

/*
====================================================
MonteCarloSphere
====================================================
*/
double MonteCarloSphere( const int maxSamples, SamplingFunction_t * sampler ) {
	double sum = 0;

	for ( int i = 0; i < maxSamples; i++ ) {
		double theta;
		double phi;
		GetRandomThetaPhi( theta, phi );

		sum += sampler( theta, phi ) / double( maxSamples );
	}

	return sum;
}

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
	const int N = 1280000;

	const double uniformSamples = MonteCarloSphere( N, SamplingUniform );
	const double polarizedSamples = MonteCarloSphere( N, SamplingPolarized );
	const double ringSamples = MonteCarloSphere( N, SamplingRing );
	const double halfSamples = MonteCarloSphere( N, SamplingHalf );
	const double samples8 = MonteCarloSphere( N, Sampling8 );

	printf( "Uniform distribution: %f\n", uniformSamples );
	printf( "Polar distribution: %f\n", polarizedSamples );
	printf( "Ring distribution: %f\n", ringSamples );
	printf( "Half distribution: %f\n", halfSamples );
	printf( "1/8 distribution: %f\n", samples8 );
	printf( "The analytic integrations of these functions should be 1, 0, pi / 4, 0.5, 0.125\n" );

	return 0;
}
