#pragma once

#pragma warning( push, 3 )
# include <WinCred.h>
#pragma warning( pop )

// CUserCredentials command target

class CUserCredentials
{
public:
	CUserCredentials();
	virtual ~CUserCredentials();
    // Prompts the user for alternate credentials and attemps a logon
    static HANDLE PromptAlternateAndLogon( HWND hWnd = NULL );
    // Logs off the previously logged on user
    static VOID Logoff( HANDLE& hToken );

    // Retrieve the computer name
    static CString GetComputerName( );

    // Retrieve the Domain\Usename of the current user
    static CString GetDomainAndUsername( );

    // Retrieve the Domain\Usename of the from the token
    static CString GetDomainAndUsername( HANDLE hToken );

private:
    static BOOL _GetDomainAndUsername( /*Out*/ CString& szUser );
    static BOOL _GetDomainAndUsername( /*In*/ HANDLE hToken, /*Out*/ CString& szUser );
};

// We want the data structures, but not the library dependencies.
// We'll locate the functions on the fly.

// Recommended for XP
typedef DWORD (WINAPI * CREDUIPROMPTFORCREDENTIALS)
(PCREDUI_INFOW,PCWSTR,PCtxtHandle,DWORD,PWSTR,ULONG,PWSTR,ULONG,BOOL*,DWORD);

typedef DWORD (WINAPI * CREDUIPARSEUSERNAME)
(WCHAR*,WCHAR*,ULONG,WCHAR*,ULONG);
