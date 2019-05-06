//
//  Threads.h
//
#pragma once
#include "Common.h"

typedef ThreadReturnType_t ThreadWorkFunctor( ThreadInputType_t data );

/*
===============================
Thread
===============================
*/
class Thread {
private:
	Thread( const Thread & rhs );
	Thread & operator = ( const Thread & rhs );

public:
	Thread() {}
	~Thread() {}

	bool Create( ThreadWorkFunctor * functor, void * data );
	void Join();
	void YieldThread();

	static unsigned int NumHardwareThreads();

private:
#if defined( WINDOWS_THREADS )
	HANDLE m_threadHandle;
	DWORD m_dwThreadID;
#endif

#if defined( POSIX_THREADS )
	pthread_t m_threadHandle;
	int m_returnCode;
#endif

#if defined( STD_THREADS )
	std::thread * m_threadHandle;
#endif
};