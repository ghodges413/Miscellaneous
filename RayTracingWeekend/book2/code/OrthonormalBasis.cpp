/*
 *  OrthonormalBasis.cpp
 *
 */

#include "OrthonormalBasis.h"

/*
====================================================
OrthonormalBasis::BuildFromW
====================================================
*/
void OrthonormalBasis::BuildFromW( const Vec3d & n ) {
	axis[ 2 ] = n;
	axis[ 2 ].Normalize();

	Vec3d a;
	if ( fabsf( w().x ) > 0.9f ) {
		a = Vec3d( 0, 0, 1 );
	} else {
		a = Vec3d( 1, 0, 0 );
	}

	axis[ 1 ] = w().Cross( a ).Normalize();
	axis[ 0 ] = w().Cross( v() );
}