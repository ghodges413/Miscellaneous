//
//  Observer.h
//
#pragma once
#include "../datastructures/List.h"

/*
===============================
Event_t
===============================
*/
struct Event_t {
	typedef void (*EventFunctor)( void *, void * );
	EventFunctor m_functor;
	void * m_sender;
	void * m_parms;
};

/*
===============================
EventSystem
===============================
*/
class EventSystem {
public:
	void RegisterEvent( Event_t event ) {
		m_events.Append( event );
	}
	void ServiceEvents() {
		for ( int i = 0; i < m_events.Num(); i++ ) {
			m_events[ i ].m_functor( m_events[ i ].m_sender, m_events[ i ].m_parms );
		}
		m_events.Clear();
	}

private:
	List< Event_t > m_events;
};
