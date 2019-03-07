/*
 *  Perlin.h
 *
 */

#pragma once
#include "Vector.h"

class Random;

/*
====================================================
Perlin
====================================================
*/
class Perlin {
public:
	static void Initialize( Random & rnd );
	static float Noise( const Vec3d & p );
	static float Noise2( const Vec3d & p );
	static float Turbulence( const Vec3d & p, const int depth = 7 );

private:
	static Vec3d * PerlinGenerateVec3d( Random & rnd );
	static float * PerlinGenerate( Random & rnd );
	static void Permute( int * p, int n, Random & rnd );
	static int * PerlinGeneratePerm( Random & rnd );

	static Vec3d * s_randomVector;
	static float * s_randomFloat;
	static int * s_permX;
	static int * s_permY;
	static int * s_permZ;
};
