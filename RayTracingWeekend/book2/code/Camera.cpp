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
This defocuses the image based upon the focal plane and aperture size.
====================================================
*/
void Camera::GetRay( float u, float v, Ray & ray ) const {
	u -= 0.5f;
	v -= 0.5f;

	float fovRadians = m_fovy * 3.1415f / 180.0f;
	float halfAngle = 0.5f * fovRadians;
	float scaleScreenY = tanf( halfAngle );
	float scaleScreenX = scaleScreenY * m_aspect;

	Vec3d ptA = pos + Random::RandomInUnitSphere() * m_aperture * 0.5f;
	Vec3d ptB = pos + ( fwd + left * -u * scaleScreenX + up * v * scaleScreenY ) * m_focalPlane;

	float time = Random::Get() * m_exposure;

	ray = Ray( ptA, ptB, time );
}

