/*
 *  epa.h
 *
 */
#pragma once

class Vec3d;

float EPA2( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, int numB, const Vec3d * polytope0 );
float EPA3( const Vec3d * ptsA, const int numA, const Vec3d * ptsB, int numB, const Vec3d * polytope0 );

