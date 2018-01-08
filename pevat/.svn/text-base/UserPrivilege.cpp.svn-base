#include "StdAfx.h"

#include "UserPrivilege.h"
#include <cassert>

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
#  undef OutputDebugString
#  define OutputDebugString(x) (void)0;
# endif
#endif

CUserPrivilege::CUserPrivilege( ){ };

CUserPrivilege::~CUserPrivilege( ){ };

BOOL CUserPrivilege::EnablePrivilege( LPCTSTR pszPrivilege, BOOL bEnabled ){

    // Returned to caller
    BOOL bResult = FALSE;

    // Thread or Process token
    HANDLE hToken = NULL;

    __try {

        bResult = OpenThreadToken( GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES, TRUE, &hToken );
        if( !bResult )
        {
            bResult = OpenProcessToken(GetCurrentProcess(),
                TOKEN_ADJUST_PRIVILEGES, &hToken );
            assert( TRUE == bResult );
            if( FALSE == bResult ) { __leave; }
        }

        bResult = EnablePrivilege( hToken, pszPrivilege, bEnabled );
        DWORD dwError = GetLastError();
        assert( TRUE == bResult || ( ERROR_SUCCESS == GetLastError() ||
            ERROR_NOT_ALL_ASSIGNED == dwError ) );

        // We're only enabling one privilege. If we get back
        // ERROR_NOT_ALL_ASSIGNED, we failed.
        if( ERROR_NOT_ALL_ASSIGNED == dwError ) { bResult = FALSE; }
    }

    __finally {
        if( NULL != hToken ) {
            CloseHandle( hToken ); hToken = NULL;
        }
    }

    return bResult;
}

BOOL CUserPrivilege::EnablePrivilege( HANDLE hToken, LPCTSTR pszPrivilege, BOOL bEnabled )
{
    BOOL bResult = FALSE;

    __try {

        LUID luid;
        TOKEN_PRIVILEGES priv;

        bResult = LookupPrivilegeValue( NULL, pszPrivilege, &luid );
        assert( TRUE == bResult );
        if( FALSE == bResult ) { __leave; }

        priv.PrivilegeCount = 1;
        priv.Privileges[0].Luid = luid;
        priv.Privileges[0].Attributes =
            (bEnabled ? SE_PRIVILEGE_ENABLED : FALSE );

        bResult = AdjustTokenPrivileges( hToken, FALSE, &priv, sizeof(priv), NULL, NULL );

        // We're only enabling one privilege. If we get back
        // ERROR_NOT_ALL_ASSIGNED, we failed.
        if( ERROR_NOT_ALL_ASSIGNED == GetLastError() ) { bResult = FALSE; }

        if( FALSE == bResult ) { __leave; }

        bResult = TRUE;
    }
    __finally { ; }

    return bResult;
}

CUserPrivilege::Status CUserPrivilege::HasPrivilege( LPCTSTR pszPrivilege )
{
    // Returned to caller
    Status status = Error;

    // Thread or Process token
    HANDLE hToken = NULL;

    // Scratch
    BOOL bResult = FALSE;

    __try {

        bResult = OpenThreadToken( GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken );
        if( !bResult )
        {
            bResult = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken );
            assert( TRUE == bResult );
            if( FALSE == bResult ) { __leave; }
        }

        status = HasPrivilege( hToken, pszPrivilege );

        //bResult = EnablePrivilege( hToken, pszPrivilege, bEnabled );
        //DWORD dwError = GetLastError();
        //assert( TRUE == bResult || ( ERROR_SUCCESS == GetLastError() ||
        //    ERROR_NOT_ALL_ASSIGNED == dwError ) );

        //// We're only enabling one privilege. If we get back
        //// ERROR_NOT_ALL_ASSIGNED, we failed.
        //if( ERROR_NOT_ALL_ASSIGNED == dwError ) { bResult = FALSE; }
    }

    __finally {
        if( NULL != hToken ) {
            CloseHandle( hToken ); hToken = NULL;
        }
    }

    return status;
}

CUserPrivilege::Status CUserPrivilege::HasPrivilege( HANDLE hToken, LPCTSTR pszPrivilege )
{
    // Returned to caller
    Status status = Error;

    // Scratch
    BOOL bResult = FALSE;

    PBYTE pBuffer = NULL;
    TOKEN_PRIVILEGES* pTokenInfo = NULL;

    __try
    {
        LUID uid = { 0, 0 };
        bResult = LookupPrivilegeValue( NULL, pszPrivilege, &uid );
        assert( TRUE == bResult );
        if( FALSE == bResult ) { __leave; }

        DWORD dwRequired = 0;
        bResult = GetTokenInformation( hToken, TokenPrivileges, NULL, 0, &dwRequired );
        assert( FALSE == bResult );
        if( TRUE == bResult || 0 == dwRequired ) { __leave; }

        pBuffer = new BYTE[dwRequired];
        assert( NULL != pBuffer );
        if( NULL == pBuffer ) { __leave; }

        DWORD dwSize = dwRequired;
        bResult = GetTokenInformation( hToken, TokenPrivileges, pBuffer, dwSize, &dwRequired );
        assert( TRUE == bResult );
        if( FALSE == bResult || dwSize != dwRequired ) { __leave; }

        pTokenInfo = (TOKEN_PRIVILEGES*)pBuffer;
        DWORD count = pTokenInfo->PrivilegeCount;

        // Status changed...
        status = Missing;

        for( DWORD i = 0; i<count; i++ )
        {
            if( pTokenInfo->Privileges[i].Luid.HighPart == uid.HighPart && 
                pTokenInfo->Privileges[i].Luid.LowPart == uid.LowPart )
            {
                DWORD attrib = pTokenInfo->Privileges[i].Attributes;
                if( attrib == SE_PRIVILEGE_ENABLED ||
                    attrib == SE_PRIVILEGE_ENABLED_BY_DEFAULT )
                {
                    status = Enabled;
                }
                else if( attrib == SE_PRIVILEGE_REMOVED )
                {
                    status = Disabled;
                }

                break;
            }
        }
    }
    __finally
    {
        if( NULL != pBuffer ) {
            delete[] pBuffer; pBuffer = NULL;
        }
    }

    return status;
}

wostream& operator<<(wostream& out, const CUserPrivilege&)
{
    // User privileges
    CUserPrivilege::Status status;

    status = CUserPrivilege::HasDebugPrivilege();
    out << L"Debug programs: " << status << endl;

    status = CUserPrivilege::HasTcbPrivilege();
    out << L"Act as part of the operating system: " << status << endl;

    status = CUserPrivilege::HasSecurityPrivilege();
    out << L"Manage auditing and security log: " << status << endl;

    //status = CUserPrivilege::HasTakeOwnershipPrivilege();
    //out << L"Take ownership of files and other objects: ";
    //status == CUserPrivilege::Enabled ? ( out << L"Enabled" ) : ( out << L"Disabled" );
    //out << endl;

    //status = CUserPrivilege::HasAssignPrimaryTokenPrivilege();
    //out << L"Replace process level token: ";
    //status == CUserPrivilege::Enabled ? ( out << L"Enabled" ) : ( out << L"Disabled" );
    //out << endl;

    //status = CUserPrivilege::HasBackupPrivilege();
    //out << L"Backup files and directories: ";
    //status == CUserPrivilege::Enabled ? ( out << L"Enabled" ) : ( out << L"Disabled" );
    //out << endl;

    //status = CUserPrivilege::HasRestorePrivilege();
    //out << L"Restore files and directories: ";
    //status == CUserPrivilege::Enabled ? ( out << L"Enabled" ) : ( out << L"Disabled" );
    //out << endl;

    return out;
}

wostream& operator<<(wostream& out, const CUserPrivilege::Status& status)
{
    switch((int)status)
    {
    case CUserPrivilege::Enabled:
        out << L"enabled"; break;
    case CUserPrivilege::Disabled:
        out << L"disabled"; break;
    case CUserPrivilege::Missing:
        out << L"missing"; break;
    case CUserPrivilege::Error:
        out << L"error"; break;

    default: ;
    }

    return out;
}