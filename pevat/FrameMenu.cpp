// MainFrame.Menucpp : implementation of the CMainFrame menu items
//

#include "stdafx.h"

#include "PevatApp.h"
#include "PevatDoc.h"
#include "StrSafe.h"
#include "LogView.h"
#include "MainFrame.h"
#include "AttackView.h"
#include "ProcTreeView.h"
#include "ProcEventView.h"
#include "InfoView.h"
#include "UserPrivilege.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
#  undef OutputDebugString
#  define OutputDebugString(x) (void)0;
# endif
#endif

void CMainFrame::OnUpdateViewRefresh(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    pCmdUI->Enable();
}

void CMainFrame::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }

    CProcEventView* pView = GetProcEventView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return; }

    if( 0 == pView->GetListCtrl().GetSelectedCount() ) {
        pCmdUI->Enable(FALSE);
    }
    else {
        pCmdUI->Enable(TRUE);
    }
}

void CMainFrame::OnViewRefresh()
{
    CPevatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return; }

    pDoc->UpdateAllViews( NULL, HINT_REFRESH_PROCESSES, 0 );
}

void CMainFrame::OnUpdatePrivilegesDebug(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    if( !EnablePrivilegesMenuItem(pCmdUI) ) { return; }

    if( CUserPrivilege::Enabled == CUserPrivilege::HasDebugPrivilege() )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnUpdatePrivilegesBackup(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    if( !EnablePrivilegesMenuItem(pCmdUI) ) { return; }

    if( CUserPrivilege::Enabled == CUserPrivilege::HasBackupPrivilege() )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnUpdatePrivilegesAssignToken(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    if( !EnablePrivilegesMenuItem(pCmdUI) ) { return; }

    if( CUserPrivilege::Enabled == CUserPrivilege::HasAssignPrimaryTokenPrivilege() )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnUpdatePrivilegesActAsOS(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    if( !EnablePrivilegesMenuItem(pCmdUI) ) { return; }

    if( CUserPrivilege::Enabled == CUserPrivilege::HasTcbPrivilege() )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnUpdatePrivilegesSecurity(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    if( !EnablePrivilegesMenuItem(pCmdUI) ) { return; }

    if( CUserPrivilege::Enabled == CUserPrivilege::HasSecurityPrivilege() )
    {
        pCmdUI->SetCheck(TRUE);
    }
    else
    {
        pCmdUI->SetCheck(FALSE);
    }
}

void CMainFrame::OnPrivilegesDebug()
{
    if( CUserPrivilege::Enabled == CUserPrivilege::HasDebugPrivilege() )
    {
        if( CUserPrivilege::DisableDebugPrivilege() ) {
            CLogView::WriteLine( L"Disabled debug privilege" );
        }
    }
    else
    {
        if( CUserPrivilege::EnableDebugPrivilege() ) {
            CLogView::WriteLine( L"Enabled debug privilege" );
        }
    }

    UpdatePrivilegMenuItem();
}

void CMainFrame::OnPrivilegesSecurity()
{
    if( CUserPrivilege::Enabled == CUserPrivilege::HasSecurityPrivilege() )
    {
        if( CUserPrivilege::DisableSecurityPrivilege() ) {
            CLogView::WriteLine( L"Disabled security privilege" );
        }
    }
    else
    {
        if( CUserPrivilege::EnableSecurityPrivilege() ) {
            CLogView::WriteLine( L"Enabled security privilege" );
        }
    }

    UpdatePrivilegMenuItem();
}

void CMainFrame::OnPrivilegesBackup()
{
    if( CUserPrivilege::Enabled == CUserPrivilege::HasBackupPrivilege() )
    {
        if( CUserPrivilege::DisableBackupPrivilege() ) {
            CLogView::WriteLine( L"Disabled backup privilege" );
        }
    }
    else
    {
        if( CUserPrivilege::EnableBackupPrivilege() ) {
            CLogView::WriteLine( L"Enabled backup privilege" );
        }
    }

    UpdatePrivilegMenuItem();
}

void CMainFrame::OnUpdatePrivilegesAssignToken()
{
    if( CUserPrivilege::Enabled == CUserPrivilege::HasAssignPrimaryTokenPrivilege() )
    {
        if( CUserPrivilege::DisableAssignPrimaryTokenPrivilege() ) {
            CLogView::WriteLine( L"Disabled assign token privilege" );
        }
    }
    else
    {
        if( CUserPrivilege::EnableAssignPrimaryTokenPrivilege() ) {
            CLogView::WriteLine( L"Enabled assign token privilege" );
        }
    }

    UpdatePrivilegMenuItem();
}

void CMainFrame::OnPrivilegesTcb()
{
    if( CUserPrivilege::Enabled == CUserPrivilege::HasTcbPrivilege() )
    {
        if( CUserPrivilege::DisableTcbPrivilege() ) {
            CLogView::WriteLine( L"Disabled TCB privilege" );
        }
    }
    else
    {
        if( CUserPrivilege::EnableTcbPrivilege() ) {
            CLogView::WriteLine( L"Enabled TCB privilege" );
        }
    }

    UpdatePrivilegMenuItem();
}

BOOL CMainFrame::EnablePrivilegesMenuItem(CCmdUI *pCmdUI)
{
    // Can only change if *no* attack is in progress
    if( IsAttackInProgress() )
    {
        pCmdUI->Enable(FALSE);
        return FALSE;
    }

    return TRUE;
}

void CMainFrame::UpdatePrivilegMenuItem()
{
    CPevatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return; }

    pDoc->UpdateAllViews( NULL, HINT_PRIVILEGE_CHANGED, NULL );
}

//////////////////////////////////////
//////////////////////////////////////

void CMainFrame::OnUpdateTotalEvents(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    pCmdUI->Enable();

    CProcEventView* pView = GetProcEventView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return; }

    const DWORD CCH = 16;
    WCHAR wsz[CCH];

    StringCchPrintf( wsz, CCH, L"Total: %d", pView->GetTotalEvents() );
    pCmdUI->SetText( wsz );
}

void CMainFrame::OnUpdateNamedEvents(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    pCmdUI->Enable();

    CProcEventView* pView = GetProcEventView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return; }

    const DWORD CCH = 16;
    WCHAR wsz[CCH];

    StringCchPrintf( wsz, CCH, L"Named: %d", pView->GetNamedEvents() );
    pCmdUI->SetText( wsz );
}

void CMainFrame::OnUpdateUnnamedEvents(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }
    pCmdUI->Enable();

    CProcEventView* pView = GetProcEventView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return; }

    const DWORD CCH = 16;
    WCHAR wsz[CCH];

    StringCchPrintf( wsz, CCH, L"Unnamed: %d", pView->GetUnnamedEvents() );
    pCmdUI->SetText( wsz );
}

void CMainFrame::OnUpdateAttackInvert(CCmdUI *pCmdUI)
{
    UpdateAttackMenuItem(pCmdUI, FALSE);
}

void CMainFrame::OnUpdateAttackPulse(CCmdUI *pCmdUI)
{
    UpdateAttackMenuItem(pCmdUI, FALSE);
}

void CMainFrame::OnUpdateAttackSet(CCmdUI *pCmdUI)
{
    UpdateAttackMenuItem(pCmdUI, FALSE);
}

void CMainFrame::OnUpdateAttackReset(CCmdUI *pCmdUI)
{
    UpdateAttackMenuItem(pCmdUI, FALSE);
}

void CMainFrame::OnUpdateAttackInvertAll(CCmdUI *pCmdUI)
{
    UpdateAttackMenuItem(pCmdUI, TRUE);
}

void CMainFrame::OnUpdateAttackPulseAll(CCmdUI *pCmdUI)
{
    UpdateAttackMenuItem(pCmdUI, TRUE);
}

void CMainFrame::OnUpdateAttackSetAll(CCmdUI *pCmdUI)
{
    UpdateAttackMenuItem(pCmdUI, TRUE);
}

void CMainFrame::OnUpdateAttackResetAll(CCmdUI *pCmdUI)
{
    UpdateAttackMenuItem(pCmdUI, TRUE);
}

void CMainFrame::UpdateAttackMenuItem(CCmdUI *pCmdUI, BOOL All)
{
    if( !pCmdUI ) { return; }

    CProcEventView* pView = GetProcEventView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return; }

    // If no items are selected *or* and attack is in progress,
    // then do not make the Attack menus available
    BOOL AnyItems = FALSE;

    if( All )
    {
        AnyItems = (0 != pView->GetListCtrl().GetItemCount());
    }
    else
    {
        AnyItems = (0 != pView->GetListCtrl().GetSelectedCount());
    }

    if( AnyItems && !IsAttackInProgress() )
    {
        pCmdUI->Enable(TRUE);
    }
    else
    {
        pCmdUI->Enable(FALSE);
    }
}

void CMainFrame::OnAttackInvert()
{
    //AttackInvert();
    OnAttackMenuItem(HINT_ATTACK_INVERTSELECTED);
}

void CMainFrame::OnAttackPulse()
{
    //AttackPulse();
    OnAttackMenuItem(HINT_ATTACK_PULSESELECTED);
}

void CMainFrame::OnAttackSet()
{
    //AttackSet();
    OnAttackMenuItem(HINT_ATTACK_SETSELECTED);
}

void CMainFrame::OnAttackReset()
{
    //AttackReset();
    OnAttackMenuItem(HINT_ATTACK_RESETSELECTED);
}

void CMainFrame::OnAttackInvertAll()
{
    //AttackInvertAll();
    OnAttackMenuItem(HINT_ATTACK_INVERTALL);
}

void CMainFrame::OnAttackPulseAll()
{
    //AttackPulseAll();
    OnAttackMenuItem(HINT_ATTACK_PULSEALL);
}

void CMainFrame::OnAttackSetAll()
{
    //AttackSetAll();
    OnAttackMenuItem(HINT_ATTACK_SETALL);
}

void CMainFrame::OnAttackResetAll()
{
    //AttackResetAll();
    OnAttackMenuItem(HINT_ATTACK_RESETALL);
}

void CMainFrame::OnAttackMenuItem(UINT message)
{
    CPevatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return; }

    pDoc->UpdateAllViews( NULL, message, 0 );
}

