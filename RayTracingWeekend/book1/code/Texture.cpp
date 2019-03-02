/*
 *  Texture.cpp
 *
 */

#pragma once

#include "Texture.h"
#include <math.h>

/*
====================================================
TextureChecker::Sample
====================================================
*/
Vec3d TextureChecker::Sample( float u, float v, const Vec3d & p ) const {
	Vec3d color = m_colorA;
	const float sines = sinf( 10.0f * p.x ) * sinf( 10.0f * p.y ) * sinf( 10.0f * p.z );
	if ( sines < 0 ) {
		color = m_colorB;
	}
	return color;
}