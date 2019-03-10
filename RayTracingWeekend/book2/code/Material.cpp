/*
 *  Material.cpp
 *
 */

#include "Material.h"
#include "Hitable.h"
#include "Random.h"
#include "Texture.h"


class OrthonormalBasis {
public:
	OrthonormalBasis() {}
	inline Vec3d operator[]( int i ) const { return axis[ i ]; }
	Vec3d u() const { return axis[ 0 ]; }
	Vec3d v() const { return axis[ 1 ]; }
	Vec3d w() const { return axis[ 2 ]; }
	Vec3d local( float a, float b, float c ) const { return a * u() + b * v() + c * w(); }
	Vec3d local( const Vec3d & a ) const { return a.x * u() + a.y * v() + a.z * w(); }
	void BuildFromW( const Vec3d & );

	Vec3d axis[ 3 ];	
};

void OrthonormalBasis::BuildFromW( const Vec3d & n ) {
	axis[ 2 ] = n;
	axis[ 2 ].Normalize();

	Vec3d a;
	if ( fabsf( w().x ) > 0.9f ) {
		a = Vec3d( 0, 1, 0 );
	} else {
		a = Vec3d( 1, 0, 0 );
	}

	axis[ 1 ] = w().Cross( a ).Normalize();
	axis[ 0 ] = w().Cross( v() );
}


/*
====================================================
Lambertian::Scatter
====================================================
*/
bool Lambertian::Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & albedo, Ray & scattered, float & pdf ) const {
#if 0
	Vec3d target = record.point + record.normal + Random::RandomInUnitSphere();
	scattered = Ray( record.point, target, ray.m_time );
	albedo = m_albedo->Sample( record.point.x, record.point.y, record.point, record.normal );
	const float pi = acosf( -1.0f );
	pdf = record.normal.DotProduct( scattered.m_direction ) / pi;
	return true;
#else
	const float pi = acosf( -1.0f );
	OrthonormalBasis uvw;
	uvw.BuildFromW( record.normal );
	Vec3d dir = uvw.local( Random::RandomCosineDirection() ).Normalize();
	scattered = Ray( record.point, dir, ray.m_time );
	albedo = m_albedo->Sample( record.u, record.v, record.point, record.normal );
	pdf = uvw.w().DotProduct( scattered.m_direction ) / pi;
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
bool Metal::Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, float & pdf ) const {
	Vec3d reflected = Reflect( ray.m_direction, record.normal );
	scattered = Ray( record.point, record.point + reflected + m_fuzz * Random::RandomInUnitSphere(), ray.m_time );
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
bool Dielectric::Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, float & pdf ) const {
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

	if ( Random::Get() < reflect_prob ) {
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
bool MaterialEmittance::Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, float & pdf ) const {
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

/*
====================================================
MaterialIsotropic::Scatter
====================================================
*/
bool MaterialIsotropic::Scatter( const Ray & ray, const hitRecord_t & record, Vec3d & attenuation, Ray & scattered, float & pdf ) const {
	scattered = Ray( record.point, Random::RandomInUnitSphere(), ray.m_time );
	attenuation = m_albedo->Sample( record.u, record.v, record.point, record.normal );
	return true;
}