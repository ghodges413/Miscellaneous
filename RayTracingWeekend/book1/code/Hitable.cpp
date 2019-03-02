/*
 *  Hitable.cpp
 *
 */

#include "Hitable.h"
#include "Random.h"
#include <stdlib.h>

/*
========================================================================================================

HitableSphere

========================================================================================================
*/

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
HitableSphere::Bounds
====================================================
*/
bool HitableSphere::Bounds( float t0, float t1, AABB & aabb ) const {
	aabb.m_min = center - Vec3d( radius );
	aabb.m_max = center + Vec3d( radius );
	assert( aabb.IsValid() );
	return true;
}

/*
========================================================================================================

HitableSphereDynamic

========================================================================================================
*/

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
HitableSphereDynamic::Bounds
====================================================
*/
bool HitableSphereDynamic::Bounds( float t0, float t1, AABB & aabb ) const {
	//const float dt = t1 - t0;

	Vec3d ptA = center + velocity * t0;
	Vec3d ptB = center + velocity * t1;

	AABB boundsA = AABB( ptA - Vec3d( radius ), ptA + Vec3d( radius ) );
	AABB boundsB = AABB( ptB - Vec3d( radius ), ptB + Vec3d( radius ) );

	aabb = AABB( boundsA, boundsB );
	assert( aabb.IsValid() );

// 	aabb.m_min = center - Vec3d( radius );
// 	aabb.m_max = center + Vec3d( radius );
	return true;
}

/*
========================================================================================================

HitablePlane

========================================================================================================
*/

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
HitablePlane::Bounds
====================================================
*/
bool HitablePlane::Bounds( float t0, float t1, AABB & aabb ) const {
// 	aabb.m_min = center - Vec3d( radius );
// 	aabb.m_max = center + Vec3d( radius );
	return false;
}

/*
========================================================================================================

HitableList

========================================================================================================
*/

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

/*
====================================================
HitableList::Bounds
====================================================
*/
bool HitableList::Bounds( float t0, float t1, AABB & bounds ) const {
	if ( num <= 0 ) {
		return false;
	}

	AABB tmp;
	bool firstTrue = list[ 0 ]->Bounds( t0, t1, tmp );
	if ( !firstTrue ) {
		return false;
	} else {
		bounds = tmp;
	}

	for ( int i = 1; i < num; i++ ) {
		if ( list[ 0 ]->Bounds( t0, t1, tmp ) ) {
			bounds = AABB( bounds, tmp );
		} else {
			return false;
		}
	}

	assert( bounds.IsValid() );
	return true;
}

/*
========================================================================================================

BoundingVolumeHierarchyNode

========================================================================================================
*/

/*
====================================================
BoundsCompareX
====================================================
*/
int BoundsCompareX( const void * a, const void * b ) {
	AABB boundsA;
	AABB boundsB;

	Hitable * ah = *(Hitable**)a;
	Hitable * bh = *(Hitable**)b;

	if ( !ah->Bounds( 0, 0, boundsA ) || !bh->Bounds( 0, 0, boundsB ) ) {
		printf( "no bounding box in bvhnode constructor\n" );
		assert( 0 );
	}

//	if ( ( boundsA.m_min.x - boundsB.m_min.x ) < 0.0f ) {
	if ( boundsA.m_min.x < boundsB.m_min.x ) {
		return -1;
	}

	return 1;
}
int BoundsCompareY( const void * a, const void * b ) {
	AABB boundsA;
	AABB boundsB;

	Hitable * ah = *(Hitable**)a;
	Hitable * bh = *(Hitable**)b;

	if ( !ah->Bounds( 0, 0, boundsA ) || !bh->Bounds( 0, 0, boundsB ) ) {
		printf( "no bounding box in bvhnode constructor\n" );
		assert( 0 );
	}

//	if ( ( boundsA.m_min.y - boundsB.m_min.y ) < 0.0f ) {
	if ( boundsA.m_min.y < boundsB.m_min.y ) {
		return -1;
	}

	return 1;
}
int BoundsCompareZ( const void * a, const void * b ) {
	AABB boundsA;
	AABB boundsB;

	Hitable * ah = *(Hitable**)a;
	Hitable * bh = *(Hitable**)b;

	if ( !ah->Bounds( 0, 0, boundsA ) || !bh->Bounds( 0, 0, boundsB ) ) {
		printf( "no bounding box in bvhnode constructor\n" );
		assert( 0 );
	}

//	if ( ( boundsA.m_min.z - boundsB.m_min.z ) < 0.0f ) {
	if ( boundsA.m_min.z < boundsB.m_min.z ) {
		return -1;
	}

	return 1;
}

/*
====================================================
BoundingVolumeHierarchyNode::BoundingVolumeHierarchyNode
====================================================
*/
BoundingVolumeHierarchyNode::BoundingVolumeHierarchyNode( Hitable ** l, int n, float t0, float t1, Random & rnd ) {
	const int axis = int( 3.0f * rnd.Get() );
	switch ( axis ) {
		default:
		case 0: { qsort( l, n, sizeof( Hitable * ), BoundsCompareX ); } break;
		case 1: { qsort( l, n, sizeof( Hitable * ), BoundsCompareY ); } break;
		case 2: { qsort( l, n, sizeof( Hitable * ), BoundsCompareZ ); } break;
	};

	if ( 1 == n ) {
		m_left = m_right = l[ 0 ];
	} else if ( 2 == n ) {
		m_left = l[ 0 ];
		m_right = l[ 1 ];
	} else {
		m_left = new BoundingVolumeHierarchyNode( l, n / 2, t0, t1, rnd );
		m_right = new BoundingVolumeHierarchyNode( l + ( n / 2 ), n - ( n / 2 ), t0, t1, rnd );
	}

	AABB boundsLeft;
	AABB boundsRight;

	const bool didBuildLeft = m_left->Bounds( t0, t1, boundsLeft );
	const bool didBuildRight = m_right->Bounds( t0, t1, boundsRight );

	if ( !didBuildLeft || !didBuildRight ) {
		printf( "no bounding box in bvhnode constructor\n" );
		assert( 0 );
	}
	assert( boundsLeft.IsValid() );
	assert( boundsRight.IsValid() );

	m_bounds = AABB( boundsLeft, boundsRight );
	assert( m_bounds.IsValid() );
}

/*
====================================================
BoundingVolumeHierarchyNode::Hit
====================================================
*/
bool BoundingVolumeHierarchyNode::Hit( const Ray & ray, float tMin, float tMax, hitRecord_t & record ) const {
	assert( m_bounds.IsValid() );
	if ( !m_bounds.Hit( ray, tMin, tMax ) ) {
		record.t = 100000000;	// set it be huge because fuck it, we hit jack shit
		return false;
	}

	hitRecord_t leftrec;
	hitRecord_t rightrec;

	const bool didHitLeft = m_left->Hit( ray, tMin, tMax, leftrec );
	const bool didHitRight = m_right->Hit( ray, tMin, tMax, rightrec );

	if ( didHitLeft && didHitRight ) {
#if 1
		if ( leftrec.t < rightrec.t ) {
			record = leftrec;
		} else {
			record = rightrec;
		}
#else
		if ( rightrec.t < leftrec.t ) {
			record = rightrec;
		} else {
			record = leftrec;
		}
#endif
		return true;
	} else if ( didHitLeft ) {
		record = leftrec;
		return true;
	} else if ( didHitRight ) {
		record = rightrec;
		return true;
	}

	return false;
}

/*
====================================================
BoundingVolumeHierarchy::Bounds
====================================================
*/
bool BoundingVolumeHierarchyNode::Bounds( float t0, float t1, AABB & bounds ) const {
	bounds = m_bounds;
	return true;
}
