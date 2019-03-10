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
CornellBox
====================================================
*/
int CornellBox( BoundingVolumeHierarchyNode ** world ) {
	const int n = 10;
	Hitable ** list = new Hitable* [ n ];
	//HitableList * world = NULL;

	int i = 0;
	Material * matRed = new Lambertian( new TextureConstant( Vec3d( 1, 0, 0 ) ) );
	Material * matWhite = new Lambertian( new TextureConstant( Vec3d( 1, 1, 1 ) ) );
	Material * matGreen = new Lambertian( new TextureConstant( Vec3d( 0, 1, 0 ) ) );
	Material * matBlue = new Lambertian( new TextureConstant( Vec3d( 0, 0, 1 ) ) );
	Material * matLight = new MaterialEmittance( NULL, Vec3d( 10.0f ) );
	Material * matMetal = new Metal( Vec3d( 0.7f, 0.6f, 0.5f ), 0.0f );
	Material * matGlass = new Dielectric( 1.5f );
	//matRed = matWhite = matGreen = matBlue = matLight;

	const float s = 2;
	const float sl = s * 0.2f;
	list[ i++ ] = new HitableRectYZ( -s, s, -s, s, -s, matBlue );
	HitableRectXY * light = new HitableRectXY( -sl, sl, -sl, sl, s - 0.01f, matLight );
	light->norm = -1;
	list[ i++ ] = light;
	list[ i++ ] = new HitableRectXY( -s, s, -s, s, s, matWhite );
	list[ i++ ] = new HitableRectXY( -s, s, -s, s, -s, matWhite );
	list[ i++ ] = new HitableRectXZ( -s, s, -s, s, -s, matRed );
	list[ i++ ] = new HitableRectXZ( -s, s, -s, s, s, matGreen );

	HitableBox * boxA = new HitableBox( AABB( Vec3d( -0.5f, -0.5f, 0.0f ), Vec3d( 0.5f, 0.5f, 2 ) ), matWhite );
	HitableBox * boxB = new HitableBox( AABB( Vec3d( -0.5f, -0.5f, 0.0f ), Vec3d( 0.5f, 0.5f, 1 ) ), matWhite );

	list[ i++ ] = new HitableInstance( Vec3d( -0.75f, -0.75f, -2 ), Matrix::RotationMatrix( Vec3d( 0, 0, 1 ), 20 ), boxA );
	list[ i++ ] = new HitableInstance( Vec3d( 0.75f, 0.75f, -2 ), Matrix::RotationMatrix( Vec3d( 0, 0, 1 ), -20 ), boxB );

// 	list[ i++ ] = new HitableSphere( Vec3d( 0.75f, -0.75f, -1.5f ), 0.5f, matMetal );
// 	list[ i++ ] = new HitableSphere( Vec3d( 0.75f, 0.75f, -0.5f ), 0.5f, matGlass );

	//*world = new HitableList( list, i );
	*world = new BoundingVolumeHierarchyNode( list, i, 0.0f, 1.0f );
	return i;
}

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
Vec3d ColorWorldMaterial( const Ray & ray, Hitable * world, int recurssion, const bool isDayTime = true ) {
	if ( recurssion > 32 ) {
		return Vec3d( 0 );
	}

	hitRecord_t record;
	if ( world->Hit( ray, 0.0f, 10000.0f, record ) ) {
		Ray scattered;
		Vec3d emittance = record.material->Emitted( record.point.x, record.point.y, record.point, record.normal );
		float pdf;
		Vec3d albedo;
		if ( record.material->Scatter( ray, record, albedo, scattered, pdf ) ) {
			Vec3d attenuation = albedo * record.material->ScatteringPDF( ray, record, scattered ) / pdf;
			Vec3d color = ColorWorldMaterial( scattered, world, recurssion + 1, isDayTime );
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

	const int nx = 512;//384 * 2;//512;
	const int ny = 512;//256 * 2;

 	Camera camera( 90, float( nx ) / float( ny ), 5.0f, 1.5f, 1.0f );
 
// 	Perlin::Initialize( random );
// 
// 	Targa targaMars;
// 	Targa targaEarth;
// 	targaEarth.Load( "../common/images/earth512.tga", true );
// 	targaMars.Load( "../common/images/mars512.tga", true );

#if 0
	//
	//	Chapter 1 - Monte Carlo Pi (How many times will we do this?)
	//
	{
		int numInside = 0;
		int numInsideStratified = 0;
		int rootN = 10000;
		for ( int i = 0; i < rootN; i++ ) {
			for ( int j = 0; j < rootN; j++ ) {
				float x = 2.0f * Random::Get() - 1.0f;
				float y = 2.0f * Random::Get() - 1.0f;
				if ( ( x * x + y * y ) < 1.0f ) {
					numInside++;
				}

				x = 2.0f * ( ( (float)i + Random::Get() ) / rootN ) - 1.0f;
				y = 2.0f * ( ( (float)j + Random::Get() ) / rootN ) - 1.0f;
				if ( ( x * x + y * y ) < 1.0f ) {
					numInsideStratified++;
				}
			}
		}

		float pi = 4.0f * (float)numInside / (float)( rootN * rootN );
		float pis = 4.0f * (float)numInsideStratified / (float)( rootN * rootN );
		printf( "Pi = %f\n", pi );
		printf( "Pi stratified = %f\n", pis );
		printf( "-------------------------------\n" );
	}
#elif 0
	//
	//	Chapter 2 - Monte Carlo with probability density functions (importance sampling)
	//
	{
		auto lambdaPDF = []( float x ) {
			return 0.5f;
		};

		int N = 10000;
		float sum = 0;
		for ( int i = 0; i < N; i++ ) {
			float x = 2.0f * Random::Get();
			sum += x * x / lambdaPDF( x );
		}
		float integrand = sum / (float)N;
		printf( "I = %f\n", integrand );


		auto lambdaPDF2 = []( float x ) {
			return 3.0f * x * x / 8.0f;
		};

		N = 1;
		sum = 0;
		for ( int i = 0; i < N; i++ ) {
			float x = powf( 8.0f * Random::Get(), 1.0f / 3.0f );
			sum += x * x / lambdaPDF2( x );
		}
		integrand = sum / (float)N;
		printf( "I = %f\n", integrand );


		auto lambdaPDF3 = []( const Vec3d & p ) {
			const float pi = acosf( -1.0f );
			return ( 1.0f / ( 4.0f * pi ) );
		};

		N = 10000;
		sum = 0.0f;
		for ( int i = 0; i < N; i++ ) {
			Vec3d p = Random::RandomOnSphereSurface();
			float cosSquared = p.z * p.z;
			sum += cosSquared / lambdaPDF3( p );
		}
		const float integral = sum / (float)N;
		printf( "I = %f\n", integral );
		printf( "-----------------------\n" );
	}
#elif 1

	//
	//	Chapter  - Monte Carlo Pi (How many times will we do this?)
	//
	{
		if ( !OpenFileWriteStream( "outputImages/final.ppm" ) ) {
			return -1;
		}

		Vec3d lookat;
		// 		camera.pos = Vec3d( 278, 278, -800 );
		// 		lookat = Vec3d( 278, 278, 0 );
		camera.pos = Vec3d( 6, 0, 0 );
		lookat = Vec3d( 0 );
		camera.fwd = ( lookat - camera.pos );
		camera.fwd.Normalize();
		camera.left = Vec3d( 0, 0, 1 ).Cross( camera.fwd );
		camera.left.Normalize();
		camera.up = camera.fwd.Cross( camera.left );
		camera.up.Normalize();
		camera.m_focalPlane = 10.0f;
		camera.m_aperture = 0.0f;//0.2f;
		camera.m_fovy = 90.0f;

		BoundingVolumeHierarchyNode * world = NULL;
		CornellBox( &world );

		sprintf( strBuffer, "P3\n%i %i\n255\n", nx, ny );
		WriteFileStream( strBuffer );
		for ( int j = ny - 1; j >= 0; j-- ) {
			for ( int i = 0; i < nx; i++ ) {
				const int ns = 64;//64;
				Vec3d colorSum( 0, 0, 0 );
				for ( int s = 0; s < ns; s++ ) {
					float u = ( ( float( i ) + Random::Get() ) / float( nx ) );
					float v = ( ( float( j ) + Random::Get() ) / float( ny ) );

					Ray ray;
					camera.GetRay( u, v, ray );

					Vec3d color = ColorWorldMaterial( ray, world, 0, false );
					colorSum += color;
				}
				Vec3d color = colorSum / float( ns );

				// Gamma correct the color
				color = Vec3d( sqrtf( color.x ), sqrtf( color.y ), sqrtf( color.z ) );

				int ir = int( 255.99f * color.x );
				int ig = int( 255.99f * color.y );
				int ib = int( 255.99f * color.z );
				ir = ( ir > 255 ) ? 255 : ir;
				ig = ( ig > 255 ) ? 255 : ig;
				ib = ( ib > 255 ) ? 255 : ib;
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}
#endif

	return 0;
}


