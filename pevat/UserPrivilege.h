#pragma once

#include <windows.h>
#include <ostream>
using std::wostream;
using std::endl;

//
// http://msdn.microsoft.com/en-us/library/aa446619(VS.85).aspx
// http://support.microsoft.com/kb/q132958/
//

class CUserPrivilege
{
public:
    enum Status { Missing = -1, Error = 0, Disabled = 1, Enabled = 2 };

public:
    CUserPrivilege( );
    virtual ~CUserPrivilege( );

    // Enable SeTcbPrivilege (SE_TCB_NAME)
    static BOOL EnableTcbPrivilege( ) {
        return EnablePrivilege(SE_TCB_NAME, TRUE);
    }

    // Enable SeDebugPrivilege (SE_DEBUG_NAME)
    static BOOL EnableDebugPrivilege( ) {
        return EnablePrivilege(SE_DEBUG_NAME, TRUE);
    }

    // Enable SeSecurityPrivilege (SE_SECURITY_NAME)
    static BOOL EnableSecurityPrivilege( ) {
        return EnablePrivilege(SE_SECURITY_NAME, TRUE);
    }

    // Enable SeAssignPrimaryTokenPrivilege (SE_ASSIGNPRIMARYTOKEN_NAME)
    static BOOL EnableAssignPrimaryTokenPrivilege( ) {
        return EnablePrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, TRUE);
    }

    // Enable SeTakeOwnershipNamePrivilege (SE_TAKE_OWNERSHIP_NAME)
    static BOOL EnableTakeOwnershipPrivilege( ) {
        return EnablePrivilege(SE_TAKE_OWNERSHIP_NAME, TRUE);
    }

    // Enable SeIncreaseQuotaPrivilege (SE_INCREASE_QUOTA_NAME)
    static BOOL EnableIncreaseQuotaPrivilege( ) {
        return EnablePrivilege(SE_INCREASE_QUOTA_NAME, TRUE);
    }

    // Enable SeBackupPrivilege (SE_BACKUP_NAME)
    static BOOL EnableBackupPrivilege( ) {
        return EnablePrivilege(SE_BACKUP_NAME, TRUE);
    }

    // Enable SeBackupPrivilege (SE_BACKUP_NAME)
    static BOOL EnableRestorePrivilege( ) {
        return EnablePrivilege(SE_RESTORE_NAME, TRUE);
    }

    /////////////////////////////////////////////////

    // Disable SeTcbPrivilege (SE_TCB_NAME)
    static BOOL DisableTcbPrivilege( ) {
        return EnablePrivilege(SE_TCB_NAME, FALSE);
    }

    // Disable SeDebugPrivilege (SE_DEBUG_NAME)
    static BOOL DisableDebugPrivilege( ) {
        return EnablePrivilege(SE_DEBUG_NAME, FALSE);
    }

    // Disable SeSecurityPrivilege (SE_SECURITY_NAME)
    static BOOL DisableSecurityPrivilege( ) {
        return EnablePrivilege(SE_SECURITY_NAME, FALSE);
    }

    // Disable SeAssignPrimaryTokenPrivilege (SE_ASSIGNPRIMARYTOKEN_NAME)
    static BOOL DisableAssignPrimaryTokenPrivilege( ) {
        return EnablePrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, FALSE);
    }

    // Disable SeTakeOwnershipNamePrivilege (SE_TAKE_OWNERSHIP_NAME)
    static BOOL DisableTakeOwnershipPrivilege( ) {
        return EnablePrivilege(SE_TAKE_OWNERSHIP_NAME, FALSE);
    }

    // Disable SeIncreaseQuotaPrivilege (SE_INCREASE_QUOTA_NAME)
    static BOOL DisableIncreaseQuotaPrivilege( ) {
        return EnablePrivilege(SE_INCREASE_QUOTA_NAME, FALSE);
    }

    // Disable SeBackupPrivilege (SE_BACKUP_NAME)
    static BOOL DisableBackupPrivilege( ) {
        return EnablePrivilege(SE_BACKUP_NAME, FALSE);
    }

    // Disable SeBackupPrivilege (SE_BACKUP_NAME)
    static BOOL DisableRestorePrivilege( ) {
        return EnablePrivilege(SE_RESTORE_NAME, FALSE);
    }

    /////////////////////////////////////////////////

    // Enable SeTcbPrivilege (SE_TCB_NAME)
    static Status HasTcbPrivilege( ) {
        return HasPrivilege(SE_TCB_NAME);
    }

    // Has SeDebugPrivilege (SE_DEBUG_NAME)
    static Status HasDebugPrivilege( ) {
        return HasPrivilege(SE_DEBUG_NAME);
    }

    // Has SeSecurityPrivilege (SE_SECURITY_NAME)
    static Status HasSecurityPrivilege( ) {
        return HasPrivilege(SE_SECURITY_NAME);
    }

    // Has SeAssignPrimaryTokenPrivilege (SE_ASSIGNPRIMARYTOKEN_NAME)
    static Status HasAssignPrimaryTokenPrivilege( ) {
        return HasPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME);
    }

    // Has SeTakeOwnershipNamePrivilege (SE_TAKE_OWNERSHIP_NAME)
    static Status HasTakeOwnershipPrivilege( ) {
        return HasPrivilege(SE_TAKE_OWNERSHIP_NAME);
    }

    // Has SeIncreaseQuotaPrivilege (SE_INCREASE_QUOTA_NAME)
    static Status HasIncreaseQuotaPrivilege( ) {
        return HasPrivilege(SE_INCREASE_QUOTA_NAME);
    }

    // Has SeBackupPrivilege (SE_BACKUP_NAME)
    static Status HasBackupPrivilege( ) {
        return HasPrivilege(SE_BACKUP_NAME);
    }

    // Has SeRestorePrivilege (SE_BACKUP_NAME)
    static Status HasRestorePrivilege( ) {
        return HasPrivilege(SE_RESTORE_NAME);
    }

    /////////////////////////////////////////////////

    static Status HasPrivilege( LPCTSTR pszPrivilege );

    static Status HasPrivilege( HANDLE hToken, LPCTSTR pszPrivilege );

    // Sets or clears a privilge for the process by name
    static BOOL EnablePrivilege( LPCTSTR pszPrivilege, BOOL bEnabled = TRUE);

    // Sets or clears a privilge for a token by name
    static BOOL EnablePrivilege( HANDLE hToken, LPCTSTR pszPrivilege, BOOL bEnabled = TRUE );
};

wostream& operator<<(wostream& out, const CUserPrivilege&);
wostream& operator<<(wostream& out, const CUserPrivilege::Status&);
