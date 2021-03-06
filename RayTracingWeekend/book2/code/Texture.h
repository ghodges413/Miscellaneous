/*
 *  Texture.h
 *
 */

#pragma once

#include "Vector.h"

/*
====================================================
Texture
====================================================
*/
class Texture {
public:
	virtual Vec3d Sample( float u, float v, const Vec3d & p, const Vec3d & normal ) const = 0;
};

/*
====================================================
TextureConstant
====================================================
*/
class TextureConstant : public Texture {
public:
	TextureConstant() {};
	TextureConstant( const Vec3d & color ) : m_color( color ) {}

	virtual Vec3d Sample( float u, float v, const Vec3d & p, const Vec3d & normal ) const override {
		return m_color;
	}

	Vec3d m_color;
};

/*
====================================================
TextureChecker
====================================================
*/
class TextureChecker : public Texture {
public:
	TextureChecker() {};
	TextureChecker( const Vec3d & colorA, const Vec3d & colorB ) : m_colorA( colorA ), m_colorB( colorB ) {}

	virtual Vec3d Sample( float u, float v, const Vec3d & p, const Vec3d & normal ) const override;

	Vec3d m_colorA;
	Vec3d m_colorB;
};

/*
====================================================
TextureNoise
====================================================
*/
class TextureNoise : public Texture {
public:
	TextureNoise() : m_scale( 0.5f ) {}
	TextureNoise( float scale ) : m_scale( scale ) {}

	virtual Vec3d Sample( float u, float v, const Vec3d & p, const Vec3d & normal ) const override;

	float m_scale;
};

/*
====================================================
TextureImage
====================================================
*/
class TextureImage : public Texture {
public:
	TextureImage() : m_data( NULL ), m_width( 0 ), m_height( 0 ) {}
	TextureImage( const unsigned char * pixels, int width, int height ) : m_data( pixels ), m_width( width ), m_height( height ) {}

	virtual Vec3d Sample( float u, float v, const Vec3d & p, const Vec3d & normal ) const override;

	const unsigned char * m_data;
	int m_width;
	int m_height;
};