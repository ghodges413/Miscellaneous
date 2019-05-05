//
//  Threads.h
//
#pragma once
#include "Common.h"

typedef ThreadReturnType_t ThreadWorkFunctor( ThreadInputType_t data );

struct ThreadWork_t {
	ThreadWorkFunctor m_functor;
	void * m_data;
};

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
	Thread();
	~Thread();

	static ThreadReturnType_t Main( ThreadInputType_t data );	// Loop idle until signaled to close thread
	bool Create();
	void Join();

	bool SetWork( ThreadWork_t * work );
	bool HasWork() const { return ( NULL != m_work ); }

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

	bool m_loopIdle;
	ThreadWork_t * m_work;
};