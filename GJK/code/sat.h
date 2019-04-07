/*
 *  sat.h
 *
 */
#pragma once
#include "Vector.h"
#include "plane.h"
#include <vector>

bool SeparatingAxisTheorem( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, const int numB, point_t * simplexPoints, Vec3d & mtv );