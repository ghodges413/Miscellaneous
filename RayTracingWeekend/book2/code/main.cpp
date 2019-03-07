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
	if ( world->Hit( ray, 0.0f, 10000.0f, record ) ) {
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
// 	char strBuffer[ 1024 ];
// 
// 	const int nx = 384 * 2;//512;
// 	const int ny = 256 * 2;
// 
// 	Camera camera( 90, float( nx ) / float( ny ), 5.0f, 1.5f, 1.0f );
// 	Random random;
// 
// 	Perlin::Initialize( random );
// 
// 	Targa targaMars;
// 	Targa targaEarth;
// 	targaEarth.Load( "../common/images/earth512.tga", true );
// 	targaMars.Load( "../common/images/mars512.tga", true );

#if 1
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
#elif 1
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

		const int N = 10000;
		float sum = 0.0f;
		for ( int i = 0; i < N; i++ ) {
			Vec3d p = Random::RandomOnSphereSurface();
			float cosSquared = p.z * p.z;
			sum += cosSquared / lambdaPDF3( p );
		}
		const float integral = sum / (float)N;
		printf( "I = %f\n", integral );
		printf( "-----------------------\n" );
	}
#elif 2
	//
	//	Chapter  - Monte Carlo Pi (How many times will we do this?)
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
#endif

	return 0;
}


