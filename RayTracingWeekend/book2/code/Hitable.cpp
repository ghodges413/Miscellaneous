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

HitableSquare

========================================================================================================
*/


bool HitableRectXY::Hit( const Ray & ray, float tMin, float tMax, hitRecord_t & record ) const {
	if ( 0 != norm ) {
		if ( ray.m_direction.z * norm > 0 ) {
			return false;
		}
	}

	float t = ( k - ray.m_point.z ) / ray.m_direction.z;
	if ( t < tMin || t > tMax ) {
		return false;
	}

	float x = ray.m_point.x + t * ray.m_direction.x;
	float y = ray.m_point.y + t * ray.m_direction.y;
	if ( x < x0 || x > x1 || y < y0 || y > y1 ) {
		return false;
	}

// 	record.u = ( x - x0 ) / ( x1 - x0 );
// 	record.v = ( y - y0 ) / ( y1 - y0 );
	record.t = t;
	record.material = m_material;
	record.point = ray.PositionAtT( t );
	record.normal = Vec3d( 0, 0, 1 );
	if ( ray.m_point.z < k ) {
		record.normal = Vec3d( 0, 0, -1 );
	}
	record.point += record.normal * 0.001f;
	return true;
}

bool HitableRectXZ::Hit( const Ray & ray, float tMin, float tMax, hitRecord_t & record ) const {
	float t = ( k - ray.m_point.y ) / ray.m_direction.y;
	if ( t < tMin || t > tMax ) {
		return false;
	}

	float x = ray.m_point.x + t * ray.m_direction.x;
	float z = ray.m_point.z + t * ray.m_direction.z;
	if ( x < x0 || x > x1 || z < z0 || z > z1 ) {
		return false;
	}

// 	record.u = ( x - x0 ) / ( x1 - x0 );
// 	record.v = ( z - z0 ) / ( z1 - z0 );
	record.t = t;
	record.material = m_material;
	record.point = ray.PositionAtT( t );
	record.normal = Vec3d( 0, 1, 0 );
	if ( ray.m_point.y < k ) {
		record.normal = Vec3d( 0, -1, 0 );
	}
	record.point += record.normal * 0.001f;
	return true;
}

bool HitableRectYZ::Hit( const Ray & ray, float tMin, float tMax, hitRecord_t & record ) const {
	float t = ( k - ray.m_point.x ) / ray.m_direction.x;
	if ( t < tMin || t > tMax ) {
		return false;
	}

	float y = ray.m_point.y + t * ray.m_direction.y;
	float z = ray.m_point.z + t * ray.m_direction.z;
	if ( y < y0 || y > y1 || z < z0 || z > z1 ) {
		return false;
	}

// 	record.u = ( y - y0 ) / ( y1 - y0 );
// 	record.v = ( z - z0 ) / ( z1 - z0 );
	record.t = t;
	record.material = m_material;
	record.point = ray.PositionAtT( t );
	record.normal = Vec3d( 1, 0, 0 );
	if ( ray.m_point.x < k ) {
		record.normal = Vec3d( -1, 0, 0 );
	}
	record.point += record.normal * 0.001f;
	return true;
}

/*
========================================================================================================

HitableBox

========================================================================================================
*/

/*
====================================================
HitableBox::HitableBox
====================================================
*/
HitableBox::HitableBox( const AABB & bounds, const Material * material ) : m_bounds( bounds ), m_material( material ) {
	Hitable ** walls = new Hitable*[ 6 ];
	walls[ 0 ] = new HitableRectXY( m_bounds.m_min.x, m_bounds.m_max.x, m_bounds.m_min.y, m_bounds.m_max.y, m_bounds.m_min.z, m_material );
	walls[ 1 ] = new HitableRectXY( m_bounds.m_min.x, m_bounds.m_max.x, m_bounds.m_min.y, m_bounds.m_max.y, m_bounds.m_max.z, m_material );

	walls[ 2 ] = new HitableRectXZ( m_bounds.m_min.x, m_bounds.m_max.x, m_bounds.m_min.z, m_bounds.m_max.z, m_bounds.m_min.y, m_material );
	walls[ 3 ] = new HitableRectXZ( m_bounds.m_min.x, m_bounds.m_max.x, m_bounds.m_min.z, m_bounds.m_max.z, m_bounds.m_max.y, m_material );

	walls[ 4 ] = new HitableRectYZ( m_bounds.m_min.y, m_bounds.m_max.y, m_bounds.m_min.z, m_bounds.m_max.z, m_bounds.m_min.x, m_material );
	walls[ 5 ] = new HitableRectYZ( m_bounds.m_min.y, m_bounds.m_max.y, m_bounds.m_min.z, m_bounds.m_max.z, m_bounds.m_max.x, m_material );

	m_list = new HitableList( walls, 6 );
}

/*
====================================================
HitableBox::Hit
====================================================
*/
bool HitableBox::Hit( const Ray & ray, float tMin, float tMax, hitRecord_t & record ) const {
	return m_list->Hit( ray, tMin, tMax, record );
}

/*
========================================================================================================

HitableInstance

========================================================================================================
*/

/*
====================================================
HitableInstance::Hit
====================================================
*/
bool HitableInstance::Hit( const Ray & r, float tMin, float tMax, hitRecord_t & record ) const {
	Ray ray = r;
	ray.m_point -= m_offset;
	Matrix transposed = m_orient;
	transposed.Transpose();
	ray.m_point = transposed * ray.m_point;
	ray.m_direction = transposed * ray.m_direction;


	if ( NULL == m_instance || !m_instance->Hit( ray, tMin, tMax, record ) ) {
		return false;
	}

	record.normal = m_orient * record.normal;
	record.point = m_orient * record.point;
	record.point += m_offset;
	return true;
}

/*
====================================================
HitableInstance::Bounds
====================================================
*/
bool HitableInstance::Bounds( float t0, float t1, AABB & aabb ) const {
	if ( NULL == m_instance || !m_instance->Bounds( t0, t1, aabb ) ) {
		return false;
	}

	// TODO: Make eight points from the aabb, rotate and translate each of them;
	Vec3d pts[ 8 ];
	pts[ 0 ] = Vec3d( aabb.m_min.x, aabb.m_min.y, aabb.m_min.z );
	pts[ 1 ] = Vec3d( aabb.m_max.x, aabb.m_min.y, aabb.m_min.z );
	pts[ 2 ] = Vec3d( aabb.m_min.x, aabb.m_max.y, aabb.m_min.z );
	pts[ 3 ] = Vec3d( aabb.m_min.x, aabb.m_min.y, aabb.m_max.z );

	pts[ 4 ] = Vec3d( aabb.m_max.x, aabb.m_max.y, aabb.m_max.z );
	pts[ 5 ] = Vec3d( aabb.m_min.x, aabb.m_max.y, aabb.m_max.z );
	pts[ 6 ] = Vec3d( aabb.m_max.x, aabb.m_min.y, aabb.m_max.z );
	pts[ 7 ] = Vec3d( aabb.m_max.x, aabb.m_max.y, aabb.m_min.z );

	aabb.m_min += m_offset;
	aabb.m_max += m_offset;

	for ( int i = 0; i < 8; i++ ) {
		pts[ i ] = m_orient * pts[ i ];
		pts[ i ] += m_offset;
		aabb.Expand( pts[ i ] );
	}

	return true;
}

/*
========================================================================================================

HitableMediumConstant

========================================================================================================
*/

/*
====================================================
HitableMediumConstant::Hit
====================================================
*/
bool HitableMediumConstant::Hit( const Ray & ray, float tMin, float tMax, hitRecord_t & record ) const {
	hitRecord_t rec1;
	hitRecord_t rec2;

	if ( !m_boundary->Hit( ray, -FLT_MAX, FLT_MAX, rec1 ) ) {
		return false;
	}

	if ( !m_boundary->Hit( ray, rec1.t + 0.0001f, FLT_MAX, rec2 ) ) {
		return false;
	}

	if ( rec1.t < tMin ) {
		rec1.t = tMin;
	}
	if ( rec2.t > tMax ) {
		rec2.t = tMax;
	}
	if ( rec1.t >= rec2.t ) {
		return false;
	}

	if ( rec1.t < 0 ) {
		rec1.t = 0;
	}

	float distInsideBoundary = ( rec2.t - rec1.t ) * ray.m_direction.GetMagnitude();
	float hitDist = -( 1.0f / m_density ) * logf( Random::Get() );
	if ( hitDist >= distInsideBoundary ) {
		return false;
	}

	record.t = rec1.t + hitDist / ray.m_direction.GetMagnitude();
	record.point = ray.PositionAtT( record.t );
	record.normal = Vec3d( 1, 0, 0 ); // arbitrary
	record.material = m_phaseFunction;
	return true;
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
BoundingVolumeHierarchyNode::BoundingVolumeHierarchyNode( Hitable ** l, int n, float t0, float t1 ) {
	const int axis = int( 3.0f * Random::Get() );
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
		m_left = new BoundingVolumeHierarchyNode( l, n / 2, t0, t1 );
		m_right = new BoundingVolumeHierarchyNode( l + ( n / 2 ), n - ( n / 2 ), t0, t1 );
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
