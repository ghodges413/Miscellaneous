//
//  Elevator.cpp
//  The elevator problem is solvable using two priority queues... one for up requests and one for down requests
//
#include "Elevator.h"

#include <iostream>

enum direction_t {
    UP,
    DOWN,
    IDLE
};

#define MAX_FLOORS 64

class Elevator {
public:
    Elevator() {
        m_currentFloor = 0;
        m_direction = direction_t::IDLE;
        m_numUp = 0;
        m_numDown = 0;
    }

    // These are requests that come from the outside (floor is the floor the person is on... internalRequest is the button they push once inside)
    void RequestUp( int floor, int internalRequest );
    void RequestDown( int floor, int internalRequest );

    void Run();

private:
    // This is a request from inside the elevator
    void RequestFloor( int floor );

private:
    int m_currentFloor;
    direction_t m_direction;

    // These are implemented as priority queues
    int m_numUp;
    int m_requestsUp[ MAX_FLOORS ];
    int m_internalRequestsUp[ MAX_FLOORS ];

    int m_numDown;
    int m_requestsDown[ MAX_FLOORS ];
    int m_internalRequestsDown[ MAX_FLOORS ];
};

void Elevator::RequestUp( int floor, int internalRequest ) {
    if ( floor < 0 || floor >= MAX_FLOORS ) {
        return;
    }

    // Check if this has already been requested
    for ( int i = 0; i < m_numUp; i++ ) {
        if ( floor == m_requestsUp[ i ] ) {
            return;
        }
    }

    // Find the index for insertion
    int insertIdx = m_numUp;
    for ( int i = 0; i < m_numUp; i++ ) {
        if ( floor > m_requestsUp[ i ] ) {
            insertIdx = i;
            break;
        }
    }

    // Insert at the index
    int tmp = m_requestsUp[ insertIdx ];
    m_requestsUp[ insertIdx ] = floor;
    

    // Push the rest of the stack up
    for ( int i = insertIdx + 1; i < m_numUp + 1; i++ ) {
        int tmp2 = m_requestsUp[ i ];
        m_requestsUp[ i ] = tmp;
        tmp = tmp2;
    }

    // Check for actions from person entering elevator
    {
        int tmp = m_internalRequestsUp[ insertIdx ];
        m_internalRequestsUp[ insertIdx ] = internalRequest;

        // Push the rest of the stack up
        for ( int i = insertIdx + 1; i < m_numUp + 1; i++ ) {
            int tmp2 = m_internalRequestsUp[ i ];
            m_internalRequestsUp[ i ] = tmp;
            tmp = tmp2;
        }
    }

    m_numUp++;
}

void Elevator::RequestDown( int floor, int internalRequest ) {
    if ( floor < 0 || floor >= MAX_FLOORS ) {
        return;
    }

    // Check if this has already been requested
    for ( int i = 0; i < m_numDown; i++ ) {
        if ( floor == m_requestsDown[ i ] ) {
            return;
        }
    }

    // Find the index for insertion
    int insertIdx = m_numDown;
    for ( int i = 0; i < m_numDown; i++ ) {
        if ( floor < m_requestsDown[ i ] ) {
            insertIdx = i;
            break;
        }
    }

    // Insert at the index
    int tmp = m_requestsDown[ insertIdx ];
    m_requestsDown[ insertIdx ] = floor;

    // Push the rest of the stack up
    for ( int i = insertIdx + 1; i < m_numDown + 1; i++ ) {
        int tmp2 = m_requestsDown[ i ];
        m_requestsDown[ i ] = tmp;
        tmp = tmp2;
    }

    // Check for actions from person entering elevator
    {
        int tmp = m_internalRequestsDown[ insertIdx ];
        m_internalRequestsDown[ insertIdx ] = internalRequest;

        // Push the rest of the stack up
        for ( int i = insertIdx + 1; i < m_numUp + 1; i++ ) {
            int tmp2 = m_internalRequestsDown[ i ];
            m_internalRequestsDown[ i ] = tmp;
            tmp = tmp2;
        }
    }

    m_numDown++;
}

void Elevator::RequestFloor( int floor ) {
    if ( floor < 0 || floor >= MAX_FLOORS ) {
        return;
    }

    if ( floor == m_currentFloor ) {
        // If it's the same floor, there's nothing to do... just open the doors
        return;
    }

    if ( floor < m_currentFloor ) {
        RequestDown( floor, -1 );
    }
    if ( floor > m_currentFloor ) {
        RequestUp( floor, -1 );
    }
}

void Elevator::Run() {
    while ( m_numDown > 0 || m_numUp > 0 ) {
        // If the elevator is idle, then choose a direction, prioritize going up
        if ( m_direction == direction_t::IDLE ) {
            if ( m_numUp > 0 ) {
                m_direction = direction_t::UP;
            } else if ( m_numDown > 0 ) {
                m_direction = direction_t::DOWN;
            }
        }

        if ( m_direction == direction_t::UP ) {
            // Process an up request
            const int oldFloor = m_currentFloor;
            m_numUp--;

            m_currentFloor = m_requestsUp[ m_numUp ];
            RequestFloor( m_internalRequestsUp[ m_numUp ] );

            if ( 0 == m_numUp ) {
                m_direction = direction_t::IDLE;
            }

            printf( "Processing Up: from %i to %i\n", oldFloor, m_currentFloor );
        }
        if ( m_direction == direction_t::DOWN ) {
            // Process a down request
            const int oldFloor = m_currentFloor;
            m_numDown--;

            m_currentFloor = m_requestsDown[ m_numDown ];
            RequestFloor( m_internalRequestsDown[ m_numDown ] );
            

            if ( 0 == m_numDown ) {
                m_direction = direction_t::IDLE;
            }

            printf( "Processing Down: from %i to %i\n", oldFloor, m_currentFloor );
        }
    }

    m_direction = direction_t::IDLE;
}

/*
===============================
TestElevator
===============================
*/
int TestElevator( int argc, char * argv[] ) {
    Elevator elevator;

    elevator.RequestDown( 10, 5 );  // Person on 15, wants to go to 5
    elevator.RequestDown( 8, 0 );   // Person on 8, wants to go to 0
    elevator.RequestDown( 15, 0 );  // Person on 15, wants to go to 0

    elevator.RequestUp( 0, 24 );    // Person on 0, wants to go to 24
    elevator.RequestUp( 3, 2 );     // Person made a mistake, by hitting up (they wanted to go down)
    elevator.RequestUp( 15, 24 );   // Person on 15, wants to go to 24

    elevator.Run();
    return 0;
}