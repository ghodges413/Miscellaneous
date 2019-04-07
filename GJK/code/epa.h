/*
 *  epa.h
 *
 */
#pragma once
#include "plane.h"

class Vec3d;

float TriangleArea( const Vec3d & a, const Vec3d & b, const Vec3d & c );

float EPA( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, int numB, const point_t * simplex );

