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
Vec3d TextureChecker::Sample( float u, float v, const Vec3d & p, const Vec3d & normal ) const {
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
Vec3d TextureNoise::Sample( float u, float v, const Vec3d & p, const Vec3d & normal ) const {
//	return Vec3d( 1.0f ) * Perlin::Noise( p );
//	return Vec3d( 1.0f ) * Perlin::Noise2( p );
	return Vec3d( 1.0f ) * 0.5f * ( 1.0f + sinf( m_scale * p.z + 10.0f * Perlin::Turbulence( m_scale * p ) ) );
}

/*
====================================================
SphereUV
====================================================
*/
void SphereUV( const Vec3d & p, float & u, float & v ) {
	const float pi = acosf( -1.0f );

	const float phi = atan2f( p.x, p.y );
	const float theta = asinf( p.z );

	u = 1.0f - ( phi + pi ) / ( 2.0f * pi );
	v = ( theta + 0.5f * pi ) / pi;
}

/*
====================================================
TextureImage::Sample
====================================================
*/
Vec3d TextureImage::Sample( float u, float v, const Vec3d & p, const Vec3d & normal ) const {
	SphereUV( normal, u, v );

	int i = u * m_width;
	int j = ( 1.0f - v ) * m_height - 0.001f;

	if ( i < 0.0f ) {
		i = 0;
	}
	if ( j < 0.0f ) {
		j = 0;
	}
	if ( i >= m_width ) {
		i = m_width - 1;
	}
	if ( j >= m_height ) {
		j = m_height - 1;
	}

	int idx = i + m_width * j;

	float r = int( m_data[ 4 * idx + 0 ] ) / 255.0f;
	float g = int( m_data[ 4 * idx + 1 ] ) / 255.0f;
	float b = int( m_data[ 4 * idx + 2 ] ) / 255.0f;
	return Vec3d( r, g, b );
}
