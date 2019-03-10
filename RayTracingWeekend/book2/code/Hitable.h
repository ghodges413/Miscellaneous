/*
 *  Hitable.h
 *
 */

#pragma once

#include "Vector.h"
#include "Material.h"
#include "Ray.h"
#include "AABB.h"
#include "Matrix.h"

/*
====================================================
hitRecord_t
====================================================
*/
struct hitRecord_t {
	float u;
	float v;
	float t;
	Vec3d point;
	Vec3d normal;
	const Material * material;

	hitRecord_t() {
		t = 1000000;
		material = NULL;
	}
};

/*
====================================================
Hitable
====================================================
*/
class Hitable {
public:
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const = 0;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const = 0;
};

/*
====================================================
HitableSphere
====================================================
*/
class HitableSphere : public Hitable {
public:
	HitableSphere() { material = NULL; }
	HitableSphere( const Vec3d & pt, const float r ) : center( pt ), radius( r ) { material = NULL; }
	HitableSphere( const Vec3d & pt, const float r, const Material * mat ) : center( pt ), radius( r ), material( mat ) {}
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override;

	Vec3d center;
	float radius;
	const Material * material;
};

/*
====================================================
HitableSphereDynamic
====================================================
*/
class HitableSphereDynamic : public Hitable {
public:
	HitableSphereDynamic() { material = NULL; }
	HitableSphereDynamic( const Vec3d & pt, const Vec3d & vel, const float r ) : center( pt ), velocity( vel ), radius( r ) { material = NULL; }
	HitableSphereDynamic( const Vec3d & pt, const Vec3d & vel, const float r, const Material * mat ) : center( pt ), velocity( vel ), radius( r ), material( mat ) {}
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override;

	Vec3d center;
	float radius;
	const Material * material;

	Vec3d velocity;
};

/*
====================================================
HitablePlane
====================================================
*/
class HitablePlane : public Hitable {
public:
	HitablePlane() { material = NULL; }
	HitablePlane( const Vec3d & pt, const Vec3d & norm ) : point( pt ), normal( norm ) { material = NULL; }
	HitablePlane( const Vec3d & pt, const Vec3d & norm, const Material * mat ) : point( pt ), normal( norm ), material( mat ) {}
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override;

	Vec3d point;
	Vec3d normal;
	const Material * material;
};

/*
====================================================
HitableRectXY
====================================================
*/
class HitableRectXY : public Hitable {
public:
	HitableRectXY() { m_material = NULL; }
	HitableRectXY( float _x0, float _x1, float _y0, float _y1, float _k, const Material * mat ) : x0( _x0 ), x1( _x1 ), y0( _y0 ), y1( _y1 ), k( _k ), m_material( mat ) {}
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override {
		aabb = AABB( Vec3d( x0, y0, k - 0.01f ), Vec3d( x1, y1, k + 0.01f ) );
		return true;
	}


	float x0, x1, y0, y1, k;
	const Material * m_material;
};

/*
====================================================
HitableRectXZ
====================================================
*/
class HitableRectXZ : public Hitable {
public:
	HitableRectXZ() { m_material = NULL; }
	HitableRectXZ( float _x0, float _x1, float _z0, float _z1, float _k, const Material * mat ) : x0( _x0 ), x1( _x1 ), z0( _z0 ), z1( _z1 ), k( _k ), m_material( mat ) {}
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override {
		aabb = AABB( Vec3d( x0, k - 0.01f, z0 ), Vec3d( x1, k + 0.01f, z1 ) );
		return true;
	}


	float x0, x1, z0, z1, k;
	const Material * m_material;
};

/*
====================================================
HitableRectYZ
====================================================
*/
class HitableRectYZ : public Hitable {
public:
	HitableRectYZ() { m_material = NULL; }
	HitableRectYZ( float _y0, float _y1, float _z0, float _z1, float _k, const Material * mat ) : y0( _y0 ), y1( _y1 ), z0( _z0 ), z1( _z1 ), k( _k ), m_material( mat ) {}
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override {
		aabb = AABB( Vec3d( k - 0.01f, y0, z0 ), Vec3d( k + 0.01f, y1, z1 ) );
		return true;
	}

	float y0, y1, z0, z1, k;
	const Material * m_material;
};

/*
====================================================
HitableList
====================================================
*/
class HitableList : public Hitable {
public:
	HitableList() {}
	HitableList( Hitable ** l, int n ) { list = l; num = n; }
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override;

	Hitable ** list;
	int num;
};

/*
====================================================
HitableBox
====================================================
*/
class HitableBox : public Hitable {
public:
	HitableBox() {}
	HitableBox( const AABB & bounds, const Material * material );
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override {
		aabb = m_bounds;
		return true;
	}

	AABB m_bounds;
	const Material * m_material;
	HitableList * m_list;
};

/*
====================================================
HitableInstance
====================================================
*/
class HitableInstance : public Hitable {
public:
	HitableInstance() {}
	HitableInstance( const Vec3d & offset, const Matrix & orient, const Hitable * instance ) : m_offset( offset ), m_orient( orient ), m_instance( instance ) {}
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override;

	const Hitable * m_instance;
	Vec3d m_offset;
	Matrix m_orient;
};

/*
====================================================
HitableMediumConstant
====================================================
*/
class HitableMediumConstant : public Hitable {
public:
	HitableMediumConstant() {}
	HitableMediumConstant( float density, const Hitable * instance, const Material * phaseFunction ) : m_density( density ), m_boundary( instance ), m_phaseFunction( phaseFunction ) {}
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override {
		if ( NULL == m_boundary || !m_boundary->Bounds( t0, t1, aabb ) ) {
			return false;
		}
		return true;
	}

	float m_density;
	const Hitable * m_boundary;
	const Material * m_phaseFunction;
};

/*
====================================================
BoundingVolumeHierarchyNode
====================================================
*/
class BoundingVolumeHierarchyNode : public Hitable {
public:
	BoundingVolumeHierarchyNode() : m_left( NULL ), m_right( NULL ) {}
	BoundingVolumeHierarchyNode( Hitable ** l, int n, float t0, float t1 );
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override;

	Hitable * m_left;
	Hitable * m_right;
	AABB m_bounds;
};