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
	virtual Vec3d Sample( float u, float v, const Vec3d & p ) const = 0;
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

	virtual Vec3d Sample( float u, float v, const Vec3d & p ) const override {
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

	virtual Vec3d Sample( float u, float v, const Vec3d & p ) const override;

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

	virtual Vec3d Sample( float u, float v, const Vec3d & p ) const override;

	float m_scale;
};