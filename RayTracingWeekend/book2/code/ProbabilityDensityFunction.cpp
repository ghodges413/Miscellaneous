/*
 *  ProbabilityDensityFunction.cpp
 *
 */

#include "ProbabilityDensityFunction.h"
#include "Random.h"
#include "Hitable.h"

/*
====================================================
ProbabilityDensityFunctionCosine::ProbabilityDensityFunctionCosine
====================================================
*/
ProbabilityDensityFunctionCosine::ProbabilityDensityFunctionCosine( const Vec3d & uvw ) {
	m_onb.BuildFromW( uvw );
}

/*
====================================================
ProbabilityDensityFunctionCosine::Value
====================================================
*/
float ProbabilityDensityFunctionCosine::Value( const Vec3d & dir ) const {
	const float pi = acosf( -1.0f );

	Vec3d normalized = dir;
	normalized.Normalize();

	float cosine = normalized.DotProduct( m_onb.w() );
	if ( cosine > 0.0f ) {
		return cosine / pi;
	}

	return 0.0f;
}

/*
====================================================
ProbabilityDensityFunctionCosine::Generate
====================================================
*/
Vec3d ProbabilityDensityFunctionCosine::Generate() const {
	Vec3d origin = Vec3d( 0 );
	Vec3d normal = m_onb.w();
	Vec3d randomPoint = origin + normal + Random::RandomInUnitSphere();
	Vec3d randomDir = randomPoint - origin;
	return randomDir;
	//return m_onb.local( Random::RandomCosineDirection() );
}

/*
====================================================
ProbabilityDensityFunctionHitable::Generate
====================================================
*/
float ProbabilityDensityFunctionHitable::Value( const Vec3d & dir ) const {
	return m_hitable->ValuePDF( m_origin, dir, m_normal );
}

/*
====================================================
ProbabilityDensityFunctionHitable::Generate
====================================================
*/
Vec3d ProbabilityDensityFunctionHitable::Generate() const {
	return m_hitable->Random( m_origin );
}

/*
====================================================
ProbabilityDensityFunctionMixed::Generate
====================================================
*/
float ProbabilityDensityFunctionMixed::Value( const Vec3d & dir ) const {
	float pdf = m_pdf0->Value( dir ) * 0.5f + m_pdf1->Value( dir ) * 0.5f;
	return pdf;
}

/*
====================================================
ProbabilityDensityFunctionMixed::Generate
====================================================
*/
Vec3d ProbabilityDensityFunctionMixed::Generate() const {
	if ( Random::Get() < 0.5f ) {
		return m_pdf0->Generate();
	}
	return m_pdf1->Generate();
}