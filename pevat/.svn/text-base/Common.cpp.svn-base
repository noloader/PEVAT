#include "StdAfx.h"

#include "Common.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
#  undef OutputDebugString
#  define OutputDebugString(x) (void)0;
# endif
#endif

extern AutoHandle g_hEvntProcessing;

//
//string WideStringToNarrowString( const wstring& str )
//{
//    if( str.empty() ) { return string(); }
//
//    string ret;
//
//    do
//    {
//        // WideCharToMultiByte includes the NULL. Fixup at exit
//
//        const DWORD dwFlags = WC_NO_BEST_FIT_CHARS;
//        int cbRequired = WideCharToMultiByte( CP_THREAD_ACP, dwFlags,
//            str.c_str(), -1, NULL, 0, NULL, NULL );
//        if( cbRequired == 0 ) { break; }
//
//        ret.resize( cbRequired );
//
//        int cbWritten = WideCharToMultiByte( CP_THREAD_ACP, dwFlags,
//            str.c_str(), -1, (LPSTR)ret.c_str(), (int)ret.size(), NULL, NULL );
//        if( cbWritten == 0 ) { ret.clear(); break; }
//
//        ret.resize( cbWritten - 1 );
//
//#   pragma warning( suppress: 4127 )
//    } while(false);
//
//    return ret;
//}


DWORD TestEvent(HANDLE handle)
{
    // No Asserts in this routine
    DWORD dwWait = WaitForSingleObject(handle, 0);

    if( dwWait != WAIT_OBJECT_0 && dwWait != WAIT_TIMEOUT )
    {
        dwWait = GetLastError();
    }

    return dwWait;
}

BOOL IsAttackInProgress()
{
    return WAIT_OBJECT_0 == WaitForSingleObject( g_hEvntProcessing, 0 );
}

wstring EventStateAsString(HANDLE handle)
{
    wstring state = L"Unknown";

    DWORD dwWait = TestEvent(handle);
    switch( dwWait )
    {
    case WAIT_OBJECT_0:
        state = L"Signaled"; break;
    case WAIT_TIMEOUT:
        state = L"Nonsignaled"; break;
    default: ;
    }

    return state;
}

wstring EventAccessAsString(DWORD access)
{
    wstring wsz = L"Unknown";

    switch(access)
    {
        case EVENT_ALL_ACCESS:
            wsz = L"All"; break;
        case EVENT_MODIFY_STATE | SYNCHRONIZE:
            wsz = L"Mod|Sync"; break;
        case EVENT_MODIFY_STATE:
            wsz = L"Mod"; break;
        case SYNCHRONIZE:
            wsz = L"Sync"; break;
        default:
            wsz = L"";
    }

    return wsz;
}

bool operator==( const ProcessData& lhs, const ProcessData& rhs )
{
    return lhs.nProcessId == rhs.nProcessId &&
        lhs.nParentId == rhs.nParentId &&
        lhs.szName == rhs.szName;
}

MonitorParams::MonitorParams
( DWORD dwFrequency, const ProcessDataList& currentList, CCriticalSection& listLock, CEvent& threadShutdown )
: m_dwFrequency(dwFrequency), m_currentList(currentList), m_listLock(listLock), m_shutdown(threadShutdown) { };

AutoBuffer::AutoBuffer( size_t size )
{
    Allocate( size );
}

void AutoBuffer::Allocate( size_t size )
{
    m_buffer.clear( );
    m_buffer.resize( size );
}

void AutoBuffer::Reallocate( size_t size )
{
    if( DOUBLE_CURRENT == size )
        size = m_buffer.size() * 2;

    m_buffer.resize( size );
}

AutoBuffer::operator LPVOID()
{
    if( 0 == m_buffer.size() )
        return NULL;

    return &m_buffer[0];
}

AutoHandle::AutoHandle(): m_handle(NULL) { }

// Copy with duplicate
AutoHandle::AutoHandle(HANDLE h): m_handle(NULL)
{
    // Close previous handle as required
    Close();

    // Duplicate the handle
    Assign(h);
}

// Copy with duplicate
AutoHandle::AutoHandle(const AutoHandle& h): m_handle(NULL)
{
    // Close previous handle as required
    Close();

    // Duplicate the handle
    Assign(h.m_handle);
}

// Assignment with duplicate
AutoHandle& AutoHandle::operator=(const AutoHandle& h)
{
    if( this == &h ) { return *this; }

    // Close previous handle as required
    Close();

    // Duplicate the handle
    Assign(h.m_handle);

    return *this;
}

// Assignment with duplicate
AutoHandle& AutoHandle::operator=(HANDLE h)
{
    // Close previous handle as required
    Close();

    // Duplicate the handle
    Assign(h);

    return *this;
}

AutoHandle::~AutoHandle()
{ 
    Close(); m_handle = NULL;
}

// Attach a handle to this. The handle is not duplicated. Handle will be closed during detach or destruction.
void AutoHandle::Attach( HANDLE h )
{
    // Close previous handle as required
    Close();

    m_handle = h;
}

// Detach a handle from this. Handle will be closed during detach or destruction.
void AutoHandle::Detach( )
{
    // Close previous handle as required
    Close();
}

// Assigns a handle to this. The handle is duplicated.
void AutoHandle::Assign( HANDLE h )
{
    // Close previous handle as required
    Close();

    bool b = !!DuplicateHandle( GetCurrentProcess(), h,
        GetCurrentProcess(), &m_handle, 0, 0, DUPLICATE_SAME_ACCESS );
    // Fires too often
    // assert( false != b );
    if( false == b ) { m_handle = NULL; }
}

// Does not duplicate
AutoHandle::operator HANDLE()
{
    return m_handle;
}

// Duplicate this handle
HANDLE AutoHandle::Duplcate() const
{
    HANDLE handle = NULL;
    bool b = !!DuplicateHandle( GetCurrentProcess(), m_handle,
        GetCurrentProcess(), &handle, 0, 0, DUPLICATE_SAME_ACCESS );
    assert( false != b );
    if( false == b ) { handle = NULL; }

    return handle;
}

// Deatch a handle from this *without* closing. Caller is responsible for CloseHandle.
void AutoHandle::Unattach( )
{
    m_handle = NULL;
}

// Closes the handle associated with this.
void AutoHandle::Close()
{
    // Though we use NULL, we honor INVALID_HANDLE_VALUE
    if( NULL != m_handle && INVALID_HANDLE_VALUE != m_handle ) {
        CloseHandle( m_handle );
    }

    m_handle = NULL;
}


HandleFunctions::HandleFunctions()
: m_hModule( NULL ),
pfnNtQuerySystemInformation( NULL ),
pfnNtQueryObject( NULL ),
pfnNtDuplicateObject( NULL ),
pfnNtClose( NULL )
{
    m_hModule = LoadLibrary( L"ntdll.dll" );
    assert( NULL != m_hModule );
    if( NULL == m_hModule ) {
        throw ProgramException( L"Unable to load ntdll.dll" );
    }

    pfnNtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)
        GetProcAddress(m_hModule, "NtQuerySystemInformation");
    pfnNtQueryObject = (NTQUERYOBJECT)
        GetProcAddress(m_hModule, "NtQueryObject");
    pfnNtDuplicateObject = (NTDUPLICATEOBJECT)
        GetProcAddress(m_hModule, "NtDuplicateObject");
    pfnNtClose = (NTCLOSE)
        GetProcAddress(m_hModule, "NtClose");

    assert( NULL != pfnNtQuerySystemInformation );
    assert( NULL != pfnNtQueryObject );
    assert( NULL != pfnNtDuplicateObject );
    assert( NULL != pfnNtClose );

    if( NULL == pfnNtQuerySystemInformation ) {
        throw ProgramException( L"Unable to locate NtQuerySystemInformation" );
    }
    if( NULL == pfnNtQueryObject ) {
        throw ProgramException( L"Unable to locate NtNtQueryObject" );
    }
    if( NULL == pfnNtDuplicateObject ) {
        throw ProgramException( L"Unable to locate NtDuplicateObject" );
    }
    if( NULL == pfnNtClose ) {
        throw ProgramException( L"Unable to locate NtClose" );
    }
}

HandleFunctions::~HandleFunctions()
{
    if( NULL != m_hModule ) {
        FreeLibrary( m_hModule ); m_hModule = NULL;
    }
}

ProcessFunctions::ProcessFunctions()
: m_hModule(NULL),
pfnCreateToolhelp32Snapshot(NULL),
pfnProcess32First(NULL),
pfnProcess32Next(NULL),
pfnModule32First(NULL)
{
	m_hModule = LoadLibrary( L"Kernel32.dll" );
	assert( NULL != m_hModule );
    if( NULL == m_hModule ) {
        throw ProgramException( L"Unable to load Kernel32.dll" );
    }

	pfnCreateToolhelp32Snapshot = (CREATETOOLHELP32SNAPSHOT)
		GetProcAddress( m_hModule, "CreateToolhelp32Snapshot" );
	assert( NULL != pfnCreateToolhelp32Snapshot );

	pfnProcess32First = (PROCESS32FIRST)
		GetProcAddress( m_hModule, "Process32FirstW" );
	assert( NULL != pfnProcess32First );

	pfnProcess32Next = (PROCESS32NEXT)
		GetProcAddress( m_hModule, "Process32NextW" );
	assert( NULL != pfnProcess32Next );

    pfnModule32First = (MODULE32FIRST)
        GetProcAddress( m_hModule, "Module32FirstW" );
    assert( NULL != pfnModule32First );

    if( NULL == pfnCreateToolhelp32Snapshot ) {
        throw ProgramException( L"Unable to locate CreateToolhelp32Snapshot" );
    }
    if( NULL == pfnProcess32First ) {
        throw ProgramException( L"Unable to locate Process32First" );
    }
    if( NULL == pfnProcess32Next ) {
        throw ProgramException( L"Unable to locate Process32Next" );
    }
    if( NULL == pfnModule32First ) {
        throw ProgramException( L"Unable to locate Module32First" );
    }
}

ProcessFunctions::~ProcessFunctions()
{
    if( NULL != m_hModule ) {
        FreeLibrary( m_hModule ); m_hModule = NULL;
    }
}

Wow64ProcFunctions::Wow64ProcFunctions()
: m_hModule( NULL), pfnIsWow64Process(NULL)
{
	m_hModule = LoadLibrary( L"Kernel32.dll" );
	assert( NULL != m_hModule );
    if( NULL == m_hModule ) { return; }

    pfnIsWow64Process = (ISWOW64PROCESS)
		GetProcAddress( m_hModule, "IsWow64Process" );
	assert( NULL != pfnIsWow64Process );
};

Wow64ProcFunctions::~Wow64ProcFunctions()
{
    if( NULL != m_hModule ) {
        FreeLibrary( m_hModule ); m_hModule = NULL;
    }
}
