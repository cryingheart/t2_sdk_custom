///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2003 Intel Corporation 
// All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met: 
//
// * Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
// * Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution. 
// * Neither name of Intel Corporation nor the names of its contributors 
// may be used to endorse or promote products derived from this software 
// without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

/************************************************************************
* Purpose: This file implements the functionality and utility functions
* used by the Miniserver module.
************************************************************************/

#include "config.h"
#ifndef WIN32
//	#include <arpa/inet.h>
//	#include <netinet/in.h>
	#include "lwip/inet.h"
//	#include <sys/socket.h>
	#include "lwip/sockets.h"
	#include <sys/wait.h>
	#include <unistd.h>
	#include <sys/time.h>
#else /* WIN32 */
	#include <winsock2.h>

	typedef int socklen_t;
	#define EAFNOSUPPORT 97
#endif /* WIN32 */
#include "unixutil.h"
#include "ithread.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "ssdplib.h"

#include "util.h"
#include "miniserver.h"
#include "ThreadPool.h"
#include "httpreadwrite.h"
#include "statcodes.h"
#include "upnp.h"
#include "upnpapi.h"

#define APPLICATION_LISTENING_PORT 49152
#define SOMAXCONN 10

struct mserv_request_t {
    int connfd;                 // connection handle
    struct in_addr foreign_ip_addr;
    unsigned short foreign_ip_port;
};

typedef enum { MSERV_IDLE, MSERV_RUNNING, MSERV_STOPPING } MiniServerState;

unsigned short miniStopSockPort;

////////////////////////////////////////////////////////////////////////////
// module vars
static MiniServerCallback gGetCallback = NULL;
static MiniServerCallback gSoapCallback = NULL;
static MiniServerCallback gGenaCallback = NULL;
static MiniServerState gMServState = MSERV_IDLE;

/************************************************************************
 * Function: SetHTTPGetCallback
 *
 * Parameters :
 * 	MiniServerCallback callback - HTTP Callback to be invoked 
 *
 * Description:
 * 	Set HTTP Get Callback
 *
 * Return: void
 ************************************************************************/
void
SetHTTPGetCallback( MiniServerCallback callback )
{
    gGetCallback = callback;
}

/************************************************************************
 * Function: SetSoapCallback
 *
 * Parameters:
 *	MiniServerCallback callback - SOAP Callback to be invoked 
 *
 * Description:
 * 	Set SOAP Callback
 *
 * Return: void
 ************************************************************************/
#ifdef INCLUDE_DEVICE_APIS
void
SetSoapCallback( MiniServerCallback callback )
{
    gSoapCallback = callback;
}
#endif /* INCLUDE_DEVICE_APIS */

/************************************************************************
 * Function: SetGenaCallback
 *
 * Parameters:
 *	MiniServerCallback callback - GENA Callback to be invoked
 *
 * Description:
 * 	Set GENA Callback
 *
 * Return: void
 ************************************************************************/
void
SetGenaCallback( MiniServerCallback callback )
{
    gGenaCallback = callback;
}

/************************************************************************
 * Function :	dispatch_request
 *
 * Parameters :
 *	IN SOCKINFO *info	- Socket Information object.
 *	http_parser_t* hparser	- HTTP parser object.
 *
 * Description :
 *	Based on the type pf message, appropriate callback is issued
 *
 * Return: int
 *	0 - On Success
 *	HTTP_INTERNAL_SERVER_ERROR - Callback is NULL
 ************************************************************************/
static int
dispatch_request( IN SOCKINFO * info,
                  http_parser_t * hparser )
{
    MiniServerCallback callback;

    switch ( hparser->msg.method ) {
            //Soap Call
        case SOAPMETHOD_POST:
        case HTTPMETHOD_MPOST:
            callback = gSoapCallback;
            break;

            //Gena Call
        case HTTPMETHOD_NOTIFY:
        case HTTPMETHOD_SUBSCRIBE:
        case HTTPMETHOD_UNSUBSCRIBE:
            //UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
            //    "miniserver %d: got GENA msg\n", info->socket );
            callback = gGenaCallback;
            break;

            //HTTP server call
        case HTTPMETHOD_GET:
        case HTTPMETHOD_POST:
        case HTTPMETHOD_HEAD:
        case HTTPMETHOD_SIMPLEGET:
            callback = gGetCallback;
            break;

        default:
            callback = NULL;
    }

    if( callback == NULL ) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    callback( hparser, &hparser->msg, info );
    return 0;
}

/************************************************************************
 * Function: handle_error
 *
 * Parameters:
 * 	IN SOCKINFO *info	- Socket Inforamtion Object
 * 	int http_error_code	- HTTP Error Code
 * 	int major		- Major Version Number
 * 	int minor		- Minor Version Number
 *
 * Description:
 * 	Send Error Message
 *
 * Return: void
 ************************************************************************/
static UPNP_INLINE void
handle_error( IN SOCKINFO * info,
              int http_error_code,
              int major,
              int minor )
{
    http_SendStatusResponse( info, http_error_code, major, minor );
}

/************************************************************************
 * Function: free_handle_request_arg
 *
 * Parameters:
 *	void *args ; Request Message to be freed
 *
 * Description:
 * 	Free memory assigned for handling request and unitialize socket
 * 	functionality
 *
 * Return: void
 ************************************************************************/
static void
free_handle_request_arg( void *args )
{
    struct mserv_request_t *request = ( struct mserv_request_t * )args;

    shutdown( request->connfd, SD_BOTH );
    UpnpCloseSocket( request->connfd );
    os_free( request );
}

/************************************************************************
 * Function: handle_request
 *
 * Parameters:
 *	void *args - Request Message to be handled
 *
 * Description:
 * 	Receive the request and dispatch it for handling
 *
 * Return: void
 ************************************************************************/
static void
handle_request( void *args )
{
    SOCKINFO info;
    int http_error_code;
    int ret_code;
    int major = 1;
    int minor = 1;
    http_parser_t parser;
    http_message_t *hmsg = NULL;
    int timeout = HTTP_DEFAULT_TIMEOUT;
    struct mserv_request_t *request = ( struct mserv_request_t * )args;
    int connfd = request->connfd;

    //UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
     //   "miniserver %d: READING\n", connfd );
    //parser_request_init( &parser ); ////LEAK_FIX_MK
    hmsg = &parser.msg;

    if( sock_init_with_ip( &info, connfd, request->foreign_ip_addr,
                           request->foreign_ip_port ) != UPNP_E_SUCCESS ) {
        os_free( request );
        httpmsg_destroy( hmsg );
        return;
    }
    // read
    ret_code = http_RecvMessage( &info, &parser, HTTPMETHOD_UNKNOWN,
                                 &timeout, &http_error_code );

    if( ret_code != 0 ) {
        goto error_handler;
    }

    //UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
    //    "miniserver %d: PROCESSING...\n", connfd );
    // dispatch
    http_error_code = dispatch_request( &info, &parser );
    if( http_error_code != 0 ) {
        goto error_handler;
    }

    http_error_code = 0;

  error_handler:
    if( http_error_code > 0 ) {
        if( hmsg ) {
            major = hmsg->major_version;
            minor = hmsg->minor_version;
        }
        handle_error( &info, http_error_code, major, minor );
    }

    //UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
    //    "miniserver %d: COMPLETE\n", connfd );
    sock_destroy( &info, SD_BOTH ); //should shutdown completely

    httpmsg_destroy( hmsg );
    os_free( request );
}

/************************************************************************
 * Function: schedule_request_job
 *
 * Parameters:
 *	IN int connfd - Socket Descriptor on which connection is accepted
 *	IN struct sockaddr_in* clientAddr - Clients Address information
 *
 * Description:
 * 	Initilize the thread pool to handle a request.
 *	Sets priority for the job and adds the job to the thread pool
 *
 * Return: void
 ************************************************************************/
static UPNP_INLINE void
schedule_request_job( IN int connfd,
                      IN struct sockaddr_in *clientAddr )
{
    struct mserv_request_t *request;
    ThreadPoolJob job;

    request =
        ( struct mserv_request_t * )
        os_alloc( sizeof( struct mserv_request_t ) );
    if( request == NULL ) {
        //UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
        //    "mserv %d: out of memory\n", connfd );
        shutdown( request->connfd, SD_BOTH );
        UpnpCloseSocket( connfd );
        return;
    }

    request->connfd = connfd;
    request->foreign_ip_addr = clientAddr->sin_addr;
    request->foreign_ip_port = ntohs( clientAddr->sin_port );

    TPJobInit( &job, ( start_routine ) handle_request, ( void * )request );
    TPJobSetFreeFunction( &job, free_handle_request_arg );
    TPJobSetPriority( &job, MED_PRIORITY );

    if( ThreadPoolAdd( &gMiniServerThreadPool, &job, NULL ) != 0 ) {
        //UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
        //    "mserv %d: cannot schedule request\n", connfd );
            os_free( request );
        shutdown( connfd, SD_BOTH );
        UpnpCloseSocket( connfd );
        return;
    }

}

/************************************************************************
 * Function: RunMiniServer
 *
 * Parameters:
 *	MiniServerSockArray *miniSock - Socket Array
 *
 * Description:
 * 	Function runs the miniserver. The MiniServer accepts a 
 *	new request and schedules a thread to handle the new request.
 *	Checks for socket state and invokes appropriate read and shutdown 
 *	actions for the Miniserver and SSDP sockets 
 *
 * Return: void
 ************************************************************************/
static void
RunMiniServer( MiniServerSockArray * miniSock )
{

    struct sockaddr_in clientAddr;
    struct timeval timeout;
    socklen_t clientLen;
    SOCKET connectHnd;
    SOCKET miniServSock = miniSock->miniServerSock;
    SOCKET miniServStopSock =  miniSock->miniServerStopSock;
    SOCKET ssdpSock = miniSock->ssdpSock;
#ifdef INCLUDE_CLIENT_APIS
    SOCKET ssdpReqSock = miniSock->ssdpReqSock;
#endif

    fd_set expSet;
    fd_set rdSet;
    unsigned int maxMiniSock;
    int byteReceived;
    //char requestBuf[256];
    char *requestBuf;//////////////////////////////

    requestBuf = os_alloc(256);///////////////////
    maxMiniSock = max( miniServSock, miniServStopSock) ;
    maxMiniSock = max( maxMiniSock, (SOCKET)(ssdpSock) );
#ifdef INCLUDE_CLIENT_APIS
    maxMiniSock = max( maxMiniSock, (SOCKET)(ssdpReqSock) );
#endif
    ++maxMiniSock;
    gMServState = MSERV_RUNNING;
    
    while( TRUE ) {
        //os_msleep(1000);//sleep 50ms, it seems don't need

        FD_ZERO( &rdSet );
        FD_ZERO( &expSet );

        FD_SET( miniServStopSock, &expSet );
        FD_SET( miniServSock, &rdSet );
        FD_SET( miniServStopSock, &rdSet );
        FD_SET( ssdpSock, &rdSet );
#ifdef INCLUDE_CLIENT_APIS
        FD_SET( ssdpReqSock, &rdSet );
#endif

        timeout.tv_sec = 0;
        timeout.tv_usec = 100*1000;
        
//the last param of select is timeout, set NULL here means block style?????????
        if( select( maxMiniSock, &rdSet, NULL, &expSet, NULL ) ==
            UPNP_SOCKETERROR ) {
           
            //UpnpPrintf( UPNP_CRITICAL, SSDP, __FILE__, __LINE__,
            //    "Error in select call!\n" );
	    /* Avoid 100% CPU in case of repeated error in select() */
	    isleep( 1 );
            continue;
        } else {
        
            if( FD_ISSET( miniServSock, &rdSet ) ) {
                clientLen = sizeof( struct sockaddr_in );
                connectHnd = accept( miniServSock,
                    ( struct sockaddr * )&clientAddr, &clientLen ); 
                if( connectHnd == UPNP_INVALID_SOCKET ) {
                    //UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
                    //    "miniserver: Error in accepting connection\n" );
                    continue;
                }
                schedule_request_job( connectHnd, &clientAddr );
            }
 
#ifdef INCLUDE_CLIENT_APIS
            // ssdp
            if( FD_ISSET( ssdpReqSock, &rdSet ) ) {
                readFromSSDPSocket( ssdpReqSock );
            }
#endif

            if( FD_ISSET( ssdpSock, &rdSet ) ) {
                    readFromSSDPSocket( ssdpSock );/////////////////////////////
            }//ssdpSock is block style now, does it need to change to nonblock?????

            if( FD_ISSET( miniServStopSock, &rdSet ) ) { 
                clientLen = sizeof( struct sockaddr_in );
                 
                memset( ( char * )&clientAddr, 0,
                        sizeof( struct sockaddr_in ) );
                  
                byteReceived =
                    recvfrom( miniServStopSock, requestBuf, 25, MSG_DONTWAIT,
                              ( struct sockaddr * )&clientAddr,
                              &clientLen );
                  
                if( byteReceived > 0 ) {                
                    requestBuf[byteReceived] = '\0';
                    //UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
                    //    "Received response !!!  %s From host %s \n",
                    //    requestBuf, inet_ntoa( clientAddr.sin_addr ) );
                    //UpnpPrintf( UPNP_PACKET, MSERV, __FILE__, __LINE__,
                    //    "Received multicast packet: \n %s\n",
                    //    requestBuf );
                    
                    if( NULL != strstr( requestBuf, "ShutDown" ) ) {
                        break;
		    }
		    
                } 
            }
        }
    }

    shutdown( miniServSock, SD_BOTH );
    UpnpCloseSocket( miniServSock );
    shutdown( miniServStopSock, SD_BOTH );
    UpnpCloseSocket( miniServStopSock );
    shutdown( ssdpSock, SD_BOTH );
    UpnpCloseSocket( ssdpSock );
#ifdef INCLUDE_CLIENT_APIS
    shutdown( ssdpReqSock, SD_BOTH );
    UpnpCloseSocket( ssdpReqSock );
#endif

    os_free( miniSock );
    os_free(requestBuf);/////////////////////////////////
    gMServState = MSERV_IDLE;

    return;

}

/************************************************************************
 * Function: get_port
 *
 * Parameters:
 *	int sockfd - Socket Descriptor 
 *
 * Description:
 * 	Returns port to which socket, sockfd, is bound.
 *
 * Return: int
 *	-1 on error; check errno
 *	 > 0 means port number
 ************************************************************************/
static int
get_port( int sockfd )
{
    struct sockaddr_in sockinfo;
    socklen_t len;
    int code;
    int port;

    len = sizeof( struct sockaddr_in );
    code = getsockname( sockfd, ( struct sockaddr * )&sockinfo, &len );
    if( code == -1 ) {
        return -1;
    }

    port = ntohs( sockinfo.sin_port );
    UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
        "sockfd = %d, .... port = %d\n", sockfd, port );

    return port;
}

/************************************************************************
 * Function: get_miniserver_sockets
 *
 * Parameters:
 *	MiniServerSockArray *out   - Socket Array
 *	unsigned short listen_port - port on which the server is
 *		listening for incoming connections	
 *
 * Description:
 * 	Creates a STREAM socket, binds to INADDR_ANY and listens for
 * 	incoming connecttions. Returns the actual port which the sockets
 * 	sub-system returned. 
 *
 *	Also creates a DGRAM socket, binds to the loop back address and 
 *	returns the port allocated by the socket sub-system.
 *
 * Return: int 
 *	UPNP_E_OUTOF_SOCKET - Failed to create a socket
 *	UPNP_E_SOCKET_BIND - Bind() failed
 *	UPNP_E_LISTEN	- Listen() failed	
 *	UPNP_E_INTERNAL_ERROR - Port returned by the socket layer is < 0
 *	UPNP_E_SUCCESS	- Success
 ************************************************************************/
int
get_miniserver_sockets( MiniServerSockArray * out,
                        unsigned short listen_port )
{
    struct sockaddr_in serverAddr;
    int listenfd;
    int success;
    unsigned short actual_port;
    int reuseaddr_on = 0;
    int sockError = UPNP_E_SUCCESS;
    int errCode = 0;
    int miniServerStopSock;

    listenfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( listenfd < 0 ) {
        return UPNP_E_OUTOF_SOCKET; // error creating socket
    }
    // As per the IANA specifications for the use of ports by applications
    // override the listen port passed in with the first available 
    if( listen_port < APPLICATION_LISTENING_PORT )
        listen_port = APPLICATION_LISTENING_PORT;

    memset( &serverAddr, 0, sizeof( serverAddr ) );
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );

    // Getting away with implementation of re-using address:port and instead 
    // choosing to increment port numbers.
    // Keeping the re-use address code as an optional behaviour that can be 
    // turned on if necessary. 
    // TURN ON the reuseaddr_on option to use the option.
    if( reuseaddr_on ) {
        //THIS IS ALLOWS US TO BIND AGAIN IMMEDIATELY
        //AFTER OUR SERVER HAS BEEN CLOSED
        //THIS MAY CAUSE TCP TO BECOME LESS RELIABLE
        //HOWEVER IT HAS BEEN SUGESTED FOR TCP SERVERS

        UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
            "mserv start: resuseaddr set\n" );
        sockError = setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR,
            ( const char * )&reuseaddr_on, sizeof( int ));
        if( sockError == UPNP_SOCKETERROR ) {
            shutdown( listenfd, SD_BOTH );
            UpnpCloseSocket( listenfd );
            return UPNP_E_SOCKET_BIND;
        }

        sockError = bind( listenfd,
                          ( struct sockaddr * )&serverAddr,
                          sizeof( struct sockaddr_in )
             );
    } else {
        do {
            serverAddr.sin_port = htons( listen_port++ );
            sockError = bind( listenfd,
                              ( struct sockaddr * )&serverAddr,
                              sizeof( struct sockaddr_in )
                 );
            if( sockError == UPNP_SOCKETERROR ) {
#ifdef WIN32
                errCode = WSAGetLastError();
#else
                errCode = errno; 
#endif
                if( errno == EADDRINUSE )
                    errCode = 1;
            } else
                errCode = 0;

        } while( errCode != 0 );
    }

    if( sockError == UPNP_SOCKETERROR ) {
        perror( "mserv start: bind failed" );
        shutdown( listenfd, SD_BOTH );
        UpnpCloseSocket( listenfd );
        return UPNP_E_SOCKET_BIND;  // bind failed
    }

    UpnpPrintf( UPNP_INFO, MSERV, __FILE__, __LINE__,
        "mserv start: bind success\n" );
//Make_Socket_NoBlocking( listenfd );//set ssdpSock to nonblock style????????????
    success = listen( listenfd, SOMAXCONN );
    if( success == UPNP_SOCKETERROR ) {
        shutdown( listenfd, SD_BOTH );
        UpnpCloseSocket( listenfd );
        return UPNP_E_LISTEN;   // listen failed
    }

    actual_port = get_port( listenfd );
    if( actual_port <= 0 ) {
        shutdown( listenfd, SD_BOTH );
        UpnpCloseSocket( listenfd );
        return UPNP_E_INTERNAL_ERROR;
    }

    out->miniServerPort = actual_port;

    if( ( miniServerStopSock = socket( AF_INET, SOCK_DGRAM, 0 ) ) ==
        UPNP_INVALID_SOCKET ) {
        UpnpPrintf( UPNP_CRITICAL,
            MSERV, __FILE__, __LINE__,
            "Error in socket operation !!!\n" );
        shutdown( listenfd, SD_BOTH );
        UpnpCloseSocket( listenfd );
        return UPNP_E_OUTOF_SOCKET;
    }

    // bind to local socket
    memset( ( char * )&serverAddr, 0, sizeof( struct sockaddr_in ) );
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
//Make_Socket_NoBlocking( miniServerStopSock );//set ssdpSock to nonblock style????????????
    if( bind( miniServerStopSock, ( struct sockaddr * )&serverAddr,
              sizeof( serverAddr ) ) == UPNP_SOCKETERROR ) {

        UpnpPrintf( UPNP_CRITICAL,
            MSERV, __FILE__, __LINE__,
            "Error in binding localhost!!!\n" );
        shutdown( listenfd, SD_BOTH );
        UpnpCloseSocket( listenfd );
        shutdown( miniServerStopSock, SD_BOTH );
        UpnpCloseSocket( miniServerStopSock );
        return UPNP_E_SOCKET_BIND;
    }

    miniStopSockPort = get_port( miniServerStopSock );
    if( miniStopSockPort <= 0 ) {
        shutdown( miniServerStopSock, SD_BOTH );
        UpnpCloseSocket( miniServerStopSock );
        shutdown( listenfd, SD_BOTH );
        UpnpCloseSocket( listenfd );
        return UPNP_E_INTERNAL_ERROR;
    }

    out->stopPort = miniStopSockPort;
    out->miniServerSock = listenfd;
    out->miniServerStopSock = miniServerStopSock;

    return UPNP_E_SUCCESS;

}

/************************************************************************
 * Function: StartMiniServer
 *
 * Parameters :
 *	unsigned short listen_port - Port on which the server listens for 
 *		incoming connections
 *
 * Description:
 * 	Initialize the sockets functionality for the 
 *	Miniserver. Initialize a thread pool job to run the MiniServer
 *	and the job to the thread pool. If listen port is 0, port is 
 *	dynamically picked
 *
 *	Use timer mechanism to start the MiniServer, failure to meet the 
 *	allowed delay aborts the attempt to launch the MiniServer.
 *
 * Return: int
 *	Actual port socket is bound to - On Success
 *	A negative number UPNP_E_XXX - On Error
 ************************************************************************/
int
StartMiniServer( unsigned short listen_port )
{
    int success;
    int count;
    int max_count = 10000;

    MiniServerSockArray *miniSocket;
    ThreadPoolJob job;

    if( gMServState != MSERV_IDLE ) {
        return UPNP_E_INTERNAL_ERROR;   // miniserver running
    }

    miniSocket =
        ( MiniServerSockArray * ) os_alloc( sizeof( MiniServerSockArray ) );
    if( miniSocket == NULL )
        return UPNP_E_OUTOF_MEMORY;

    if( ( success = get_miniserver_sockets( miniSocket, listen_port ) )
        != UPNP_E_SUCCESS ) {
        os_free( miniSocket );

        return success;
    }

    if( ( success = get_ssdp_sockets( miniSocket ) ) != UPNP_E_SUCCESS ) {
        shutdown( miniSocket->miniServerSock, SD_BOTH );
        UpnpCloseSocket( miniSocket->miniServerSock );
        shutdown( miniSocket->miniServerStopSock, SD_BOTH );
        UpnpCloseSocket( miniSocket->miniServerStopSock );
        os_free( miniSocket );

        return success;
    }

    TPJobInit( &job, ( start_routine ) RunMiniServer,
               ( void * )miniSocket );
    TPJobSetPriority( &job, MED_PRIORITY );

    TPJobSetFreeFunction( &job, ( free_routine ) os_free );

    success = ThreadPoolAddPersistent( &gMiniServerThreadPool, &job, NULL );

    if( success < 0 ) {
        shutdown( miniSocket->miniServerSock, SD_BOTH );
        UpnpCloseSocket( miniSocket->miniServerSock );
        shutdown( miniSocket->miniServerStopSock, SD_BOTH );
        UpnpCloseSocket( miniSocket->miniServerStopSock );
        shutdown( miniSocket->ssdpSock, SD_BOTH );
        UpnpCloseSocket( miniSocket->ssdpSock );
#ifdef INCLUDE_CLIENT_APIS
        shutdown( miniSocket->ssdpReqSock, SD_BOTH );
        UpnpCloseSocket( miniSocket->ssdpReqSock );
#endif

        return UPNP_E_OUTOF_MEMORY;
    }
    // wait for miniserver to start
    count = 0;
    while( gMServState != MSERV_RUNNING && count < max_count ) {
        os_usleep( 50 * 1000 );    // 0.05s
        count++;
    }

    // taking too long to start that thread
    if( count >= max_count ) {
        shutdown( miniSocket->miniServerSock, SD_BOTH );
        UpnpCloseSocket( miniSocket->miniServerSock );
        shutdown( miniSocket->miniServerStopSock, SD_BOTH );
        UpnpCloseSocket( miniSocket->miniServerStopSock );
        shutdown( miniSocket->ssdpSock, SD_BOTH );
        UpnpCloseSocket( miniSocket->ssdpSock );
#ifdef INCLUDE_CLIENT_APIS
        shutdown( miniSocket->ssdpReqSock, SD_BOTH );
        UpnpCloseSocket( miniSocket->ssdpReqSock );
#endif

        return UPNP_E_INTERNAL_ERROR;
    }

    return miniSocket->miniServerPort;/////////////////////////////
}

/************************************************************************
 * Function: StopMiniServer
 *
 * Parameters:
 *	void
 *
 * Description:
 * 	Stop and Shutdown the MiniServer and free socket 
 *	resources.
 *
 * Return: int
 *		Always returns 0 
 ************************************************************************/
int
StopMiniServer( void )
{

    int socklen = sizeof( struct sockaddr_in );
    int sock;
    struct sockaddr_in ssdpAddr;
    char buf[256] = "ShutDown";
    int bufLen = strlen( buf );

    if( gMServState == MSERV_RUNNING ) {
        gMServState = MSERV_STOPPING;
    } else {
        return 0;
    }

    sock = socket( AF_INET, SOCK_DGRAM, 0 );
    if( sock == UPNP_INVALID_SOCKET ) {
        UpnpPrintf( UPNP_INFO, SSDP, __FILE__, __LINE__,
            "SSDP_SERVER:StopSSDPServer: Error in socket operation !!!\n" );
        return 0;
    }

    while( gMServState != MSERV_IDLE ) {
        ssdpAddr.sin_family = AF_INET;
        ssdpAddr.sin_addr.s_addr = inet_addr( "127.0.0.1" );
        ssdpAddr.sin_port = htons( miniStopSockPort );
        sendto( sock, buf, bufLen, 0, ( struct sockaddr * )&ssdpAddr,
                socklen );
        os_usleep( 1000 );
        if( gMServState == MSERV_IDLE )
            break;
        isleep( 1 );
    }
    shutdown( sock, SD_BOTH );
    UpnpCloseSocket( sock );
    return 0;
}

