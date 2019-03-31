/*
 *  gjk.h
 *
 */
#pragma once
#include "Vector.h"

//class Vec3d;

Vec3d Support( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, Vec3d dir );

bool GJK( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, Vec3d * simplexPoints );