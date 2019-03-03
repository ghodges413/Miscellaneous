//
//  main.cpp
//
#include <string>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include "Fileio.h"
#include "Hitable.h"
#include "Random.h"
#include "Material.h"
#include "Texture.h"
#include "Camera.h"
#include "Perlin.h"
#include "Targa.h"

/*
====================================================
ColorBackground
====================================================
*/
Vec3d ColorBackground( Vec3d dir ) {
	dir.Normalize();

	float t = 0.5 * ( dir.z + 1.0f );
	Vec3d color = Vec3d( 1.0f ) * ( 1.0f - t ) + Vec3d( 0.5f, 0.7f, 1.0f ) * t;
	return color;
}

/*
====================================================
ColorWorldMaterial
====================================================
*/
Vec3d ColorWorldMaterial( const Ray & ray, Hitable * world, Random & rnd, int recurssion, const bool isDayTime = true ) {
	if ( recurssion > 32 ) {
		return Vec3d( 0 );
	}

	hitRecord_t record;
	if ( world->Hit( ray, 0.0f, 1000.0f, record ) ) {
		Ray scattered;
		Vec3d attenuation;
		Vec3d emittance = record.material->Emitted( record.point.x, record.point.y, record.point, record.normal );
		if ( NULL != record.material && record.material->Scatter( ray, record, attenuation, scattered, rnd ) ) {
			Vec3d color = ColorWorldMaterial( scattered, world, rnd, recurssion + 1, isDayTime );
			color.x *= attenuation.x;
			color.y *= attenuation.y;
			color.z *= attenuation.z;
			return color + emittance;
		}
		
		return emittance;
	}

	if ( isDayTime ) {
		return ColorBackground( ray.m_direction );
	}
	return Vec3d( 0 );
}

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
	char strBuffer[ 1024 ];

	const int nx = 512;
	const int ny = 256;

	Camera camera( 90, float( nx ) / float( ny ), 5.0f, 1.5f, 1.0f );
	Random random;

	Perlin::Initialize( random );

	Targa targaMars;
	Targa targaEarth;
	targaEarth.Load( "../common/images/earth512.tga", true );
	targaMars.Load( "../common/images/mars512.tga", true );
#if 0
	//
	//	Chapter 1 - motion blur
	//
	{
		if ( !OpenFileWriteStream( "outputImages/motion.ppm" ) ) {
			return -1;
		}

//		camera.pos = Vec3d( 9,-3, 2 );
		camera.pos = Vec3d( 13,-3, 2 );
		Vec3d lookat = Vec3d( 0, 0, 0 );
		camera.fwd = ( lookat - camera.pos );
		camera.fwd.Normalize();
		camera.left = Vec3d( 0, 0, 1 ).Cross( camera.fwd );
		camera.left.Normalize();
		camera.up = camera.fwd.Cross( camera.left );
		camera.up.Normalize();
		camera.m_focalPlane = 10.0f;//( lookat - camera.pos ).GetMagnitude();
		camera.m_aperture = 0;//0.2f;
		camera.m_fovy = 40.0f;

		const int n = 500;
		Hitable ** list = new Hitable* [ n ];
		list[ 0 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, -1000.0f ), 1000.0f, new Lambertian( Vec3d( 0.5f, 0.5f, 0.5f ) ) );
		HitableList * world = NULL;
		{
			int i = 1;
			for ( int a = -11; a < 11; a++ ) {
				for ( int b = -11; b < 11; b++ ) {
					float chooseMat = random.Get();
					Vec3d center( a + 0.9f * random.Get(), b + 0.9f * random.Get(), 0.2f );
					if ( ( center - Vec3d( 4.0f, 0.2f, 0.0f ) ).GetMagnitude() > 0.9f ) {
						Material * material = NULL;
						if ( chooseMat < 0.8f ) {
							material = new Lambertian( Vec3d( random.Get() * random.Get(), random.Get() * random.Get(), random.Get() * random.Get() ) );
						} else if ( chooseMat < 0.95f ) {
							material = new Metal( Vec3d( 0.5f * ( 1.0f + random.Get() ), 0.5f * ( 1.0f + random.Get() ), 0.5f * ( 1.0f + random.Get() ) ), 0.5f * random.Get() );
						} else {
							material = new Dielectric( 1.5f );
						}

						list[ i++ ] = new HitableSphereDynamic( center, Vec3d( 0.0f, 0.0f, random.Get() * 0.5f ), 0.2f, material );
					}
				}
			}

			list[ i++ ] = new HitableSphere( Vec3d( 0, 0, 1 ), 1.0f, new Dielectric( 1.5f ) );
			list[ i++ ] = new HitableSphere( Vec3d( -4, 0, 1 ), 1.0f, new Lambertian( Vec3d( 0.4f, 0.2f, 0.1f ) ) );
			list[ i++ ] = new HitableSphere( Vec3d( 4, 0, 1 ), 1.0f, new Metal( Vec3d( 0.7f, 0.6f, 0.5f ), 0.0f ) );
			world = new HitableList( list, i );
		}

		sprintf( strBuffer, "P3\n%i %i\n255\n", nx, ny );
		WriteFileStream( strBuffer );
		for ( int j = ny - 1; j >= 0; j-- ) {
			for ( int i = 0; i < nx; i++ ) {
				const int ns = 64;//64;
				Vec3d colorSum( 0, 0, 0 );
				for ( int s = 0; s < ns; s++ ) {
					float u = ( ( float( i ) + random.Get() ) / float( nx ) );
					float v = ( ( float( j ) + random.Get() ) / float( ny ) );

					Ray ray;
					camera.GetRay( u, v, ray, random );

					Vec3d color = ColorWorldMaterial( ray, world, random, 0 );
					colorSum += color;
				}
				Vec3d color = colorSum / float( ns );

				// Gamma correct the color
				color = Vec3d( sqrtf( color.x ), sqrtf( color.y ), sqrtf( color.z ) );

				int ir = int( 255.99f * color.x );
				int ig = int( 255.99f * color.y );
				int ib = int( 255.99f * color.z );
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}
#elif 0

	//
	//	Chapter 2/3/4 - BVH, checker texture, and perlin noise
	//
	{
		if ( !OpenFileWriteStream( "outputImages/checker.ppm" ) ) {
			return -1;
		}

		camera.pos = Vec3d( 13,-3, 2 );
		Vec3d lookat = Vec3d( 0, 0, 0 );
		camera.fwd = ( lookat - camera.pos );
		camera.fwd.Normalize();
		camera.left = Vec3d( 0, 0, 1 ).Cross( camera.fwd );
		camera.left.Normalize();
		camera.up = camera.fwd.Cross( camera.left );
		camera.up.Normalize();
		camera.m_focalPlane = 10.0f;
		camera.m_aperture = 0.1f;//0.2f;
		camera.m_fovy = 40.0f;

		const int n = 500;
		Hitable ** list = new Hitable* [ n ];
		list[ 0 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, -1000.0f ), 1000.0f, new Lambertian( new TextureChecker( Vec3d( 0.9f, 0.9f, 0.9f ), Vec3d( 0.2f, 0.3f, 0.1f ) ) ) );
		BoundingVolumeHierarchyNode * world = NULL;
		{
			int i = 1;
			for ( int a = -11; a < 11; a++ ) {
				for ( int b = -11; b < 11; b++ ) {
					float chooseMat = random.Get();
					Vec3d center( a + 0.9f * random.Get(), b + 0.9f * random.Get(), 0.2f );
					if ( ( center - Vec3d( 4.0f, 0.2f, 0.0f ) ).GetMagnitude() > 0.9f ) {
						Material * material = NULL;
						if ( chooseMat < 0.8f ) {
							material = new Lambertian( new TextureConstant( Vec3d( random.Get() * random.Get(), random.Get() * random.Get(), random.Get() * random.Get() ) ) );
						} else if ( chooseMat < 0.95f ) {
							material = new Metal( Vec3d( 0.5f * ( 1.0f + random.Get() ), 0.5f * ( 1.0f + random.Get() ), 0.5f * ( 1.0f + random.Get() ) ), 0.5f * random.Get() );
						} else {
							material = new Dielectric( 1.5f );
						}

						list[ i++ ] = new HitableSphereDynamic( center, Vec3d( 0.0f, 0.0f, random.Get() * 0.5f ), 0.2f, material );
					}
				}
			}

			list[ i++ ] = new HitableSphere( Vec3d( 0, 0, 1 ), 1.0f, new Dielectric( 1.5f ) );
			list[ i++ ] = new HitableSphere( Vec3d( -4, 0, 1 ), 1.0f, new Lambertian( new TextureConstant( Vec3d( 0.4f, 0.2f, 0.1f ) ) ) );
			list[ i++ ] = new HitableSphere( Vec3d( 4, 0, 1 ), 1.0f, new Metal( Vec3d( 0.7f, 0.6f, 0.5f ), 0.0f ) );
			list[ i++ ] = new HitableSphere( Vec3d( 0, -3, 1 ), 1.0f, new Lambertian( new TextureNoise() ) );
			list[ i++ ] = new HitableSphere( Vec3d( 6, 1, 1 ), 1.0f, new Lambertian( new TextureImage( targaEarth.DataPtr(), targaEarth.GetWidth(), targaEarth.GetHeight() ) ) );
			list[ i++ ] = new HitableSphere( Vec3d( 3, -2, 1 ), 1.0f, new Lambertian( new TextureImage( targaMars.DataPtr(), targaMars.GetWidth(), targaMars.GetHeight() ) ) );

			world = new BoundingVolumeHierarchyNode( list, i, 0.0f, 1.0f, random );
		}

		sprintf( strBuffer, "P3\n%i %i\n255\n", nx, ny );
		WriteFileStream( strBuffer );
		for ( int j = ny - 1; j >= 0; j-- ) {
			for ( int i = 0; i < nx; i++ ) {
				const int ns = 64;
				Vec3d colorSum( 0, 0, 0 );
				for ( int s = 0; s < ns; s++ ) {
					float u = ( ( float( i ) + random.Get() ) / float( nx ) );
					float v = ( ( float( j ) + random.Get() ) / float( ny ) );

					Ray ray;
					camera.GetRay( u, v, ray, random );

					Vec3d color = ColorWorldMaterial( ray, world, random, 0 );
					colorSum += color;
				}
				Vec3d color = colorSum / float( ns );

				// Gamma correct the color
				color = Vec3d( sqrtf( color.x ), sqrtf( color.y ), sqrtf( color.z ) );

				int ir = int( 255.99f * color.x );
				int ig = int( 255.99f * color.y );
				int ib = int( 255.99f * color.z );
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}
#elif 0

	//
	//	Chapter 6 - Emittance
	//
	{
		if ( !OpenFileWriteStream( "outputImages/emittance.ppm" ) ) {
			return -1;
		}

		camera.pos = Vec3d( 13,-3, 2 );
		Vec3d lookat = Vec3d( 0, 0, 0 );
		camera.fwd = ( lookat - camera.pos );
		camera.fwd.Normalize();
		camera.left = Vec3d( 0, 0, 1 ).Cross( camera.fwd );
		camera.left.Normalize();
		camera.up = camera.fwd.Cross( camera.left );
		camera.up.Normalize();
		camera.m_focalPlane = 10.0f;
		camera.m_aperture = 0.0f;//0.2f;
		camera.m_fovy = 40.0f;

		const int n = 500;
		Hitable ** list = new Hitable* [ n ];
		list[ 0 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, -1000.0f ), 1000.0f, new Lambertian( new TextureChecker( Vec3d( 0.9f, 0.9f, 0.9f ), Vec3d( 0.2f, 0.3f, 0.1f ) ) ) );
		BoundingVolumeHierarchyNode * world = NULL;
		{
			int i = 1;
			for ( int a = -11; a < 11; a++ ) {
				for ( int b = -11; b < 11; b++ ) {
					float chooseMat = random.Get();
					Vec3d center( a + 0.9f * random.Get(), b + 0.9f * random.Get(), 0.2f );
					if ( ( center - Vec3d( 4.0f, 0.2f, 0.0f ) ).GetMagnitude() > 0.9f ) {
						Material * material = NULL;
						if ( chooseMat < 0.8f ) {
							material = new Lambertian( new TextureConstant( Vec3d( random.Get() * random.Get(), random.Get() * random.Get(), random.Get() * random.Get() ) ) );
						} else if ( chooseMat < 0.95f ) {
							material = new Metal( Vec3d( 0.5f * ( 1.0f + random.Get() ), 0.5f * ( 1.0f + random.Get() ), 0.5f * ( 1.0f + random.Get() ) ), 0.5f * random.Get() );
						} else {
							material = new Dielectric( 1.5f );
						}

						list[ i++ ] = new HitableSphereDynamic( center, Vec3d( 0.0f, 0.0f, random.Get() * 0.5f ), 0.2f, material );
					}
				}
			}

			list[ i++ ] = new HitableSphere( Vec3d( 0, 0, 1 ), 1.0f, new Dielectric( 1.5f ) );
			list[ i++ ] = new HitableSphere( Vec3d( -4, 0, 1 ), 1.0f, new Lambertian( new TextureConstant( Vec3d( 0.4f, 0.2f, 0.1f ) ) ) );
			list[ i++ ] = new HitableSphere( Vec3d( 4, 0, 1 ), 1.0f, new Metal( Vec3d( 0.7f, 0.6f, 0.5f ), 0.0f ) );
			list[ i++ ] = new HitableSphere( Vec3d( 0, -3, 1 ), 1.0f, new Lambertian( new TextureNoise() ) );
			list[ i++ ] = new HitableSphere( Vec3d( 6, 1, 1 ), 1.0f, new Lambertian( new TextureImage( targaEarth.DataPtr(), targaEarth.GetWidth(), targaEarth.GetHeight() ) ) );
			list[ i++ ] = new HitableSphere( Vec3d( 3, -2, 1 ), 1.0f, new MaterialEmittance( new TextureImage( targaMars.DataPtr(), targaMars.GetWidth(), targaMars.GetHeight() ), Vec3d( 1.0f ) ) );

			world = new BoundingVolumeHierarchyNode( list, i, 0.0f, 1.0f, random );
		}

		sprintf( strBuffer, "P3\n%i %i\n255\n", nx, ny );
		WriteFileStream( strBuffer );
		for ( int j = ny - 1; j >= 0; j-- ) {
			for ( int i = 0; i < nx; i++ ) {
				const int ns = 64;
				Vec3d colorSum( 0, 0, 0 );
				for ( int s = 0; s < ns; s++ ) {
					float u = ( ( float( i ) + random.Get() ) / float( nx ) );
					float v = ( ( float( j ) + random.Get() ) / float( ny ) );

					Ray ray;
					camera.GetRay( u, v, ray, random );

					Vec3d color = ColorWorldMaterial( ray, world, random, 0, false );
					colorSum += color;
				}
				Vec3d color = colorSum / float( ns );

				// Gamma correct the color
				color = Vec3d( sqrtf( color.x ), sqrtf( color.y ), sqrtf( color.z ) );

				int ir = int( 255.99f * color.x );
				int ig = int( 255.99f * color.y );
				int ib = int( 255.99f * color.z );
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}
#elif 1
	//
	//	Chapter 6.5 - cornell box
	//
	{
		if ( !OpenFileWriteStream( "outputImages/cornell.ppm" ) ) {
			return -1;
		}

		Vec3d lookat;
// 		camera.pos = Vec3d( 278, 278, -800 );
// 		lookat = Vec3d( 278, 278, 0 );
		camera.pos = Vec3d( 15, 0, 0 );
		lookat = Vec3d( 0 );
		camera.fwd = ( lookat - camera.pos );
		camera.fwd.Normalize();
		camera.left = Vec3d( 0, 0, 1 ).Cross( camera.fwd );
		camera.left.Normalize();
		camera.up = camera.fwd.Cross( camera.left );
		camera.up.Normalize();
		camera.m_focalPlane = 10.0f;
		camera.m_aperture = 0.0f;//0.2f;
		camera.m_fovy = 40.0f;

		const int n = 500;
		Hitable ** list = new Hitable* [ n ];
		HitableList * world = NULL;

		int i = 0;
		Material * matRed = new Lambertian( new TextureConstant( Vec3d( 1, 0, 0 ) ) );
		Material * matWhite = new Lambertian( new TextureConstant( Vec3d( 1, 1, 1 ) ) );
		Material * matGreen = new Lambertian( new TextureConstant( Vec3d( 0, 1, 0 ) ) );
		Material * matBlue = new Lambertian( new TextureConstant( Vec3d( 0, 0, 1 ) ) );
		Material * matLight = new MaterialEmittance( NULL, Vec3d( 10.0f ) );

		const float s = 2;
		const float sl = s * 0.2f;
 		list[ i++ ] = new HitableRectYZ( -s, s, -s, s, -s, matBlue );
		list[ i++ ] = new HitableRectXY( -sl, sl, -sl, sl, s - 0.01f, matLight );
 		list[ i++ ] = new HitableRectXY( -s, s, -s, s, s, matWhite );
		list[ i++ ] = new HitableRectXY( -s, s, -s, s, -s, matWhite );
		list[ i++ ] = new HitableRectXZ( -s, s, -s, s, -s, matRed );
		list[ i++ ] = new HitableRectXZ( -s, s, -s, s, s, matGreen );

// 		list[ i++ ] = new HitableSphere( Vec3d( 0, 0, 1 ), 0.5f, new MaterialEmittance( new TextureImage( targaMars.DataPtr(), targaMars.GetWidth(), targaMars.GetHeight() ), Vec3d( 1.0f ) ) );
// 		list[ i++ ] = new HitableSphere( Vec3d( 0, 0, 0 ), 0.5f, new Metal( Vec3d( 0.7f, 0.6f, 0.5f ), 0.0f ) );
		
		world = new HitableList( list, i );

		sprintf( strBuffer, "P3\n%i %i\n255\n", nx, ny );
		WriteFileStream( strBuffer );
		for ( int j = ny - 1; j >= 0; j-- ) {
			for ( int i = 0; i < nx; i++ ) {
				const int ns = 64;
				Vec3d colorSum( 0, 0, 0 );
				for ( int s = 0; s < ns; s++ ) {
					float u = ( ( float( i ) + random.Get() ) / float( nx ) );
					float v = ( ( float( j ) + random.Get() ) / float( ny ) );

					Ray ray;
					camera.GetRay( u, v, ray, random );

					Vec3d color = ColorWorldMaterial( ray, world, random, 0, false );
					colorSum += color;
				}
				Vec3d color = colorSum / float( ns );

				// Gamma correct the color
				color = Vec3d( sqrtf( color.x ), sqrtf( color.y ), sqrtf( color.z ) );

				int ir = int( 255.99f * color.x );
				int ig = int( 255.99f * color.y );
				int ib = int( 255.99f * color.z );
				ir = std::min< int >( 255, ir );
				ig = std::min< int >( 255, ig );
				ib = std::min< int >( 255, ib );
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}
#endif

	return 0;
}


