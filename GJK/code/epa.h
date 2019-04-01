/*
 *  epa.h
 *
 */
#pragma once

class Vec3d;

float EPA( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, int numB, const Vec3d * polytope0 );

