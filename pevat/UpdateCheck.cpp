#include "StdAfx.h"

#include "UpdateCheck.h"
#include "VersionInfo.h"
#include "UpdateWarnDlg.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

DWORD LaunchUpdateThread()
{
	return ERROR_SUCCESS;

#if 0
    CWinApp* pApplication = AfxGetApp();
    ASSERT( NULL != pApplication );
    if( NULL != pApplication )
    {
        INT warn = 0;
        warn = pApplication->GetProfileInt( L"Settings", L"No Update Warning", 0 );

        if( warn == 0 )
        {
            CUpdateWarnDlg dlg;
            dlg.DoModal();
        }
    }

    CWinThread* pThread = AfxBeginThread( UpdateThreadProc, NULL,
        THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL );
    DWORD dwLastError = GetLastError();

    ASSERT( NULL != pThread );
    if( NULL == pThread ) { return dwLastError; }

    pThread->m_bAutoDelete = TRUE;
    pThread->ResumeThread();

    return ERROR_SUCCESS;
#endif
}

UINT CDECL UpdateThreadProc( LPVOID /*lpParams*/ )
{
	return ERROR_SUCCESS;

#if 0
    DWORD dwLastError = ERROR_SUCCESS;
    INT nResult = 0;

    const char DEFAULT_SITE[] = "version.softwareintegrity.com";
    const char DEFAULT_PAGE[] = "/versioninfo.txt";
    const char DEFAULT_PORT[] = "80";

    // Receive buffer
    const int BUFFER_SIZE = 1024 * 4;
    char rbuffer[ BUFFER_SIZE ];

    // WinSock
    WSADATA wsaData = { 0 };
    addrinfo hints, *info = NULL;
    ZeroMemory( &hints, sizeof(hints) );
    SOCKET s = INVALID_SOCKET;

    ostringstream request;

    string name, version;

    GetProductName( name );
    GetProductVersion( version );

    // An HTTP 1.1 request
    request << "GET " << DEFAULT_PAGE << " HTTP/1.1\r\n";
    request << "Host: " << DEFAULT_SITE << "\r\n";
    request << "User-Agent: " << name << " Update Check";

#ifdef WIN64
    request << " (x64); ";
#else
    request << " (x86); ";
#endif

    request << version << "\r\n";
    request << "Accept: text/plain" << "\r\n";
    request << "\r\n";

    try
    {
        // Initialize Winsock
        nResult = WSAStartup( MAKEWORD(1,1), &wsaData );
        dwLastError = GetLastError();
        assert( NO_ERROR == nResult );
        if( NO_ERROR != nResult )
        {
            ostringstream emessage;
            emessage << "WSAStartup failed: " << nResult;
            throw SocketException( emessage.str().c_str() );
        }

        // Resolve the server address and port
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        nResult = getaddrinfo( DEFAULT_SITE, DEFAULT_PORT, &hints, &info );
        dwLastError = WSAGetLastError();
        assert( NO_ERROR == nResult );
        if ( NO_ERROR != nResult )
        {
            ostringstream emessage;
            emessage << "getaddrinfo failed: " << nResult;
            throw SocketException( emessage.str().c_str() );
        }

        // Create a SOCKET for connecting to server
        s = socket( info->ai_family, info->ai_socktype, info->ai_protocol );
        dwLastError = WSAGetLastError();
        assert( INVALID_SOCKET != s );
        if(INVALID_SOCKET == s )
        {
            ostringstream emessage;
            emessage << "Socket error: " << dwLastError;
            throw SocketException( emessage.str().c_str() );
        }

        int rbuf_size = 1024 * 4;
        nResult = setsockopt( s, SOL_SOCKET, SO_RCVBUF, (const char *)&rbuf_size, sizeof( rbuf_size ) );
        dwLastError = WSAGetLastError();
        assert( SOCKET_ERROR != nResult );
        //if( SOCKET_ERROR == nResult )
        //{
        // // Non-fatal
        // cerr << "Unable to scale TCP window: ";
        // cerr << dwLastError
        //}

        // Connect to server
        nResult = connect( s, info->ai_addr, (int)info->ai_addrlen);
        dwLastError = WSAGetLastError();
        assert( SOCKET_ERROR != nResult );
        if( SOCKET_ERROR == nResult )
        {
            ostringstream emessage;
            emessage << "Socket connect error: " << dwLastError;
            throw SocketException( emessage.str().c_str() );
        }

        // Send the request
        nResult = send( s, request.str().c_str(), (int)request.str().length(), 0 );
        dwLastError = WSAGetLastError();
        assert( SOCKET_ERROR != nResult );
        assert( (size_t)nResult == request.str().length() );
        if( SOCKET_ERROR == nResult )
        {
            ostringstream emessage;
            emessage << "Socket send error: " << dwLastError;
            throw SocketException( emessage.str().c_str() );
        }

        // Receive the response
        nResult = recv( s, rbuffer, BUFFER_SIZE, 0 );
        dwLastError = WSAGetLastError();
        assert( SOCKET_ERROR != nResult );
        assert( 0 != nResult );
        assert( nResult < BUFFER_SIZE );
        if( SOCKET_ERROR == nResult )
        {
            ostringstream emessage;
            emessage << "Socket read error: " << dwLastError;
            throw SocketException( emessage.str().c_str() );
        }

        if( 0 == nResult )
        {
            ostringstream emessage;
            emessage << "Empty response from server";
            throw HttpException( emessage.str().c_str() );
        }

        if( BUFFER_SIZE == nResult )
        {
            ostringstream emessage;
            emessage << "Receive buffer too small";
            throw HttpException( emessage.str().c_str() );
        }

        string response = string( rbuffer, nResult );
        string header, body;

        if( false == GetHeaderAndBody( response, header, body ) )
        {
            ostringstream emessage;
            emessage << "Unable to parse response" << endl;
            throw HttpException( emessage.str().c_str() );
        }

        int code = 0;
        if( false == GetResponseCode( header, code ) )
        {
            ostringstream emessage;
            emessage << "Unable to determine response code" << endl;
            throw HttpException( emessage.str().c_str() );
        }

        if( 200 != code )
        {
            ostringstream emessage;
            emessage << "Html response code: " << code << endl;
            throw HttpException( emessage.str().c_str() );
        }

        if( version == body )
        {
            HWND hWnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
            ::SendMessage( hWnd, MSG_UPDATE_AVAILABLE, NULL, NULL );
        }

        nResult = ERROR_SUCCESS;
    }

    catch( SocketException& /*e*/ )
    {
        // cerr << endl << e.what() << endl;
    }

    catch( HttpException& /*e*/ )
    {
        // cerr << endl << e.what() << endl;
    }

    catch(...)
    {
        // cerr << "Caught unhandled exception" << endl;
    }

    if( INVALID_SOCKET != s )
    { closesocket( s ); s = INVALID_SOCKET; }

    if( NULL != info )
    { freeaddrinfo(info); info = NULL; }

    WSACleanup();

    return dwLastError;
#endif
}

bool GetProductName( string& name )
{
    WCHAR filename[MAX_PATH];
    if( !GetModuleFileName(NULL, filename, _countof(filename)) ) { return false; }

    CVersionInfo info;
    if( !info.Load( filename ) ) { return false; }

    name.clear();
    name.resize(255);

    size_t size = 0;
    errno_t err = wcstombs_s(&size, &name[0], name.size(),
        info.GetProductName(), lstrlen(info.GetProductName())+1);
    if( 0 != err ) { return false; }

    // size includes terminating NULL
    name.resize(size-1);

    return true;
}

bool GetProductVersion( string& version )
{
    WCHAR filename[MAX_PATH];
    if( !GetModuleFileName(NULL, filename, _countof(filename)) ) { return false; }

    CVersionInfo info;
    if( !info.Load( filename ) ) { return false; }

    version.clear();
    version.resize(255);

    size_t size = 0;
    errno_t err = wcstombs_s(&size, &version[0], version.size(),
        info.GetProductVersionAsString(), lstrlen(info.GetProductVersionAsString())+1);
    if( 0 != err ) { return false; }

    // size includes terminating NULL
    version.resize(size-1);

    return true;
}

bool GetHeaderAndBody( const string& response, string& header, string& body )
{
    string temp = response;
    std::transform(temp.begin(), temp.end(), temp.begin(), tolower);

    string::size_type pos = temp.find( "\r\n\r\n" );
    if( string::npos == pos ) { return false; }
    pos += strlen( "\r\n\r\n" );

    header = response.substr( 0, pos );
    body = response.substr( pos+1, string::npos );

    return true;
}

bool GetResponseCode( const string& header, int& code )
{
    if( string::npos == header.find( "HTTP" ) )
    { return false; }

    string::size_type start = header.find( " " );
    if( string::npos == start ) { return false; }
    start += strlen( " " );

    string::size_type end = header.find( " ", start );
    if( string::npos == end ) { return false; }

#ifdef _DEBUG
    string s;
    stringstream ss( s = header.substr( start, end - start ) );
#else
    stringstream ss( header.substr( start, end - start ) );
#endif

    if( ( ss >> code ).fail() ) { return false; }

    return true;
}
