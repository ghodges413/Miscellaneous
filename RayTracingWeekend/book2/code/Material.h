/*
 *  Material.h
 *
 */

#pragma once
#include "Vector.h"

class Ray;
struct hitRecord_t;
class Texture;

/*
====================================================
Material
====================================================
*/
class Material {
public:
	virtual bool Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered ) const = 0;
	virtual float ScatteringPDF( const Ray & ray, const hitRecord_t & record, const Ray & scattered ) const { return false; }
	virtual Vec3d Emitted( float u, float v, const Vec3d & p, const Vec3d & normal ) const { return Vec3d( 0.0f ); }
};

/*
====================================================
Lambertian
====================================================
*/
class Lambertian : public Material {
public:
	Lambertian( const Texture * albedo ) : m_albedo( albedo ) {}
	virtual bool Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered ) const override;

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
	virtual bool Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered ) const override;

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
	virtual bool Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered ) const override;

private:
	float m_indexOfRefraction;
};

/*
====================================================
MaterialEmittance
====================================================
*/
class MaterialEmittance : public Material {
public:
	MaterialEmittance() : m_texture( NULL ), m_emittance( 1.0f ) {}
	MaterialEmittance( Texture * texture, Vec3d emittance ) : m_texture( texture ), m_emittance( emittance ) {}
	virtual bool Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered ) const override;
	virtual Vec3d Emitted( float u, float v, const Vec3d & p, const Vec3d & normal ) const override;

	Texture * m_texture;
	Vec3d m_emittance;
};

/*
====================================================
MaterialIsotropic
====================================================
*/
class MaterialIsotropic : public Material {
public:
	MaterialIsotropic() : m_albedo( NULL ) {}
	MaterialIsotropic( Texture * albedo ) : m_albedo( albedo ) {}
	virtual bool Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered ) const override;

	Texture * m_albedo;
};