/*
 *  Hitable.cpp
 *
 */

#include "Hitable.h"

/*
====================================================
HitableSphere::Hit
====================================================
*/
bool HitableSphere::Hit( const Ray & ray, float tMin, float tMax, hitRecord_t & record ) const {
	const Vec3d oc = ray.m_point - center;
	const float a = ray.m_direction.DotProduct( ray.m_direction );
	const float b = oc.DotProduct( ray.m_direction );
	const float c = oc.DotProduct( oc ) - radius * radius;
	const float discriminant = b * b - a * c;
	if ( discriminant < 0.0f ) {
		return false;
	}

	record.material = material;

	float tmp = ( - b - sqrt( discriminant ) ) / a;
	if ( tmp < tMax && tmp > tMin ) {
		record.t = tmp;
		record.point = ray.PositionAtT( record.t );
		record.normal = ( record.point - center ) / radius;
		return true;
	}
	tmp = ( - b + sqrt( discriminant ) ) / a;
	if ( tmp < tMax && tmp > tMin ) {
		record.t = tmp;
		record.point = ray.PositionAtT( record.t );
		record.normal = ( record.point - center ) / radius;
		return true;
	}

	return false;
}

/*
====================================================
HitableSphereDynamic::Hit
====================================================
*/
bool HitableSphereDynamic::Hit( const Ray & ray, float tMin, float tMax, hitRecord_t & record ) const {
	const Vec3d position = center + velocity * ray.m_time;

	const Vec3d oc = ray.m_point - position;
	const float a = ray.m_direction.DotProduct( ray.m_direction );
	const float b = oc.DotProduct( ray.m_direction );
	const float c = oc.DotProduct( oc ) - radius * radius;
	const float discriminant = b * b - a * c;
	if ( discriminant < 0.0f ) {
		return false;
	}

	record.material = material;

	float tmp = ( - b - sqrt( discriminant ) ) / a;
	if ( tmp < tMax && tmp > tMin ) {
		record.t = tmp;
		record.point = ray.PositionAtT( record.t );
		record.normal = ( record.point - position ) / radius;
		return true;
	}
	tmp = ( - b + sqrt( discriminant ) ) / a;
	if ( tmp < tMax && tmp > tMin ) {
		record.t = tmp;
		record.point = ray.PositionAtT( record.t );
		record.normal = ( record.point - position ) / radius;
		return true;
	}

	return false;
}

/*
====================================================
HitablePlane::Hit
====================================================
*/
bool HitablePlane::Hit( const Ray & ray, float tMin, float tMax, hitRecord_t & record ) const {
	// Don't bother with hits from the negative side (for now)
	if ( ray.m_direction.DotProduct( normal ) >= 0 ) {
		return false;
	}

	// Calculate distance from plane
	float distanceFromPlane = normal.DotProduct( ray.m_point - point );
	Vec3d pointOnPlane = ray.m_point - normal * distanceFromPlane;
	float lengthSqr = pointOnPlane.DotProduct( pointOnPlane );
	float hypotenuse = sqrtf( distanceFromPlane * distanceFromPlane + lengthSqr * lengthSqr );

	record.t = hypotenuse;
	record.point = ray.PositionAtT( record.t );
	record.normal = normal;
	record.material = material;
	return true;
}

/*
====================================================
HitableList::Hit
====================================================
*/
bool HitableList::Hit( const Ray & ray, float tMin, float tMax, hitRecord_t & record ) const {
	hitRecord_t tmp;
	bool didHit = false;
	float closestHit = tMax;
	for ( int i = 0; i < num; i++ ) {
		const Hitable * hitable = list[ i ];
		if ( hitable->Hit( ray, tMin, closestHit, tmp ) ) {
			didHit = true;
			closestHit = tmp.t;
			record = tmp;
		}
	}
	return didHit;
}

