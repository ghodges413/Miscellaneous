/*
 *  Material.cpp
 *
 */

#include "Material.h"
#include "Hitable.h"
#include "Random.h"
#include "Texture.h"

/*
====================================================
Lambertian::Scatter
====================================================
*/
bool Lambertian::Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, Random & rnd ) const {
	Vec3d target = record.point + record.normal + rnd.RandomInUnitSphere();
	scattered = Ray( record.point, target, ray.m_time );
	attenuation = m_albedo->Sample( record.point.x, record.point.y, record.point, record.normal );
	return true;
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
bool Metal::Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, Random & rnd ) const {
	Vec3d reflected = Reflect( ray.m_direction, record.normal );
	scattered = Ray( record.point, record.point + reflected + m_fuzz * rnd.RandomInUnitSphere(), ray.m_time );
	attenuation = m_albedo;
	return ( scattered.m_direction.DotProduct( record.normal ) > 0.0f );
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
bool Dielectric::Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, Random & rnd ) const {
	Vec3d outwardNormal;
	Vec3d reflected = Reflect( ray.m_direction, record.normal );

	float ni_over_nt;
	attenuation = Vec3d( 1.0f, 1.0f, 1.0f );
	
	Vec3d refracted;
	float reflect_prob;
	float cosine;
	if ( ray.m_direction.DotProduct( record.normal ) > 0.0f ) {
		outwardNormal = record.normal * -1.0f;
		ni_over_nt = m_indexOfRefraction;
		cosine = m_indexOfRefraction * ray.m_direction.DotProduct( record.normal ) / ray.m_direction.GetMagnitude();
	} else {
		outwardNormal = record.normal;
		ni_over_nt = 1.0f / m_indexOfRefraction;
		cosine = -ray.m_direction.DotProduct( record.normal ) / ray.m_direction.GetMagnitude();
	}

	if ( Refract( ray.m_direction, outwardNormal, ni_over_nt, refracted ) ) {
		reflect_prob = Schlick( cosine, m_indexOfRefraction );
	} else {
		scattered = Ray( record.point, record.point + refracted, ray.m_time );
		reflect_prob = 1.0f;
	}

	if ( rnd.Get() < reflect_prob ) {
		scattered = Ray( record.point, record.point + reflected, ray.m_time );
	} else {
		scattered = Ray( record.point, record.point + refracted, ray.m_time );
	}

	return true;
}

/*
====================================================
MaterialEmittance::Scatter
====================================================
*/
bool MaterialEmittance::Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, Random & rnd ) const {
	return false;
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