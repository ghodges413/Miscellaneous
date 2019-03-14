/*
 *  ProbabilityDensityFunction.h
 *
 */

#pragma once

#include "Vector.h"
#include "OrthonormalBasis.h"

class Hitable;

/*
====================================================
ProbabilityDensityFunction
====================================================
*/
class ProbabilityDensityFunction {
public:
	virtual float Value( const Vec3d & dir ) const = 0;
	virtual Vec3d Generate() const = 0;
};

/*
====================================================
ProbabilityDensityFunctionCosine
====================================================
*/
class ProbabilityDensityFunctionCosine : public ProbabilityDensityFunction {
public:
	ProbabilityDensityFunctionCosine( const Vec3d & uvw );
	virtual float Value( const Vec3d & dir ) const override;
	virtual Vec3d Generate() const override;

	OrthonormalBasis m_onb;
};

/*
====================================================
ProbabilityDensityFunctionHitable
====================================================
*/
class ProbabilityDensityFunctionHitable : public ProbabilityDensityFunction {
public:
	ProbabilityDensityFunctionHitable( const Hitable * p, const Vec3d & origin, const Vec3d & normal ) : m_hitable( p ), m_origin( origin ), m_normal( normal ) {}
	virtual float Value( const Vec3d & dir ) const override;
	virtual Vec3d Generate() const override;

	Vec3d m_normal;
	Vec3d m_origin;
	const Hitable * m_hitable;
};

/*
====================================================
ProbabilityDensityFunctionMixed
====================================================
*/
class ProbabilityDensityFunctionMixed : public ProbabilityDensityFunction {
public:
	ProbabilityDensityFunctionMixed( const ProbabilityDensityFunction * pdf0, const ProbabilityDensityFunction * pdf1 ) {
		m_pdf0 = pdf0;
		m_pdf1 = pdf1;
	}
	virtual float Value( const Vec3d & dir ) const override;
	virtual Vec3d Generate() const override;

	const ProbabilityDensityFunction * m_pdf0;
	const ProbabilityDensityFunction * m_pdf1;
};