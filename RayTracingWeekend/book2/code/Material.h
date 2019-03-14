/*
 *  Material.h
 *
 */

#pragma once
#include "Vector.h"
#include "Ray.h"

struct hitRecord_t;
class Texture;
class ProbabilityDensityFunction;

/*
====================================================
scatterRecord_t
====================================================
*/
struct scatterRecord_t {
	Ray specularRay;
	bool isSpecular;
	Vec3d attenuation;
	ProbabilityDensityFunction * pdf_ptr;
};

/*
====================================================
Material
====================================================
*/
class Material {
public:
	virtual bool Scatter( const Ray & ray, const hitRecord_t & recordHit, scatterRecord_t & recordScatter ) const { return false; }
	virtual float ScatteringPDF( const Ray & ray, const hitRecord_t & record, const Ray & scattered ) const { return 0.0f; }
	virtual Vec3d Emitted( float u, float v, const Vec3d & p, const Vec3d & normal ) const { return Vec3d( 0.0f ); }

// 	virtual bool Scatter( const Ray & ray, const hitRecord_t & hrec, scatterRecord_t & srec) const {
// 		return false;}
// 	virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
// 		return false;}
//	virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p) const { return vec3(0,0,0); }
};

/*
====================================================
Lambertian
====================================================
*/
class Lambertian : public Material {
public:
	Lambertian( const Texture * albedo ) : m_albedo( albedo ) {}
	virtual bool Scatter( const Ray & ray, const hitRecord_t & recordHit, scatterRecord_t & recordScatter ) const override;
	virtual float ScatteringPDF( const Ray & ray, const hitRecord_t & record, const Ray & scattered ) const override;

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
	virtual bool Scatter( const Ray & ray, const hitRecord_t & recordHit, scatterRecord_t & recordScatter ) const override;

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
	virtual bool Scatter( const Ray & ray, const hitRecord_t & recordHit, scatterRecord_t & recordScatter ) const override;

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
	virtual bool Scatter( const Ray & ray, const hitRecord_t & recordHit, scatterRecord_t & recordScatter ) const override;

	Texture * m_albedo;
};