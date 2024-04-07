//
//  ProducerConsumer.cpp
//
#include "ProducerConsumer.h"

#include <iostream>
#include <queue>
#include <array>
#include <thread>
#include <semaphore>
#include <condition_variable>

#define BUFFER_SIZE 8

// This implements a counting semaphore
class Semaphore {
public:
    void Release() {
        std::lock_guard< std::mutex > lock( m_mutex );

        ++m_count;
        m_condition.notify_one();
    }

    void Acquire() {
        std::unique_lock< std::mutex > lock( m_mutex );

        while ( 0 == m_count ) {
            m_condition.wait( lock );
        }

        --m_count;
    }

    bool TryAcquire() {
        std::lock_guard< std::mutex > lock( m_mutex );

        if ( m_count ) {
            --m_count;
            return true;
        }

        return false;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    unsigned int m_count = 0;
};


template< typename T >
class SyncQueue {
public:
    void Push( const T & value ) {
        // Lock to prevent pushes from multiple threads corrupting the queue
        std::lock_guard< std::mutex > lock( m_mutex );

        bool isEmpty = m_queue.empty(); // we may need to wake consumer
        m_queue.push( value );

        // Notify the condition variable to stop sleeping
        if ( isEmpty ) {
            m_condvar.notify_one();
        }
    }

    T Pop() {
        std::unique_lock< std::mutex > ulock( m_mutex );
        
        // spin until the queue contains a value
        while ( m_queue.empty() ) {
            m_condvar.wait( ulock );    // sleep until notified to wake up
        }

        // now m_queue is non-empty and we still have the lock
        T value = m_queue.front();
        m_queue.pop();
        return value;
    }

private:
    std::queue< T > m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condvar;
};


class ProducerConsumer {
public:
    ProducerConsumer( int start, int delta, int maxCount ) {
        m_value = start;
        m_delta = delta;
        m_maxCount = maxCount;

        m_startIdx = 0;
        m_endIdx = 0;
    }

#if 0
    void ProduceValue() {
        for ( int i = 0; i < m_maxCount; i++ ) {
            const int nextIdx = ( m_endIdx + 1 ) % BUFFER_SIZE;

            while ( nextIdx == m_startIdx ) {
                // spin
                std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
            }

            int idx = m_endIdx;
            m_buffer[ idx ] = m_value;
            m_value += m_delta;
            m_endIdx = nextIdx;
        }
    }

    int ConsumeValue() {
        while ( m_endIdx == m_startIdx ) {
            // spin
            std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        }

        int val = m_buffer[ m_startIdx ];
        m_startIdx = ( m_startIdx + 1 ) % BUFFER_SIZE;
        return val;
    }
#else
    void ProduceValue() {
        for ( int i = 0; i < m_maxCount; i++ ) {
            std::unique_lock< std::mutex > ulock( m_mutexProducer );

            const int nextIdx = ( m_endIdx + 1 ) % BUFFER_SIZE;
            if ( nextIdx == m_startIdx ) {
                m_cvProducer.wait( ulock ); // sleep until notified to wake up
            }

            int idx = m_endIdx;
            m_buffer[ idx ] = m_value;
            m_value += m_delta;
            m_endIdx = nextIdx;
            m_cvConsumer.notify_one();      // wake up the consumer
        }
    }

    int ConsumeValue() {
        std::unique_lock< std::mutex > ulock( m_mutexConsumer );

        if ( m_endIdx == m_startIdx ) {
            m_cvConsumer.wait( ulock ); // sleep until notified to wake up
        }

        int val = m_buffer[ m_startIdx ];
        m_startIdx = ( m_startIdx + 1 ) % BUFFER_SIZE;
        m_cvProducer.notify_one();      // wake up the producer
        return val;
    }
#endif

private:
    std::array< int, BUFFER_SIZE > m_buffer;

    int m_value;
    int m_delta;
    int m_maxCount;

    int m_startIdx;
    int m_endIdx;

    std::mutex m_mutexProducer;
    std::condition_variable m_cvProducer;

    std::mutex m_mutexConsumer;
    std::condition_variable m_cvConsumer;
};

#define MAX_COUNT 10000

ProducerConsumer odds( 1, 2, MAX_COUNT );
ProducerConsumer evens( 0, 2, MAX_COUNT );

void ProduceOdds() {
    odds.ProduceValue();
}

void ProduceEvens() {
    evens.ProduceValue();
}

std::vector< int > accumulator;

void Accumulate() {
    for ( int i = 0; i < MAX_COUNT; i++ ) {
        int a = odds.ConsumeValue();
        int b = evens.ConsumeValue();

        if ( a < b ) {
            accumulator.push_back( a );
            accumulator.push_back( b );
        } else {
            accumulator.push_back( b );
            accumulator.push_back( a );
        }
    }
}

void TestyTest() {
    std::thread oddsThread( ProduceOdds );
    std::thread evensThread( ProduceEvens );
    std::thread accumulatorThread( Accumulate );

    oddsThread.join();
    evensThread.join();
    accumulatorThread.join();

    // Validate
    bool isValid = true;
    for ( int i = 0; i < accumulator.size(); i++ ) {
        if ( i != accumulator[ i ] ) {
            isValid = false;
            break;
        }
    }

    if ( isValid ) {
        printf( "SUCCESS\n" );
    } else {
        printf( "FAILED\n" );
    }
    printf( "done\n" );
}

/*
===============================
TestProducerConsumer
===============================
*/
int TestProducerConsumer( int argc, char * argv[] ) {
    TestyTest();
    return 0;
}
