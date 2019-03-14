/*
 *  Material.cpp
 *
 */

#include "Material.h"
#include "Hitable.h"
#include "Random.h"
#include "Texture.h"
#include "OrthonormalBasis.h"
#include "ProbabilityDensityFunction.h"

/*
====================================================
Lambertian::Scatter
====================================================
*/
bool Lambertian::Scatter( const Ray & ray, const hitRecord_t & recordHit, scatterRecord_t & recordScatter ) const {
#if 0
	Vec3d target = record.point + record.normal + Random::RandomInUnitSphere();
	scattered = Ray( record.point, target, ray.m_time );
	albedo = m_albedo->Sample( record.point.x, record.point.y, record.point, record.normal );
	const float pi = acosf( -1.0f );
	pdf = record.normal.DotProduct( scattered.m_direction ) / pi;
	return true;
#elif 0
	const float pi = acosf( -1.0f );
	OrthonormalBasis uvw;
	uvw.BuildFromW( recordHit.normal );
	Vec3d dir = uvw.local( Random::RandomCosineDirection() ).Normalize();
	scattered = Ray( record.point, dir, ray.m_time );
	albedo = m_albedo->Sample( record.u, record.v, record.point, record.normal );
	pdf = uvw.w().DotProduct( scattered.m_direction ) / pi;
	return true;
#elif 1
	recordScatter.isSpecular = false;
	recordScatter.attenuation = m_albedo->Sample( recordHit.u, recordHit.v, recordHit.point, recordHit.normal );
	recordScatter.pdf_ptr = new ProbabilityDensityFunctionCosine( recordHit.normal );
	return true;
#endif
}

/*
====================================================
Lambertian::ScatteringPDF
====================================================
*/
float Lambertian::ScatteringPDF( const Ray & ray, const hitRecord_t & record, const Ray & scattered ) const {
	Vec3d dir = scattered.m_direction;
	dir.Normalize();

	float cosine = record.normal.DotProduct( dir );
	if ( cosine < 0 ) {
		cosine = 0;
	}

	const float pi = acosf( -1.0f );
	return cosine / pi;
}

/*
====================================================
Reflect
====================================================
*/
Vec3d Reflect( const Vec3d & dir, const Vec3d & normal ) {
	Vec3d r = dir - normal * dir.DotProduct( normal ) * 2.0f;
	return r;
}

/*
====================================================
Metal::Scatter
====================================================
*/
bool Metal::Scatter( const Ray & ray, const hitRecord_t & recordHit, scatterRecord_t & recordScatter ) const {
#if 0
	Vec3d reflected = Reflect( ray.m_direction, recordHit.normal );
	scattered = Ray( record.point, record.point + reflected + m_fuzz * Random::RandomInUnitSphere(), ray.m_time );
	attenuation = m_albedo;
	return ( scattered.m_direction.DotProduct( record.normal ) > 0.0f );
#else
	Vec3d reflected = Reflect( ray.m_direction, recordHit.normal );
	recordScatter.specularRay = Ray( recordHit.point, recordHit.point + reflected + m_fuzz * Random::RandomInUnitSphere(), ray.m_time );
	recordScatter.attenuation = m_albedo;
	recordScatter.isSpecular = true;
	recordScatter.pdf_ptr = NULL;
	return ( recordScatter.specularRay.m_direction.DotProduct( recordHit.normal ) > 0.0f );
#endif
}

float Schlick( float cosine, float indexOfRefraction ) {
	float r0 = ( 1.0f - indexOfRefraction ) / ( 1.0f + indexOfRefraction );
	r0 = r0 * r0;
	return r0 + ( 1.0f - r0 ) * pow( ( 1.0f - cosine ), 5.0f );
}

bool Refract( const Vec3d & v, const Vec3d & n, float ni_over_nt, Vec3d & refracted ) {
	Vec3d uv = v;
	uv.Normalize();
	float dt = uv.DotProduct( n );
	float discriminant = 1.0f - ni_over_nt * ni_over_nt * ( 1.0f - dt * dt );
	if ( discriminant > 0.0f ) {
		refracted = ni_over_nt * ( uv - n * dt ) - n * sqrt( discriminant );
		return true;
	}

	return false;
}

/*
====================================================
Dielectric::Scatter
====================================================
*/
bool Dielectric::Scatter( const Ray & ray, const hitRecord_t & recordHit, scatterRecord_t & recordScatter ) const {
	recordScatter.isSpecular = true;
	recordScatter.pdf_ptr = NULL;

	Vec3d outwardNormal;
	Vec3d reflected = Reflect( ray.m_direction, recordHit.normal );

	float ni_over_nt;
	recordScatter.attenuation = Vec3d( 1.0f, 1.0f, 1.0f );
	
	Vec3d refracted;
	float reflect_prob;
	float cosine;
	if ( ray.m_direction.DotProduct( recordHit.normal ) > 0.0f ) {
		outwardNormal = recordHit.normal * -1.0f;
		ni_over_nt = m_indexOfRefraction;
		cosine = m_indexOfRefraction * ray.m_direction.DotProduct( recordHit.normal ) / ray.m_direction.GetMagnitude();
	} else {
		outwardNormal = recordHit.normal;
		ni_over_nt = 1.0f / m_indexOfRefraction;
		cosine = -ray.m_direction.DotProduct( recordHit.normal ) / ray.m_direction.GetMagnitude();
	}

	if ( Refract( ray.m_direction, outwardNormal, ni_over_nt, refracted ) ) {
		reflect_prob = Schlick( cosine, m_indexOfRefraction );
	} else {
		recordScatter.specularRay = Ray( recordHit.point, recordHit.point + refracted, ray.m_time );
		reflect_prob = 1.0f;
	}

	if ( Random::Get() < reflect_prob ) {
		recordScatter.specularRay = Ray( recordHit.point, recordHit.point + reflected, ray.m_time );
	} else {
		recordScatter.specularRay = Ray( recordHit.point, recordHit.point + refracted, ray.m_time );
	}

	return true;
}

/*
====================================================
MaterialEmittance::Emitted
====================================================
*/
Vec3d MaterialEmittance::Emitted( float u, float v, const Vec3d & p, const Vec3d & normal ) const {
	Vec3d color( 1.0f );
	if ( NULL != m_texture ) {
		color = m_texture->Sample( u, v, p, normal );
	}
	color.x *= m_emittance.x;
	color.y *= m_emittance.y;
	color.z *= m_emittance.z;
	return color;
}

/*
====================================================
MaterialIsotropic::Scatter
====================================================
*/
bool MaterialIsotropic::Scatter( const Ray & ray, const hitRecord_t & recordHit, scatterRecord_t & recordScatter ) const {
	recordScatter.specularRay = Ray( recordHit.point, Random::RandomInUnitSphere(), ray.m_time );
	recordScatter.attenuation = m_albedo->Sample( recordHit.u, recordHit.v, recordHit.point, recordHit.normal );
	return true;
}