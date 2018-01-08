// ProcTreeView.h : interface of the CProcTreeView class
//

#pragma once

#pragma warning( push, 3 )
# include "PevatApp.h"
# include "PevatDoc.h"
# include "Common.h"
#pragma warning( pop )

class CProcTreeView: public CTreeView
{
    DECLARE_DYNCREATE(CProcTreeView)

public:
    static CProcTreeView* GetView();
    UINT GetProcessCount() const;
    INT GetSelectedProcess() const;
    CString GetSelectedProcessName() const;

protected: // create from serialization only
    CProcTreeView();
    virtual ~CProcTreeView();

    CPevatDoc* GetDocument();

protected:
    // Selects the root of the tree view
    BOOL SelectRootItem( );
    // Selects a process in the tree view based on procees id
    BOOL SelectItemByProcessId( INT processID );
    // Retrieves the process id of the selected item
    INT GetSelectedProcessId( ) const;
    // Retrieves the process id of the selected item's parent
    INT GetSelectedProcessParentId( ) const;
    // Determines if a process has children
    inline BOOL HasChild( INT processID );
    // Retrieves the number of children for a process
    inline INT GetChildCount( INT processID );

protected:
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate(); // called first time after construct

    afx_msg void OnTvnGetInfoTip( NMHDR *pNMHDR, LRESULT *pResult );
    afx_msg void OnTvnSelChanging(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnSelChanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnDestroy();

    DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    HTREEITEM FindProcess( const ProcessData& tvd );
    HTREEITEM FindProcess( HTREEITEM parent, const ProcessData& tvd );

    BOOL ResetView();

    HTREEITEM FindParent( const ProcessData& tvd );
    HTREEITEM FindParent( HTREEITEM parent, const ProcessData& tvd );

    BOOL AddProcesses( );    

    BOOL RefreshProcesses( );

    DWORD LaunchMonitorThread( const MonitorParams* pParams );

private:
    INT m_currentPid;
    mutable CCriticalSection m_listLock;
    ProcessDataList m_listProcs;

    AutoHandle m_monitorThread;
    CEvent m_shutdown;
};

BOOL BuildProcessDataList( ProcessDataList& listProcs, CCriticalSection& listLock );

UINT CDECL MonitorThreadProc( LPVOID lpParams );

#ifndef _DEBUG// debug version in ProcTreeView.cpp
inline CPevatDoc* CProcTreeView::GetDocument()
{ return reinterpret_cast<CPevatDoc*>(m_pDocument); }
#endif
