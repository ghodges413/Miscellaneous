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
#include "Camera.h"

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
HitSphere
====================================================
*/
float HitSphere( const Vec3d & center, const float radius, const Ray & ray ) {
	const Vec3d oc = ray.pt - center;
	const float a = ray.dir.DotProduct( ray.dir );
	const float b = 2.0f * oc.DotProduct( ray.dir );
	const float c = oc.DotProduct( oc ) - radius * radius;
	const float discriminant = b * b - 4.0f * a * c;
	if ( discriminant < 0.0f ) {
		return -1.0f;
	}

	return ( ( -b - sqrtf( discriminant ) ) / ( 2.0f * a ) );
}

/*
====================================================
ColorWorld
====================================================
*/
Vec3d ColorWorld( const Ray & ray, Hitable * world, Random & rnd, int recurssion ) {
	if ( recurssion > 32 ) {
		return Vec3d( 0 );
	}

	hitRecord_t record;
	if ( world->Hit( ray, 0.0f, 1000.0f, record ) ) {
		Vec3d target = record.point + record.normal + rnd.RandomInUnitSphere();
		return ( ColorWorld( Ray( record.point, target ), world, rnd, recurssion + 1 ) * 0.5f );
	}

	return ColorBackground( ray.dir );
}

/*
====================================================
ColorWorldMaterial
====================================================
*/
Vec3d ColorWorldMaterial( const Ray & ray, Hitable * world, Random & rnd, int recurssion ) {
	if ( recurssion > 32 ) {
		return Vec3d( 0 );
	}

	hitRecord_t record;
	if ( world->Hit( ray, 0.0f, 1000.0f, record ) ) {
		Ray scattered;
		Vec3d attenuation;
		if ( NULL != record.material && record.material->Scatter( ray, record, attenuation, scattered, rnd ) ) {
			Vec3d color = ColorWorldMaterial( scattered, world, rnd, recurssion + 1 );
			color.x *= attenuation.x;
			color.y *= attenuation.y;
			color.z *= attenuation.z;
			return color;
		}
		
		return Vec3d( 0 );
	}

	return ColorBackground( ray.dir );
}

/*
====================================================
main
====================================================
*/
int main( int argc, char * argv[] ) {
	Vec3d camPos = Vec3d( 0, 0, 0 );
	Vec3d camFwd = Vec3d( 1, 0, 0 );
	Vec3d camUp = Vec3d( 0, 0, 1 );
	Vec3d camLeft = Vec3d( 0, 1, 0 );

	char strBuffer[ 1024 ];

	const int nx = 512;
	const int ny = 256;

	Camera camera( 90, float( nx ) / float( ny ), 5.0f, 1.5f );
	Random random;
#if 0
	//
	//	Chapter 1 - Hello World
	//
	{
		if ( !OpenFileWriteStream( "outputImages/helloworld.ppm" ) ) {
			return -1;
		}

		sprintf( strBuffer, "P3\n%i %i\n255\n", nx, ny );
		WriteFileStream( strBuffer );
		for ( int j = ny - 1; j >= 0; j-- ) {
			for ( int i = 0; i < nx; i++ ) {
				float r = float( i ) / float( nx );
				float g = float( j ) / float( ny );
				float b = 0.2f;
				int ir = int( 255.99f * r );
				int ig = int( 255.99f * g );
				int ib = int( 255.99f * b );
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}

	//
	//	Chapter 2 - Blue sky background
	//
	{
		if ( !OpenFileWriteStream( "outputImages/background.ppm" ) ) {
			return -1;
		}

		sprintf( strBuffer, "P3\n%i %i\n255\n", nx, ny );
		WriteFileStream( strBuffer );
		for ( int j = ny - 1; j >= 0; j-- ) {
			for ( int i = 0; i < nx; i++ ) {
				float screenRatio = float( nx ) / float( ny );
				float u = ( ( float( i ) / float( nx ) ) - 0.5f ) * screenRatio;
				float v = ( float( j ) / float( ny ) ) - 0.5f;

				Vec3d ptA = camPos;
				Vec3d ptB = camPos + camFwd + camLeft * u + camUp * v;

				Ray ray = Ray( ptA, ptB );
				Vec3d color = ColorBackground( ray.dir );

				int ir = int( 255.99f * color.x );
				int ig = int( 255.99f * color.y );
				int ib = int( 255.99f * color.z );
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}


	//
	//	Chapter 4 - Red Sphere
	//
	{
		if ( !OpenFileWriteStream( "outputImages/redSphere.ppm" ) ) {
			return -1;
		}

		Vec3d spherePos = Vec3d( 10, 0, 0 );
		float sphereRadius = 2.0f;

		sprintf( strBuffer, "P3\n%i %i\n255\n", nx, ny );
		WriteFileStream( strBuffer );
		for ( int j = ny - 1; j >= 0; j-- ) {
			for ( int i = 0; i < nx; i++ ) {
				float screenRatio = float( nx ) / float( ny );
				float u = ( ( float( i ) / float( nx ) ) - 0.5f ) * screenRatio;
				float v = ( float( j ) / float( ny ) ) - 0.5f;

				Vec3d ptA = camPos;
				Vec3d ptB = camPos + camFwd + camLeft * u + camUp * v;

				Ray ray = Ray( ptA, ptB );
				Vec3d color = ColorBackground( ray.dir );
				if ( HitSphere( spherePos, sphereRadius, ray ) >= 0.0f ) {
					color = Vec3d( 1, 0, 0 );
				}

				int ir = int( 255.99f * color.x );
				int ig = int( 255.99f * color.y );
				int ib = int( 255.99f * color.z );
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}

	//
	//	Chapter 5 - Normal
	//
	{
		if ( !OpenFileWriteStream( "outputImages/normal.ppm" ) ) {
			return -1;
		}

	 	Vec3d spherePos = Vec3d( 10, 0, 0 );
	 	float sphereRadius = 2.0f;

		sprintf( strBuffer, "P3\n%i %i\n255\n", nx, ny );
		WriteFileStream( strBuffer );
		for ( int j = ny - 1; j >= 0; j-- ) {
			for ( int i = 0; i < nx; i++ ) {
				float screenRatio = float( nx ) / float( ny );
				float u = ( ( float( i ) / float( nx ) ) - 0.5f ) * screenRatio;
				float v = ( float( j ) / float( ny ) ) - 0.5f;

				Vec3d ptA = camPos;
				Vec3d ptB = camPos + camFwd + camLeft * u + camUp * v;

				Ray ray = Ray( ptA, ptB );
				Vec3d color = ColorBackground( ray.dir );
				float t = HitSphere( spherePos, sphereRadius, ray );
				if ( t >= 0 ) {
					Vec3d ptOnSphere = ray.PositionAtT( t );
					Vec3d normal = ptOnSphere - spherePos;
					normal.Normalize();
					color = ( normal + Vec3d( 1.0f ) ) * 0.5f;
				}

				int ir = int( 255.99f * color.x );
				int ig = int( 255.99f * color.y );
				int ib = int( 255.99f * color.z );
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}

	//
	//	Chapter 5.5 - World
	//
	{
		if ( !OpenFileWriteStream( "outputImages/world.ppm" ) ) {
			return -1;
		}

		Hitable * list[ 2 ];
		list[ 0 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, 0.1f ), 1.0f );
		list[ 1 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, -101.0f ), 100.0f );
		HitableList * world = new HitableList( list, 2 );

		sprintf( strBuffer, "P3\n%i %i\n255\n", nx, ny );
		WriteFileStream( strBuffer );
		for ( int j = ny - 1; j >= 0; j-- ) {
			for ( int i = 0; i < nx; i++ ) {
				float screenRatio = float( nx ) / float( ny );
				float u = ( ( float( i ) / float( nx ) ) - 0.5f ) * screenRatio;
				float v = ( float( j ) / float( ny ) ) - 0.5f;

				Vec3d ptA = camPos;
				Vec3d ptB = camPos + camFwd + camLeft * u + camUp * v;

				Ray ray = Ray( ptA, ptB );
				Vec3d color = ColorBackground( ray.dir );
				hitRecord_t record;
				if ( world->Hit( ray, 1.0f, 200.0f, record ) ) {
					color = ( record.normal + Vec3d( 1.0f ) ) * 0.5f;
				}

				int ir = int( 255.99f * color.x );
				int ig = int( 255.99f * color.y );
				int ib = int( 255.99f * color.z );
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}

	//
	//	Chapter 6 - AntiAliasing
	//
	{
		if ( !OpenFileWriteStream( "outputImages/antialiasing.ppm" ) ) {
			return -1;
		}

		Hitable * list[ 2 ];
		list[ 0 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, 0.1f ), 1.0f );
		list[ 1 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, -101.0f ), 100.0f );
		HitableList * world = new HitableList( list, 2 );

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
					camera.GetRay( u, v, ray );

					Vec3d color = ColorBackground( ray.dir );
					hitRecord_t record;
					if ( world->Hit( ray, 1.0f, 200.0f, record ) ) {
						color = ( record.normal + Vec3d( 1.0f ) ) * 0.5f;
					}
					colorSum += color;
				}
				Vec3d color = colorSum / float( ns );

				int ir = int( 255.99f * color.x );
				int ig = int( 255.99f * color.y );
				int ib = int( 255.99f * color.z );
				sprintf( strBuffer, "%i %i %i\n", ir, ig, ib );
				WriteFileStream( strBuffer );
			}
		}

		CloseFileWriteStream();
	}


	//
	//	Chapter 7 - Diffuse
	//
	{
		if ( !OpenFileWriteStream( "outputImages/diffuse.ppm" ) ) {
			return -1;
		}

		Hitable * list[ 2 ];
		list[ 0 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, 0.1f ), 1.0f );
		list[ 1 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, -101.0f ), 100.0f );
		HitableList * world = new HitableList( list, 2 );

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
					camera.GetRay( u, v, ray );
				
					Vec3d color = ColorWorld( ray, world, random, 0 );
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

	//
	//	Chapter 8 - Metal
	//
	{
		if ( !OpenFileWriteStream( "outputImages/metal.ppm" ) ) {
			return -1;
		}

		Hitable * list[ 4 ];
		list[ 0 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, 0.1f ), 1.0f, new Lambertian( Vec3d( 0.8f, 0.3f, 0.3f ) ) );
		list[ 1 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, -101.0f ), 100.0f, new Lambertian( Vec3d( 0.8f, 0.8f, 0.0f ) ) );
		list[ 2 ] = new HitableSphere( Vec3d( 5.0f, -3.0f, 0.1f ), 1.0f, new Metal( Vec3d( 0.8f, 0.6f, 0.2f ), 0.3f ) );
		list[ 3 ] = new HitableSphere( Vec3d( 5.f, 3.0f, 0.1f ), 1.0f, new Metal( Vec3d( 0.8f, 0.8f, 0.8f ), 0.0f ) );
		HitableList * world = new HitableList( list, 4 );

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
					camera.GetRay( u, v, ray );

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

	//
	//	Chapter 9 - Dielectrics
	//
	{
		if ( !OpenFileWriteStream( "outputImages/dielectrics.ppm" ) ) {
			return -1;
		}

		Hitable * list[ 5 ];
		list[ 0 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, 0.1f ), 1.0f, new Lambertian( Vec3d( 0.8f, 0.3f, 0.3f ) ) );
		list[ 1 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, -101.0f ), 100.0f, new Lambertian( Vec3d( 0.8f, 0.8f, 0.0f ) ) );
		list[ 2 ] = new HitableSphere( Vec3d( 5.0f, -3.0f, 0.1f ), 1.0f, new Metal( Vec3d( 0.8f, 0.6f, 0.2f ), 0.3f ) );
		list[ 3 ] = new HitableSphere( Vec3d( 5.0f, 3.0f, 0.1f ), 1.0f, new Dielectric( 1.5f ) );
		list[ 4 ] = new HitableSphere( Vec3d( 7.0f, 2.0f, 0.1f ), 1.0f, new Dielectric( 1.5f ) );
		HitableList * world = new HitableList( list, 5 );

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
					camera.GetRay( u, v, ray );

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

	//
	//	Chapter 10 - defocus
	//
	{
		if ( !OpenFileWriteStream( "outputImages/defocus.ppm" ) ) {
			return -1;
		}

		Hitable * list[ 5 ];
		list[ 0 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, 0.1f ), 1.0f, new Lambertian( Vec3d( 0.8f, 0.3f, 0.3f ) ) );
		list[ 1 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, -101.0f ), 100.0f, new Lambertian( Vec3d( 0.8f, 0.8f, 0.0f ) ) );
		list[ 2 ] = new HitableSphere( Vec3d( 5.0f, -3.0f, 0.1f ), 1.0f, new Metal( Vec3d( 0.8f, 0.6f, 0.2f ), 0.3f ) );
		list[ 3 ] = new HitableSphere( Vec3d( 5.0f, 3.0f, 0.1f ), 1.0f, new Dielectric( 1.5f ) );
		list[ 4 ] = new HitableSphere( Vec3d( 7.0f, 2.0f, 0.1f ), 1.0f, new Dielectric( 1.5f ) );
		HitableList * world = new HitableList( list, 5 );

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
#endif
	//
	//	Chapter 11 - cover image
	//
	{
		if ( !OpenFileWriteStream( "outputImages/cover.ppm" ) ) {
			return -1;
		}

		camera.pos = Vec3d( 9,-3, 2 );
		Vec3d lookat = Vec3d( 0, 0, 0 );
		camera.fwd = ( lookat - camera.pos );
		camera.fwd.Normalize();
		camera.left = Vec3d( 0, 0, 1 ).Cross( camera.fwd );
		camera.left.Normalize();
		camera.up = camera.fwd.Cross( camera.left );
		camera.up.Normalize();
		camera.m_focalPlane = ( lookat - camera.pos ).GetMagnitude();
		camera.m_aperture = 0.2f;
		camera.m_fovy = 60.0f;

		const int n = 500;
		Hitable ** list = new Hitable* [ n ];
		list[ 0 ] = new HitableSphere( Vec3d( 5.0f, 0.0f, -500.0f ), 500.0f, new Lambertian( Vec3d( 0.5f, 0.5f, 0.5f ) ) );
		HitableList * world = NULL;
		{
			int i = 1;
			for ( int a = -11; a < 11; a++ ) {
				for ( int b = -11; b < 11; b++ ) {
					float chooseMat = random.Get();
					Vec3d center( a + 0.9f * random.Get(), b + 0.9f * random.Get(), 0.2f );
					if ( ( center - Vec3d( 4.0f, 0.2f, 0.0f ) ).GetMagnitude() > 0.9f ) {
						if ( chooseMat < 0.8f ) {
							list[ i++ ] = new HitableSphere( center, 0.2f, new Lambertian( Vec3d( random.Get() * random.Get(), random.Get() * random.Get(), random.Get() * random.Get() ) ) );
						} else if ( chooseMat < 0.95f ) {
							list[ i++ ] = new HitableSphere( center, 0.2f, new Metal( Vec3d( 0.5f * ( 1.0f + random.Get() ), 0.5f * ( 1.0f + random.Get() ), 0.5f * ( 1.0f + random.Get() ) ), 0.5f * random.Get() ) );
						} else {
							list[ i++ ] = new HitableSphere( center, 0.2f, new Dielectric( 1.5f ) );
						}
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

	return 0;
}


