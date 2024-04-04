//
//  Sockets.cpp
//
#include "Sockets.h"
#include "Client.h"
#include "Server.h"

/*
===============================
TestSockets
===============================
*/
int TestSockets( int argc, char * argv[] ) {
    Client( argc, argv );
    Server( argc, argv );

    return 0;
}
