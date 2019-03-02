/*
 *  Material.h
 *
 */

#pragma once
#include "Vector.h"

class Ray;
struct hitRecord_t;
class Random;
class Texture;

/*
====================================================
Material
====================================================
*/
class Material {
public:
	virtual bool Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, Random & rnd ) const = 0;
};

/*
====================================================
Lambertian
====================================================
*/
class Lambertian : public Material {
public:
	Lambertian( const Texture * albedo ) : m_albedo( albedo ) {}
	virtual bool Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, Random & rnd ) const override;

private:
	const Texture * m_albedo;
};

/*
====================================================
Metal
====================================================
*/
class Metal : public Material {
public:
	Metal( const Vec3d & albedo, float fuzz ) : m_albedo( albedo ), m_fuzz( fuzz ) {}
	virtual bool Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, Random & rnd ) const override;

private:
	Vec3d m_albedo;
	float m_fuzz;
};

/*
====================================================
Dielectric
====================================================
*/
class Dielectric : public Material {
public:
	Dielectric( float ior ) : m_indexOfRefraction( ior ) {}
	virtual bool Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, Random & rnd ) const override;

private:
	float m_indexOfRefraction;
};