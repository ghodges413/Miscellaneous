//
//  Common.h
//
#pragma once

#define WINDOWS_THREADS
//#define POSIX_THREADS
//#define STD_THREADS

#if defined( WINDOWS_THREADS )
	#include <Windows.h>
	#define ThreadReturnType_t DWORD WINAPI
	//typedef DWORD WINAPI ThreadReturnType_t;
	typedef LPVOID ThreadInputType_t;
#endif

#if defined( POSIX_THREADS )
	#include <pthread.h>
	typedef void * ThreadReturnType_t;
	typedef void * ThreadInputType_t;
#endif

#if defined( STD_THREADS )
	#include <thread>
	#include <mutex>
	#include <atomic>
#endif