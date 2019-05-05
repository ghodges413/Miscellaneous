//
//  Threads.cpp
//
#include "Threads.h"

/*
===============================
Thread::Thread
===============================
*/
Thread::Thread() :
m_loopIdle( false ),
m_work( NULL ) {
}

/*
===============================
Thread::~Thread
===============================
*/
Thread::~Thread() {
}

/*
===============================
Thread::Thread
===============================
*/
ThreadReturnType_t Thread::Main( ThreadInputType_t data ) {
	// Get the pointer to this thread.
	Thread * thread_ptr = (Thread *)data;

	// Loop idly until there's work available to do.
	while ( thread_ptr->m_loopIdle ) {
		if ( thread_ptr->HasWork() ) {
			thread_ptr->m_work->m_functor( thread_ptr->m_work->m_data );
			thread_ptr->m_work = NULL;
		}

		// I'm sure there's a better way to do this without "sleeping" and using a loop
		// Let the thread sleep for a second
		Sleep( 1000 );
	}

	return NULL;
}

/*
===============================
Thread::Create
===============================
*/
bool Thread::Create() {
	m_loopIdle = true;

#if defined( WINDOWS_THREADS )
	m_threadHandle = CreateThread( 
		NULL,						// default security attributes
		0,							// use default stack size  
		Thread::Main,				// thread function name
		this,						// argument to thread function 
		0,							// use default creation flags 
		&m_dwThreadID );			// returns the thread identifier
#endif

#if defined( POSIX_THREADS )
	m_returnCode = pthread_create( &m_threadHandle, NULL, Thread::Main, (void *)this );
#endif

#if defined( STD_THREADS )
	m_threadHandle = new std::thread( &Thread::Main, this );
#endif

	return true;
}

/*
===============================
Thread::Join
===============================
*/
void Thread::Join() {
	m_loopIdle = false;

#if defined( WINDOWS_THREADS )
	// Wait until this thread has terminated.
	WaitForMultipleObjects( 1, &m_threadHandle, TRUE, INFINITE );
	CloseHandle( m_threadHandle );
#endif

#if defined( POSIX_THREADS )
	m_returnCode = pthread_join( m_threadHandle, NULL );
#endif

#if defined( STD_THREADS )
	m_threadHandle->join();
	delete m_threadHandle;
	m_threadHandle = NULL;
#endif
}

/*
===============================
Thread::SetWork
===============================
*/
bool Thread::SetWork( ThreadWork_t * work ) {
	if ( HasWork() ) {
		return false;
	}

	m_work = work;
	return true;
}