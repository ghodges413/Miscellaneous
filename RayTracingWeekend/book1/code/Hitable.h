/*
 *  Hitable.h
 *
 */

#pragma once

#include "Vector.h"
#include "Material.h"
#include "Ray.h"
#include "AABB.h"

/*
====================================================
hitRecord_t
====================================================
*/
struct hitRecord_t {
	float t;
	Vec3d point;
	Vec3d normal;
	const Material * material;
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
BoundingVolumeHierarchyNode
====================================================
*/
class BoundingVolumeHierarchyNode : public Hitable {
public:
	BoundingVolumeHierarchyNode() : m_left( NULL ), m_right( NULL ) {}
	BoundingVolumeHierarchyNode( Hitable ** l, int n, float t0, float t1, Random & rnd );
	virtual bool Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const override;
	virtual bool Bounds( float t0, float t1, AABB & aabb ) const override;

	Hitable * m_left;
	Hitable * m_right;
	AABB m_bounds;
};