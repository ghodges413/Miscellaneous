/*
 *  gjk.h
 *
 */
#pragma once
#include "Vector.h"
#include "plane.h"

point_t Support( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, Vec3d dir );

bool GJK( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, point_t * simplexPoints );