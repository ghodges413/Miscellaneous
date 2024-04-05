//
//  Client.cpp
//
#include "Client.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment( lib, "Ws2_32.lib" )
#pragma comment( lib, "Mswsock.lib" )
#pragma comment( lib, "AdvApi32.lib" )


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

#define BUFSIZE 512

/*
===============================
Client
===============================
*/
int Client( int argc, char * argv[] ) {
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;

    struct addrinfo * result = NULL;
    struct addrinfo * ptr = NULL;
    struct addrinfo hints;

    const char * sendbuf = "this is a test";
    char recvbuf[ DEFAULT_BUFLEN ];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Validate the parameters
    if ( 2 != argc ) {
        printf( "usage: %s server-name\n", argv[ 0 ] );
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
    if ( 0 != iResult ) {
        printf( "WSAStartup failed with error: %i\n", iResult );
        return 1;
    }

    ZeroMemory( &hints, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo( argv[ 1 ], DEFAULT_PORT, &hints, &result );
    if ( 0 != iResult ) {
        printf( "getaddrinfo failed with error: %i\n", iResult );
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for ( ptr = result; ptr != NULL; ptr = ptr->ai_next ) {
        // Create a SOCKET for connecting to server
        ConnectSocket = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol );
        if ( INVALID_SOCKET == ConnectSocket ) {
            printf( "socket failed with error: %ld\n", WSAGetLastError() );
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen );
        if ( SOCKET_ERROR == iResult ) {
            closesocket( ConnectSocket );
            ConnectSocket = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo( result );

    if ( INVALID_SOCKET == ConnectSocket ) {
        printf( "Unable to connect to server!\n" );
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen( sendbuf ), 0 );
    if ( SOCKET_ERROR == iResult ) {
        printf( "send failed with error: %i\n", WSAGetLastError() );
        closesocket( ConnectSocket );
        WSACleanup();
        return 1;
    }

    printf( "Bytes Sent: %ld\n", iResult );

    // shutdown the connection since no more data will be sent
    iResult = shutdown( ConnectSocket, SD_SEND );
    if ( SOCKET_ERROR == iResult ) {
        printf( "shutdown failed with error: %i\n", WSAGetLastError() );
        closesocket( ConnectSocket );
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {
        iResult = recv( ConnectSocket, recvbuf, recvbuflen, 0 );

        if ( iResult > 0 ) {
            printf( "Bytes received: %i\n", iResult );
        } else if ( iResult == 0 ) {
            printf( "Connection closed\n" );
        } else {
            printf( "recv failed with error: %i\n", WSAGetLastError() );
        }
    } while ( iResult > 0 );

    // cleanup
    closesocket( ConnectSocket );
    WSACleanup();
    return 0;
}


#if 0   // Enable to use POSIX
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"

void DieWithUserMessage( const char * str ) {
    printf( str );
    exit( 0 );
}


/*
===============================
ClientPosix
===============================
*/
int ClientPosix( int argc, char * argv[] ) {
    if ( argc < 3 || argc > 4 ) {
        DieWithUserMessage( "Parameters <server address> <echo word> [<server port>]" );
    }

    char * servIP = argv[ 1 ];      // server ip address
    char * echoString = argv[ 2 ];  // string to echo

    // third arg is optional, server port.  If dne, then use 7 as a default
    in_port_t servPort = (argc == 4 ) ? atoi( argv[ 3 ] ) : 7;

    // Create a reliable, stream socket using TCP
    int sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( sock < 0 ) {
        DieWithUserMessage( "socket failed" );
    }

    // Construct the server address structure
    struct sockaddr_in servAddr;
    memset( &servAddr, 0, sizeof( servAddr ) );
    servAddr.sin_family = AF_INET; // ipv4 family

    // convert address ( inet_pton = printable to numeric )
    int returnValue = inet_pton( AF_INET, servIP, &servAddr.sin_addr.s_addr );
    if ( 0 == returnValue ) {
        DieWithUserMessage( "inet_pton failed, invalid address string" );
    } else if ( returnValue < 0 ) {
        DieWithUserMessage( "inet_pton failed" );
    }
    servAddr.sin_port = htons( servPort ); // server port ( htons = host to network  [ there's a host byte order and a network byte order ] )

    // Establish the connection to the echo server
    if ( connect( sock, (struct sockaddr *)&servAddr, sizeof( servAddr ) ) < 0 ) {
        DieWithUserMessage( "connect failed" );
    }

    size_t echoStringLen = strlen( echoString ); // determine input length

    // Send the string to the server
    size_t numBytes = send( sock, echoString, echoStringLen, 0 );
    if ( numBytes < 0 ) {
        DieWithUserMessage( "send failed" );
    } else if ( numBytes != echoStringLen ) {
        DieWithUserMessage( "send sent unexpected number of bytes" );
    }

    // Receive the same string back from the server
    unsigned int totalBytesRecvd = 0;
    fputs( "Received: ", stdout );
    while ( totalBytesRecvd < echoStringLen ) {
        char buffer[ BUFSIZE ];
        // receive up to the buffer size ( minus 1 to leave space for anull terminator ) bytes fromt he sender */
        numBytes = recv( sock, buffer, BUFSIZE - 1, 0 );
        if ( numBytes < 0 ) {
            DieWithUserMessage( "recv failed" );
        } else if ( 0 == numBytes ) {
            DieWithUserMessage( "recv connection closed prematurely" );
        }
        totalBytesRecvd += numBytes;
        buffer[ numBytes ] = '\0';
        fputs( buffer, stdout );
    }

    fputc( '\n', stdout );

    close( sock );
    exit( 0 );
}

/*
===============================
PrintSocketAddress
===============================
*/
void PrintSocketAddress( const struct sockaddr * address ) {
    if ( NULL == address ) {
        return;
    }

    void * numericAddress; // pointer to binary address
    char addrBuff[ INET6_ADDRSTRLEN ]; // buffer to contain resulst (ipv6 sufficient tho hold ipv4)
    in_port_t port; // port to print

    switch ( address->sa_family ) {
        case AF_INET: {
            numericAddress = &((struct sockaddr_in *)address)->sin_addr;
            port = ntohs( ((struct sockaddr_in *)address)->sin_port );
        } break;
        case AF_INET6: {
            numericAddress = &((struct sockaddr_in6 *)address)->sin6_addr;
            port = ntohs( ((struct sockaddr_in6 *)address)->sin6_port );
        } break;
        default: {
            printf( "[unknown type]" );
        } return;
    }

    // convert binary to printable address
    if ( inet_ntop( address->sa_family, numericAddress, addrBuff, sizeof( addrBuff ) ) == NULL ) {
        printf( "invalid address" );
    } else {
        printf( "%s", addrBuff );
        if ( 0 != port ) {
            printf( "-%u", port );
        }
    }
}

/*
===============================
GetAddrInfo
===============================
*/
int GetAddrInfo( int argc, char * argv[] ) {
    if ( argc != 3 ) {
        DieWithUserMessage( "parms: <address/name> <port/service>" );
    }

    char * addrString = argv[ 1 ];  // server address
    char * portString = argv[ 2 ];  // server port

    // tell the system what kinds of address info we want
    struct addrinfo addrCriteria;
    memset( & addrCriteria, 0, sizeof( addrCriteria ) );
    addrCriteria.ai_family = AF_UNSPEC; // any address family
    addrCriteria.ai_socktype = SOCK_STREAM; // only stream sockets
    addrCriteria.ai_protocol = IPPROTO_TCP; // only tcp protocol

    // get addresses associated wit the specified name/service
    struct addrinfo * addrList;

    // modify servaddr contents to reference linked list of addresses
    int rtnVal = getaddrinfo( addrString, portString, &addrCriteria, &addrList );
    if ( 0 != rtnVal ) {
        printf( gai_strerror( rtnVal ) );
        DieWithUserMessage( "get addrinfo failed" );
    }

    // display returned addresses
    for ( struct addrinfo * addr = addrList; addr != NULL; addr->ai_next ) {
        PrintSocketAddress( addr->ai_addr );//, stdout );
        fputc( '\n', stdout );
    }

    freeaddrinfo( addrList );
    exit( 0 );
}


void HandleTCPClient( int clntSocket ) {
    char buffer[ BUFSIZE ]; // buffer for echo string

    // receive message from client
    size_t numBytesRcvd = recv( clntSocket, buffer, BUFSIZE, 0 );
    if ( numBytesRcvd < 0 ) {
        DieWithUserMessage( "recv failed" );
    }

    // send receive string and recive again until end of stream
    while ( numBytesRcvd > 0 ) {
        // echo mesage back to client
        size_t numBytesSent = send( clntSocket, buffer, numBytesRcvd, 0 );
        if ( numBytesSent < 0 ) {
            DieWithUserMessage( "send failed" );
        } else if ( numBytesSent != numBytesRcvd ) {
            DieWithUserMessage( "send, sent unexpected number of bytes" );
        }

        // see if there is more data to receive
        numBytesRcvd = recv( clntSocket, buffer, BUFSIZE, 0 );
        if ( numBytesRcvd < 0 ) {
            DieWithUserMessage( "recv failed" );
        }
    }

    close( clntSocket );
}

static const int MAXPENDING = 5; // max outstanding connection requests

int SeverPosix( int argc, char * argv[] ) {
    if ( 2 != argc ) {
        DieWithUserMessage( "Paramter <server port>" );
    }

    in_port_t servPort = atoi( argv[ 1 ] ); // local port

    // create socket for incoming connections
    int servSock;
    if ( ( servSock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 ) {
        DieWithUserMessage( "socket failed" );
    }

    // construct local address structure
    struct sockaddr_in servAddr;
    memset( &servAddr, 0, sizeof( servAddr ) );
    servAddr.sin_family = AF_INET; // ipv4 protocol
    servAddr.sin_addr.s_addr = htonl( INADDR_ANY ); // any incoming interface ( htonl = host to network  [ there's a host byte order and a network byte order ] )
    servAddr.sin_port = htons( servPort ); // local port

    // bind to the local address
    if ( bind( servSock, (struct sockaddr *)&servAddr, sizeof( servAddr ) ) < 0 ) {
        DieWithUserMessage( "bind failed" );
    }

    // mark the socket so it will listen for incoming connections
    if ( listen( servSock, MAXPENDING ) < 0 ) {
        DieWithUserMessage( "listen failed" );
    }

    while ( 1 ) {
        struct sockaddr_in clntAddr; //client address

        // set length of clietn address structure
        socklen_t clntAddrLen = sizeof( clntAddr );

        // wait for a client to connect
        int clntSock = accept( servSock, (struct sockaddr *)&clntAddr, &clntAddrLen );
        if ( clntSock < 0 ) {
            DieWithUserMessage( "accept failed" );
        }

        // clntSock is connect to a client!

        char clntName[ INET_ADDRSTRLEN ]; // str containing client address
        // inet_ntop = numeric to printable
        if ( inet_ntop( AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof( clntName ) ) != NULL ) {
            printf( "Handling client %s/%d\n", clntName, ntohs( clntAddr.sin_port ) );
        } else {
            puts( "Unable to get client address" );
        }

        HandleTCPClient( clntSock );
    }
}
#endif