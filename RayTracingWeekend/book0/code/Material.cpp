/*
 *  Material.cpp
 *
 */

#include "Material.h"
#include "Hitable.h"
#include "Random.h"

/*
====================================================
Lambertian::Scatter
====================================================
*/
bool Lambertian::Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, Random & rnd ) const {
	Vec3d target = record.point + record.normal + rnd.RandomInUnitSphere();
	scattered = Ray( record.point, target );
	attenuation = m_albedo;
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
	Vec3d reflected = Reflect( ray.dir, record.normal );
	scattered = Ray( record.point, record.point + reflected + m_fuzz * rnd.RandomInUnitSphere() );
	attenuation = m_albedo;
	return ( scattered.dir.DotProduct( record.normal ) > 0.0f );
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
	Vec3d reflected = Reflect( ray.dir, record.normal );

	float ni_over_nt;
	attenuation = Vec3d( 1.0f, 1.0f, 1.0f );
	
	Vec3d refracted;
	float reflect_prob;
	float cosine;
	if ( ray.dir.DotProduct( record.normal ) > 0.0f ) {
		outwardNormal = record.normal * -1.0f;
		ni_over_nt = m_indexOfRefraction;
		cosine = m_indexOfRefraction * ray.dir.DotProduct( record.normal ) / ray.dir.GetMagnitude();
	} else {
		outwardNormal = record.normal;
		ni_over_nt = 1.0f / m_indexOfRefraction;
		cosine = -ray.dir.DotProduct( record.normal ) / ray.dir.GetMagnitude();
	}

	if ( Refract( ray.dir, outwardNormal, ni_over_nt, refracted ) ) {
		reflect_prob = Schlick( cosine, m_indexOfRefraction );
	} else {
		scattered = Ray( record.point, record.point + refracted );
		reflect_prob = 1.0f;
	}

	if ( rnd.Get() < reflect_prob ) {
		scattered = Ray( record.point, record.point + reflected );
	} else {
		scattered = Ray( record.point, record.point + refracted );
	}

	return true;
}