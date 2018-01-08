#pragma once

#include "Common.h"

// For proper WinSock 2.0 inclusion. See
//  http://msdn.microsoft.com/en-us/library/ms737629(VS.85).aspx
#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif

#pragma warning( push, 3 )
# include <winsock2.h>
# include <ws2tcpip.h>
# include <windows.h>
# include <assert.h>
# include <algorithm>
#pragma warning( pop )

using std::endl;
using std::string;
using std::exception;
using std::stringstream;
using std::ostringstream;

class SocketException: public exception
{
public:
    SocketException( const char* msg )
        : exception( msg ) { };
};

class HttpException: public exception
{
public:
    HttpException( const char* msg )
        : exception( msg ) { };
};

bool GetProductName( string& name );
bool GetProductVersion( string& version );

bool GetHeaderAndBody( const string& response, string& header, string& body );
bool GetResponseCode( const string& header, int& code );

DWORD LaunchUpdateThread();
UINT CDECL UpdateThreadProc( LPVOID lpParams );