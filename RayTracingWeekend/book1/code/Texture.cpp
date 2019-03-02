/*
 *  Texture.cpp
 *
 */

#pragma once

#include "Texture.h"
#include "Perlin.h"
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

/*
====================================================
TextureChecker::Sample
====================================================
*/
Vec3d TextureNoise::Sample( float u, float v, const Vec3d & p ) const {
//	return Vec3d( 1.0f ) * Perlin::Noise( p );
//	return Vec3d( 1.0f ) * Perlin::Noise2( p );
	return Vec3d( 1.0f ) * 0.5f * ( 1.0f + sinf( m_scale * p.z + 10.0f * Perlin::Turbulence( m_scale * p ) ) );
}

