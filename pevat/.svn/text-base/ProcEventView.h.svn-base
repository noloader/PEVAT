#pragma once

#include "Common.h"
#include "PevatApp.h"
#include "PevatDoc.h"

class CProcEventView: public CListView
{
    DECLARE_DYNCREATE(CProcEventView)

    enum Column { Number=0, Dacl, Access, State, Name };

public:
    static CProcEventView* GetView();
    INT GetTotalEvents() const { return m_total; }
    INT GetNamedEvents() const { return m_named; }
    INT GetUnnamedEvents() const{ return m_unnamed; }    

protected:
    CProcEventView();
    virtual ~CProcEventView();

    BOOL BuildAttackTableAll( AttackHandleList& list, BOOL reverse = FALSE );
    BOOL BuildAttackTableSelected( AttackHandleList& list, BOOL reverse = FALSE );

protected:
    CPevatDoc* GetDocument();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual void OnInitialUpdate();

    afx_msg void OnLvnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

    DECLARE_MESSAGE_MAP()

protected:
    // Attack Operations
    BOOL AttackInvertAll();
    BOOL AttackInvertSelected();
    BOOL AttackPulseAll();
    BOOL AttackPulseSelected();
    BOOL AttackSetAll();
    BOOL AttackSetSelected();
    BOOL AttackResetAll();
    BOOL AttackResetSelected();

    BOOL BuildAttackTable( AttackHandleList& list, BOOL selected = FALSE );
    DWORD LauchAttackThread( DWORD dwOptions = 0 );

protected:
    // View Operations
    BOOL ResetView();

    BOOL RefreshEventHandles();
    BOOL AddEventHandles();

protected:
    // Handle Operations
    BOOL DuplicateEventHandles(INT pid);
    BOOL DuplicateEventHandle(const HANDLE& hProcess, const SYSTEM_HANDLE& syshandle);

    BOOL QueryEventHandleName( EventHandle& handle );
    BOOL QueryEventHandleDacl( EventHandle& handle );
    BOOL QueryEventHandleState( EventHandle& handle );

    enum { MenuNotFound = -1 };
    INT FindMenuPosition( CMenu* pMenu, LPCWSTR pszText );

private:

    HandleFunctions m_hfuncs;

    CCriticalSection m_listHandleLock;
    HandleTableList m_listHandles;

    CCriticalSection m_listAttackLock;
    AttackHandleList m_listAttack;

    // Stats
    INT m_named;
    INT m_unnamed;
    INT m_total;
};

// The attack thread
UINT CDECL AttackThreadProc( LPVOID lpParams );
BOOL IsWellKnownEvent( LPCWSTR pwsz );

#ifndef _DEBUG// debug version in ProcEventView.cpp
inline CPevatDoc* CProcEventView::GetDocument()
{ return reinterpret_cast<CPevatDoc*>(m_pDocument); }
#endif