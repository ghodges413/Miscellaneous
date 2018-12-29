//
//  main.cpp
//
#include <string>
#include <stdio.h>
#include <math.h>

/*
====================================================
GnomonicProjectedArea
Returns the gnomonic projected area of the section
of the plane in the range [0,x] u [0,y]
====================================================
*/
float GnomonicProjectedArea( const float x, const float y ) {
	const float numerator = x * y;
	const float denominator = sqrtf( x * x + y * y + 1.0f );
	const float area = atanf( numerator / denominator );
	return area;
}

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
	const float pi = acosf( -1.0f );
	
	// [0,1]u[0,1] covers exactly 1/24 of the unit cube.  And therefore,
	// from the cubic gnomonic projection, it also covers exactly 1/24 of
	// the unit sphere.  So the projected area should be pi/6.
	float area = GnomonicProjectedArea( 1.0f, 1.0f );
	printf( "area of the projection: %f %f\n", area, pi / 6.0f );

	// This barely covers the sphere, and near the pole the area should be nearly equal to the unprojected area
	area = GnomonicProjectedArea( 0.1f, 0.1f );
	printf( "area of the projection: %f %f\n", area, 0.1f * 0.1f );
	return 0;
}
