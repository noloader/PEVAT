// UserCredentials.cpp : implementation file
//

#include "stdafx.h"

#pragma warning( push, 3 )
# include <StrSafe.h>
#pragma warning( pop )

#include "UserCredentials.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
#  undef OutputDebugString
#  define OutputDebugString(x) (void)0;
# endif
#endif

// CUserCredentials

CUserCredentials::CUserCredentials() { }

CUserCredentials::~CUserCredentials() { }

// Retrieve the computer name
CString CUserCredentials::GetComputerName( )
{
    WCHAR wsz[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD dwSize = _countof(wsz);
    if( !::GetComputerName(wsz, &dwSize) ) {
        return L"";
    }

    return wsz;
}

// Retrieve the Domain\Usename of the current user
CString CUserCredentials::GetDomainAndUsername( )
{
    CString sz;
    if( _GetDomainAndUsername( sz ) ) { return sz; }

    return L"";
}

// Retrieve the Domain\Usename of the current user
BOOL CUserCredentials::_GetDomainAndUsername( CString& szUser )
{
    BOOL bApiResult = FALSE;

    // Thread or Process token
    HANDLE hToken = NULL;

    // Token Information
    BYTE* pTokenInfo = NULL;
    DWORD dwSize = 0;

    WCHAR* pszDomain = NULL;
    DWORD dwDomainLength = 0;
    WCHAR* pszUsername = NULL;
    DWORD dwUserLength = 0;

    __try {

        // Most threads do not have a Token - they use the process token
        bApiResult = OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken );

        // Fires too often
        // ASSERT( TRUE == bApiResult );

        // Failure with ERROR_SUCCESS - WTF...
        if( FALSE == bApiResult )
        {
            ASSERT( hToken == NULL );
            bApiResult = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken );
            ASSERT( TRUE == bApiResult );
            if( FALSE == bApiResult ) { __leave; }
        }

        bApiResult = GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
        ASSERT( FALSE == bApiResult && ERROR_INSUFFICIENT_BUFFER == GetLastError() );
        if( TRUE == bApiResult || ERROR_INSUFFICIENT_BUFFER != GetLastError() ) { __leave; }

        pTokenInfo = (BYTE*)HeapAlloc( GetProcessHeap(), 0, dwSize );
        ASSERT( NULL != pTokenInfo );
        if( NULL == pTokenInfo ) { __leave; }

        bApiResult = GetTokenInformation(hToken, TokenUser, pTokenInfo, dwSize, &dwSize);
        ASSERT( TRUE == bApiResult );
        if( FALSE == bApiResult ) { __leave; }

        TOKEN_USER* pTokenUser = (TOKEN_USER*)pTokenInfo;
        SID_NAME_USE sidUse;
        bApiResult = LookupAccountSid( NULL, pTokenUser->User.Sid,
            NULL, &dwUserLength, NULL, &dwDomainLength, &sidUse );
        ASSERT( FALSE == bApiResult );
        if( TRUE == bApiResult || ERROR_INSUFFICIENT_BUFFER != GetLastError() ) { __leave; }

        pszDomain = (WCHAR*)HeapAlloc( GetProcessHeap(), 0, dwDomainLength*sizeof(WCHAR) );
        ASSERT( NULL != pszDomain );
        if( NULL == pszDomain ) { __leave; }

        pszUsername = (WCHAR*)HeapAlloc( GetProcessHeap(), 0, dwUserLength*sizeof(WCHAR) );
        ASSERT( NULL != pszUsername );
        if( NULL == pszUsername ) { __leave; }

        bApiResult = LookupAccountSid( NULL, pTokenUser->User.Sid,
            pszUsername, &dwUserLength, pszDomain, &dwDomainLength, &sidUse );

        ASSERT( TRUE == bApiResult );
        ASSERT( SidTypeUser == sidUse );

        if( FALSE == bApiResult || SidTypeUser != sidUse ) { __leave; }

        szUser.Empty();
        szUser.Preallocate( dwDomainLength + 1 + dwUserLength );

        szUser += pszDomain;
        szUser += L'\\';
        szUser += pszUsername;
    }
    __finally {
        if( NULL != pszUsername ) {
            HeapFree( GetProcessHeap(), 0, pszUsername );
            pszUsername = NULL;
        }
        if( NULL != pszDomain ) {
            HeapFree( GetProcessHeap(), 0, pszDomain );
            pszDomain = NULL;
        }
        if( NULL != pTokenInfo ) {
            HeapFree( GetProcessHeap(), 0, pTokenInfo );
            pTokenInfo = NULL;
        }
        if( NULL != hToken ) {
            CloseHandle( hToken ); hToken = NULL;
        }
    }

    return TRUE == bApiResult;
}

// Retrieve the Domain\Usename of the from the token
CString CUserCredentials::GetDomainAndUsername( HANDLE hToken )
{
    CString sz;
    if( _GetDomainAndUsername( hToken, sz ) ) { return sz; }

    return L"";
}

// Retrieve the Domain\Usename of the from the token
BOOL CUserCredentials::_GetDomainAndUsername( /*In*/ HANDLE hToken, /*Out*/ CString& szUser )
{
    BOOL bApiResult = FALSE;

    // Token Information
    BYTE* pTokenInfo = NULL;
    DWORD dwSize = 0;

    WCHAR* pszDomain = NULL;
    DWORD dwDomainLength = 0;
    WCHAR* pszUsername = NULL;
    DWORD dwUserLength = 0;

    __try {

        bApiResult = GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
        ASSERT( FALSE == bApiResult );
        if( TRUE == bApiResult || ERROR_INSUFFICIENT_BUFFER != GetLastError() ) { __leave; }

        pTokenInfo = (BYTE*)HeapAlloc( GetProcessHeap(), 0, dwSize );
        ASSERT( NULL != pTokenInfo );
        if( NULL == pTokenInfo ) { __leave; }

        bApiResult = GetTokenInformation(hToken, TokenUser, pTokenInfo, dwSize, &dwSize);
        ASSERT( TRUE == bApiResult );
        if( FALSE == bApiResult ) { __leave; }

        TOKEN_USER* pTokenUser = (TOKEN_USER*)pTokenInfo;
        SID_NAME_USE sidUse;
        bApiResult = LookupAccountSid( NULL, pTokenUser->User.Sid,
            NULL, &dwUserLength, NULL, &dwDomainLength, &sidUse );
        ASSERT( FALSE == bApiResult );
        if( TRUE == bApiResult || ERROR_INSUFFICIENT_BUFFER != GetLastError() ) { __leave; }

        pszDomain = (WCHAR*)HeapAlloc( GetProcessHeap(), 0, dwDomainLength*sizeof(WCHAR) );
        ASSERT( NULL != pszDomain );
        if( NULL == pszDomain ) { __leave; }

        pszUsername = (WCHAR*)HeapAlloc( GetProcessHeap(), 0, dwUserLength*sizeof(WCHAR) );
        ASSERT( NULL != pszUsername );
        if( NULL == pszUsername ) { __leave; }

        bApiResult = LookupAccountSid( NULL, pTokenUser->User.Sid,
            pszUsername, &dwUserLength, pszDomain, &dwDomainLength, &sidUse );

        ASSERT( TRUE == bApiResult );
        ASSERT( SidTypeUser == sidUse );

        if( FALSE == bApiResult ) { __leave; }
        if( SidTypeUser != sidUse ) { __leave; }

        szUser = pszDomain;
        szUser += L'\\';
        szUser += pszUsername;
    }
    __finally {
        if( NULL != pszUsername ) {
            HeapFree( GetProcessHeap(), 0, pszUsername );
            pszUsername = NULL;
        }
        if( NULL != pszDomain ) {
            HeapFree( GetProcessHeap(), 0, pszDomain );
            pszDomain = NULL;
        }
        if( NULL != pTokenInfo ) {
            HeapFree( GetProcessHeap(), 0, pTokenInfo );
            pTokenInfo = NULL;
        }
    }

    return TRUE == bApiResult;
}

// Prompts the user for alternate credentials

HANDLE CUserCredentials::PromptAlternateAndLogon( HWND hWnd )
{
    // Returned to User
    HANDLE hToken = NULL;

    // credui.dll
    HMODULE hModule = NULL;

    // CredUIPromptForCredentials
    CREDUI_INFO credinfo;
    ZeroMemory( &credinfo, sizeof(credinfo) );

    // CredUIPromptForCredentials
    credinfo.cbSize = sizeof(credinfo);
    credinfo.hwndParent = hWnd;
    credinfo.pszCaptionText = L"Alternate Credentials";
    credinfo.pszMessageText = L"Please enter the alternate credentials";

    // CredUIPromptForCredentials
    WCHAR wszDomain[CREDUI_MAX_DOMAIN_TARGET_LENGTH+1];
    WCHAR wszUsername[CREDUI_MAX_USERNAME_LENGTH+1];
    WCHAR wszPassword[CREDUI_MAX_PASSWORD_LENGTH+1];

    // In/Out parameters
    ZeroMemory( wszUsername, sizeof(wszUsername) );
    ZeroMemory( wszPassword, sizeof(wszPassword) );

    // CredUIParseUserName does not allow overlapping buffers
    WCHAR* pwszDomainUser = NULL;
    DWORD length = 0;

    DWORD dwReturn = ERROR_SUCCESS;

    __try {

        // Target is the local machine
        WCHAR wszTarget[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD dwSize = _countof(wszTarget);
        if( !::GetComputerName(wszTarget, &dwSize) ) { __leave; }

        DWORD dwReason = 0;
        BOOL bSave = FALSE;
        DWORD dwFlags = CREDUI_FLAGS_ALWAYS_SHOW_UI |
            CREDUI_FLAGS_GENERIC_CREDENTIALS;

        hModule = LoadLibrary( L"credui.dll" );
        ASSERT( NULL != hModule );
        if( NULL == hModule ) { __leave; }

        #pragma warning( suppress: 6387 )
        CREDUIPROMPTFORCREDENTIALS pfnCredUIPromptForCredentials =
            (CREDUIPROMPTFORCREDENTIALS)GetProcAddress(
            hModule, "CredUIPromptForCredentialsW" );
        ASSERT( NULL != pfnCredUIPromptForCredentials );
        if( NULL == pfnCredUIPromptForCredentials ) { __leave; }

        dwReturn = pfnCredUIPromptForCredentials(
            &credinfo, wszTarget, NULL, dwReason, wszUsername, _countof(wszUsername),
            wszPassword, _countof(wszPassword), &bSave, dwFlags );

        // User may have cancelled
        if( ERROR_SUCCESS != dwReturn ) { __leave; }

        // Copy the Domain\UserName since CredUIParseUserName
        // does not handle overlapping buffers
        length = (DWORD)wcslen(wszUsername)+1;
        pwszDomainUser = new WCHAR[ length ];
        ASSERT( NULL != pwszDomainUser );
        if( NULL == pwszDomainUser ) { __leave; }

        StringCchCopy( pwszDomainUser, length, wszUsername );
        
        CREDUIPARSEUSERNAME pfnCredUIParseUserName =
            #pragma warning( suppress: 6387 )
            (CREDUIPARSEUSERNAME)GetProcAddress( hModule, "CredUIParseUserNameW" );
        ASSERT( NULL != pfnCredUIParseUserName );
        if( NULL == pfnCredUIParseUserName ) { __leave; }

        dwReturn = pfnCredUIParseUserName( pwszDomainUser,
            wszUsername, _countof(wszUsername),
            wszDomain, _countof(wszDomain) );

        if( ERROR_SUCCESS != dwReturn ) { __leave; }

        if( !LogonUser( wszUsername, wszDomain, wszPassword,
            LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &hToken ) ) {
                __leave;
        }
    }
    __finally {

        SecureZeroMemory( wszUsername, sizeof(wszUsername));
        SecureZeroMemory( wszPassword, sizeof(wszPassword));

        if( NULL != pwszDomainUser ) {
            SecureZeroMemory( pwszDomainUser, length);
            delete[] pwszDomainUser;
            pwszDomainUser = NULL;
        }

        if( NULL != hModule ) {
            FreeLibrary( hModule );
            hModule = NULL;
        }
    }

    return hToken;
}

// Logs off the previously logged on user
void CUserCredentials::Logoff( HANDLE& hToken )
{
    if( NULL != hToken ) {
        if( CloseHandle( hToken ) ) {
            hToken = NULL;
        }
    }    
}
