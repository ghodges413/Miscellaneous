/*
 *  Camera.h
 *
 */
#pragma once

#include "Vector.h"

class Ray;

/*
====================================================
Camera
====================================================
*/
class Camera {
public:
	Camera( const float fovy, const float aspect, const float focalPlane, const float aperture, const float exposure ) {
		pos = Vec3d( 0, 0, 0 );
		fwd = Vec3d( 1, 0, 0 );
		up = Vec3d( 0, 0, 1 );
		left = Vec3d( 0, 1, 0 );

		m_fovy = fovy;
		m_aspect = aspect;

		m_focalPlane = focalPlane;
		m_aperture = aperture;
		m_exposure = exposure;
	}

	void GetRay( float u, float v, Ray & ray ) const;

	Vec3d pos;
	Vec3d fwd;
	Vec3d up;
	Vec3d left;

	float m_fovy;
	float m_aspect;

	float m_focalPlane;
	float m_aperture;
	float m_exposure;
};
