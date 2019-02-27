/*
 *  Camera.cpp
 *
 */

#include "Camera.h"
#include "Ray.h"
#include "Random.h"

/*
====================================================
Camera::GetRay

Assumes that u,v are in the range [0,1).
This simulates a pinhole camera.
====================================================
*/
void Camera::GetRay( float u, float v, Ray & ray ) const {
	u -= 0.5f;
	v -= 0.5f;

	float fovRadians = m_fovy * 3.1415f / 180.0f;
	float halfAngle = 0.5f * fovRadians;
	float scaleScreenY = tanf( halfAngle );
	float scaleScreenX = scaleScreenY * m_aspect;

	Vec3d ptA = pos;
	Vec3d ptB = pos + fwd + left * -u * scaleScreenX + up * v * scaleScreenY;

	ray = Ray( ptA, ptB );
}

/*
====================================================
Camera::GetRay

Assumes that u,v are in the range [0,1).
This defocuses the image based upon the focal plane and aperture size.
====================================================
*/
void Camera::GetRay( float u, float v, Ray & ray, Random & rnd ) const {
	u -= 0.5f;
	v -= 0.5f;

	float fovRadians = m_fovy * 3.1415f / 180.0f;
	float halfAngle = 0.5f * fovRadians;
	float scaleScreenY = tanf( halfAngle );
	float scaleScreenX = scaleScreenY * m_aspect;

	Vec3d ptA = pos + rnd.RandomInUnitSphere() * m_aperture * 0.5f;
	Vec3d ptB = pos + ( fwd + left * -u * scaleScreenX + up * v * scaleScreenY ) * m_focalPlane;

	ray = Ray( ptA, ptB );
}

