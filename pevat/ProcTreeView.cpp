// ProcTreeView.cpp : implementation of the CProcTreeView class
//

#include "stdafx.h"

#pragma warning( push, 3 )
# include <StrSafe.h>
# include <algorithm>
# include "Common.h"
# include "PevatDoc.h"
# include "MainFrame.h"
#pragma warning( pop )

#include "ProcTreeView.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

IMPLEMENT_DYNCREATE(CProcTreeView, CTreeView)

BEGIN_MESSAGE_MAP(CProcTreeView, CTreeView)
    ON_WM_DESTROY()
    ON_WM_RBUTTONDOWN()
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CProcTreeView::OnTvnSelChanged)
    ON_NOTIFY_REFLECT(TVN_GETINFOTIP, &CProcTreeView::OnTvnGetInfoTip)
    ON_NOTIFY_REFLECT(TVN_SELCHANGING, &CProcTreeView::OnTvnSelChanging)
    ON_NOTIFY_REFLECT(TVN_GETDISPINFO, &CProcTreeView::OnTvnGetDispInfo)
END_MESSAGE_MAP()

CProcTreeView::CProcTreeView()
: m_currentPid( PROCESS_ID_INVALID ), m_shutdown(NULL, TRUE, FALSE, NULL) { }

CProcTreeView::~CProcTreeView(){ }

BOOL CProcTreeView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= TVS_HASLINES | TVS_HASBUTTONS |
        TVS_SHOWSELALWAYS | TVS_LINESATROOT | TVS_INFOTIP;

    return CTreeView::PreCreateWindow(cs);
}

void CProcTreeView::OnInitialUpdate()
{
    CTreeView::OnInitialUpdate();

    // Scratch
    BOOL bResult = FALSE;

    bResult = RefreshProcesses();
    ASSERT( bResult );

    const INT TIMEOUT = 1000;
    INT timeout = TIMEOUT;

    CWinApp* pApplication = AfxGetApp();
    ASSERT( NULL != pApplication );
    if( NULL != pApplication )
    {
        timeout = pApplication->GetProfileInt( L"Settings", L"Process List Update Interval", TIMEOUT );
        if( timeout < 250 ) { timeout = TIMEOUT; }
    }

    const MonitorParams* pParams = new MonitorParams( timeout, m_listProcs, m_listLock, m_shutdown );
    ASSERT( NULL != pParams );

    DWORD dwResult = LaunchMonitorThread( pParams );
    ASSERT( ERROR_SUCCESS == dwResult ); UNUSED( dwResult );
}

// CProcTreeView diagnostics

#ifdef _DEBUG
void CProcTreeView::AssertValid() const
{
    CTreeView::AssertValid();
}

void CProcTreeView::Dump(CDumpContext& dc) const
{
    CTreeView::Dump(dc);
}

CPevatDoc* CProcTreeView::GetDocument()
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPevatDoc)));
    return (CPevatDoc*)m_pDocument;
}
#endif //_DEBUG

CProcTreeView* CProcTreeView::GetView()
{
    CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
    ASSERT( pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
    if( !pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) ) { return NULL; }

    return pFrame->GetProcTreeView();
}

BOOL CProcTreeView::RefreshProcesses( )
{
    // Returned to caller. Start high, pull low
    BOOL bResult = TRUE;

    // After the initial update or refresh, we want to reselect the old node
    // (in the case of the initial update, there is no 'previous node or parent).
    // If the previous node has disappeared due to attack, we want to select
    // its parent. If it is also gone, just reselect root
    INT previousProcess=PROCESS_ID_INVALID, previousParent=PROCESS_ID_INVALID;

    previousProcess = GetSelectedProcessId();
    previousParent = GetSelectedProcessParentId();

    CSingleLock lock( &m_listLock, TRUE );
    GetTreeCtrl().SetRedraw(FALSE);
    {
        bResult &= ResetView();
        ASSERT( bResult );

        bResult &= AddProcesses();
        ASSERT( bResult );
    }
    GetTreeCtrl().SetRedraw(TRUE);
    lock.Unlock();

    if( FALSE == SelectItemByProcessId( previousProcess ) )
    {
        ASSERT( previousProcess == PROCESS_ID_INVALID );
        if( FALSE == SelectItemByProcessId( previousParent ) )
        {
            ASSERT( previousParent == PROCESS_ID_INVALID );
            bResult &= SelectRootItem();
            ASSERT( bResult );
        }
    }

    //// Do we need to
    //if( previousProcess != GetSelectedProcessId() && !IsAttackInProgress() )
    //{
    // // Pass message through the document
    // CPevatDoc* pDoc = GetDocument();
    // ASSERT_VALID( pDoc );
    // if( NULL != pDoc )
    // {
    // pDoc->UpdateAllViews( this, HINT_PROCESS_CHANGED, NULL );
    // }
    //}

    return bResult;
}

BOOL CProcTreeView::ResetView()
{
    // Start high, pull low
    BOOL bResult = TRUE;

    CSingleLock( &m_listLock, TRUE );

    bResult = GetTreeCtrl().DeleteAllItems();
    m_listProcs.clear();

    ASSERT( bResult );
    return bResult;
}

HTREEITEM CProcTreeView::FindParent( const ProcessData& tvd )
{
    CSingleLock( &m_listLock, TRUE );

    HTREEITEM root = GetTreeCtrl().GetRootItem();
    if( NULL == root ) { return NULL; }

    size_t idx = (size_t)GetTreeCtrl().GetItemData( root );
    const ProcessData& data = m_listProcs[idx];

    if( data.nProcessId == tvd.nParentId ) {
        return root;
    }

    return FindParent( root, tvd );
}

HTREEITEM CProcTreeView::FindParent( HTREEITEM parent, const ProcessData& tvd )
{
    CSingleLock( &m_listLock, TRUE );

    // Base case
    if( NULL == parent ) { return NULL; }

    size_t idx = (size_t)GetTreeCtrl().GetItemData( parent );
    const ProcessData& pdata = m_listProcs[idx];

    // Match?
    if( pdata.nProcessId == tvd.nParentId ) {
        return parent;
    }

    // Check all children
    HTREEITEM child = GetTreeCtrl().GetNextItem( parent, TVGN_CHILD );
    while ( child != NULL ) {

        idx = (size_t)GetTreeCtrl().GetItemData( child );
        const ProcessData& cdata = m_listProcs[idx];

        // Match?
        if( cdata.nProcessId == tvd.nParentId ) {
            return child;
        }

        // Next child
        child = GetTreeCtrl().GetNextSiblingItem(child);
    }

    // Next, recursive on children
    child = GetTreeCtrl().GetNextItem( parent, TVGN_CHILD );
    while ( child != NULL ) {

        // Match?
        HTREEITEM grandchild = FindParent( child, tvd );
        if( NULL != grandchild ) { return grandchild; }

        // Next child
        child = GetTreeCtrl().GetNextSiblingItem(child);
    }

    return NULL;
}

BOOL CProcTreeView::SelectRootItem( )
{
    CSingleLock( &m_listLock, TRUE );

    HTREEITEM root = GetTreeCtrl().GetRootItem();
    ASSERT( root != NULL );

    return GetTreeCtrl().SelectItem( root );
}

HTREEITEM CProcTreeView::FindProcess( const ProcessData& tvd )
{
    CSingleLock( &m_listLock, TRUE );

    HTREEITEM root = GetTreeCtrl().GetRootItem();
    if( NULL == root ) { return NULL; }

    size_t idx = (size_t)GetTreeCtrl().GetItemData( root );
    const ProcessData& proc = m_listProcs[idx];

    if( proc.nProcessId == tvd.nProcessId ) {
        return root;
    }

    return FindProcess( root, tvd );
}

HTREEITEM CProcTreeView::FindProcess( HTREEITEM parent, const ProcessData& tvd )
{
    CSingleLock( &m_listLock, TRUE );

    // Base case
    if( NULL == parent ) { return NULL; }

    size_t idx = (size_t)GetTreeCtrl().GetItemData( parent );
    const ProcessData& pdata = m_listProcs[idx];

    if( pdata.nProcessId == tvd.nProcessId ) {
        return parent;
    }

    // Check all children
    HTREEITEM child = GetTreeCtrl().GetNextItem( parent, TVGN_CHILD );
    while ( child != NULL )
    {
        // Match?
        idx = (size_t)GetTreeCtrl().GetItemData( child );
        const ProcessData& cdata = m_listProcs[idx];

        if( cdata.nProcessId == tvd.nProcessId ) {
            return child;
        }

        // Next child
        child = GetTreeCtrl().GetNextSiblingItem(child);
    }

    // Next, recursive on children
    child = GetTreeCtrl().GetNextItem( parent, TVGN_CHILD );
    while ( child != NULL ) {

        // Match?
        HTREEITEM grandchild = FindProcess( child, tvd );
        if( NULL != grandchild ) { return grandchild; }

        // Next child
        child = GetTreeCtrl().GetNextSiblingItem(child);
    }

    return NULL;
}

BOOL CProcTreeView::SelectItemByProcessId( INT processID )
{
    CSingleLock( &m_listLock, TRUE );

    if( PROCESS_ID_INVALID == processID ) { return FALSE; }

    ProcessData dummy;
    dummy.nProcessId = processID;

    HTREEITEM node = FindProcess( dummy );
    ASSERT( NULL != node );
    if( NULL == node ) { return FALSE; }

    BOOL bResult = GetTreeCtrl().SelectItem( node );
    ASSERT( NULL != node );

    return bResult;
}

INT CProcTreeView::GetSelectedProcessId( ) const
{
    CSingleLock( &m_listLock, TRUE );

    HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
    // Fires if nothing is selected (ie, InitialUpdate)
    // ASSERT( NULL != hItem );
    if( NULL == hItem ) { return PROCESS_ID_INVALID; }

    size_t idx = (size_t)GetTreeCtrl().GetItemData( hItem );
    return m_listProcs[idx].nProcessId;
}

INT CProcTreeView::GetSelectedProcessParentId() const
{
    CSingleLock( &m_listLock, TRUE );

    HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
    // Fires if nothing is selected (ie, InitialUpdate)
    // ASSERT( NULL != hItem );
    if( NULL == hItem ) { return PROCESS_ID_INVALID; }

    size_t idx = (size_t)GetTreeCtrl().GetItemData( hItem );
    return m_listProcs[idx].nParentId;
}

CString CProcTreeView::GetSelectedProcessName() const
{
    CSingleLock( &m_listLock, TRUE );

    HTREEITEM hItem = GetTreeCtrl().GetSelectedItem();
    // Fires if nothing is selected (ie, InitialUpdate)
    // ASSERT( NULL != hItem );
    if( NULL == hItem ) { return CString(); }

    size_t idx = (size_t)GetTreeCtrl().GetItemData( hItem );
    return m_listProcs[idx].szName;
}

UINT CProcTreeView::GetProcessCount() const
{
    CSingleLock( &m_listLock, TRUE );
    return GetTreeCtrl().GetCount();
}

INT CProcTreeView::GetSelectedProcess() const
{
    CSingleLock( &m_listLock, TRUE );
    return GetSelectedProcessId();
}

void CProcTreeView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    switch( lHint )
    {
    case HINT_REFRESH_PROCESSES:
        RefreshProcesses();
        break;
    case HINT_PROCESS_LIST_CHANGED:
        RefreshProcesses();
        break;
    default:
        CTreeView::OnUpdate(pSender, lHint, pHint);
        break;
    }
}

void CProcTreeView::OnTvnSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    ASSERT( NULL != pNMHDR && NULL != pResult );
    if( NULL == pNMHDR || NULL == pResult ) { return; }

    // In case of early out
    const LRESULT PROCESSED = 0;
    *pResult = !PROCESSED;

    LPNMTREEVIEW pInfo = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    ASSERT( NULL != pInfo );
    if( pInfo == NULL ) { return; }

    INT previousPid = m_currentPid;

    size_t idx = (size_t)pInfo->itemNew.lParam;
    m_currentPid = m_listProcs[idx].nProcessId;

    if( m_currentPid != previousPid )
    {
        // Pass message through the document
        CPevatDoc* pDoc = GetDocument();
        ASSERT_VALID( pDoc );
        if( NULL == pDoc ) { return; }

        pDoc->UpdateAllViews( this, HINT_PROCESS_CHANGED, NULL );
    }

    *pResult = PROCESSED;
}

void CProcTreeView::OnRButtonDown(UINT nFlags, CPoint point)
{
    UINT unused = nFlags;
    HTREEITEM hItem = GetTreeCtrl().HitTest( point, &unused );
    if(hItem) {
        GetTreeCtrl().SelectItem(hItem);
    }

    CTreeView::OnRButtonDown(nFlags, point);
}

void CProcTreeView::OnDestroy()
{
    DWORD dwWait = WaitForSingleObject( m_monitorThread, 0 );
    if( WAIT_OBJECT_0 != dwWait )
    {
        SetEvent( m_shutdown.m_hObject );
        dwWait = WaitForSingleObject( m_monitorThread, 1000 );
        ASSERT( dwWait == WAIT_OBJECT_0 );
    }

    CTreeView::OnDestroy();
}

BOOL CProcTreeView::AddProcesses( )
{
    // Grab the lock
    CSingleLock( &m_listLock, TRUE );

    BOOL bResult = BuildProcessDataList( m_listProcs, m_listLock );
    ASSERT( TRUE == bResult );
    if( !bResult ) { return FALSE; }

    // Returned to caller. Start high, pull low
    bResult = TRUE;

    // Scratch for tree view items
    HTREEITEM hItem = NULL;

    for( unsigned i=0; i<(unsigned)m_listProcs.size(); i++ )
    {
        const ProcessData& proc = m_listProcs[i];

        // If no parent, insert below root
        HTREEITEM parent = FindParent( proc );
        if( NULL == parent ) { parent = GetTreeCtrl().GetRootItem(); }

        TVITEM item;
        ZeroMemory( &item, sizeof(item) );

        item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_PARAM;
        item.pszText = LPSTR_TEXTCALLBACK;
        item.cChildren = I_CHILDRENCALLBACK;
        item.lParam = (LPARAM)i;

        TVINSERTSTRUCT tvi;
        ZeroMemory( &tvi, sizeof(tvi) );

        tvi.hParent = parent;
        tvi.item = item;

        HTREEITEM pos = GetTreeCtrl().InsertItem( &tvi );
        ASSERT( NULL != pos );
        bResult &= (NULL != pos);
    }

    // Expand Level 0 and Level 1
    hItem = GetTreeCtrl().GetRootItem();
    GetTreeCtrl().Expand( hItem, TVE_EXPAND );
    HTREEITEM child = GetTreeCtrl().GetNextItem( hItem, TVGN_CHILD );
    while ( child != NULL ) {
        GetTreeCtrl().Expand( child, TVE_EXPAND );
        child = GetTreeCtrl().GetNextSiblingItem(child);
    }

    ASSERT( bResult );
    return bResult;
}

BOOL BuildProcessDataList( ProcessDataList& listProcs, CCriticalSection& listLock )
{
    // Grab the lock
    CSingleLock( &listLock, TRUE );

    // Clear the current list
    listProcs.clear();

    WCHAR wszCompName[MAX_COMPUTERNAME_LENGTH+1];
    DWORD dwSize = _countof( wszCompName );

    // Fetch computer name
    BOOL bResult = GetComputerName( wszCompName, &dwSize );
    ASSERT( TRUE == bResult );
    if( FALSE == bResult ) { return FALSE; }

    // Computer name will be 'Root'
    listProcs.push_back(
        ProcessData(PROCESS_ID_INVALID, PROCESS_ID_INVALID, wszCompName));

    // Next is 'System Idle' because its PID is 0, which causes grief
    listProcs.push_back(
        ProcessData( PROCESS_ID_INVALID, PROCESS_ID_SYSTEM, L"System Idle" ));

    do
    {
        ProcessFunctions pfuncs;
        if( NULL == pfuncs ) { break; }

        // Must use attach, otherwise the handle is duplicated (and leaked)
        AutoHandle hProcEnum;
        hProcEnum.Attach(pfuncs.pfnCreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 ));

        // Sanity check
        assert( INVALID_HANDLE_VALUE != hProcEnum );
        if( INVALID_HANDLE_VALUE == hProcEnum ) { break; }

        PROCESSENTRY32 pe;
        pe.dwSize = sizeof(PROCESSENTRY32);

        bResult = pfuncs.pfnProcess32First( hProcEnum, &pe );
        if( FALSE == bResult && ERROR_NO_MORE_FILES != GetLastError() ) { break; }

        while( bResult )
        {
            // System Process (PID 0) was added manually above
            if( PROCESS_ID_SYSTEM != pe.th32ProcessID )
            {
                listProcs.push_back(
                    ProcessData( pe.th32ParentProcessID, pe.th32ProcessID, pe.szExeFile ));
            }

            ZeroMemory( &pe, sizeof(pe) );
            pe.dwSize = sizeof( pe );
            bResult = pfuncs.pfnProcess32Next( hProcEnum, &pe );
        }

    } while(false);

    return TRUE;
}

void CProcTreeView::OnTvnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    ASSERT( NULL != pNMHDR && NULL != pResult );
    if( NULL == pNMHDR || NULL == pResult ) { return; }

    const LRESULT PROCESSED = 0;
    *pResult = !PROCESSED;

    // Grab the lock
    CSingleLock( &m_listLock, TRUE );

    LPNMTVDISPINFO pInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    size_t idx = pInfo->item.lParam;
    const ProcessData& proc = m_listProcs[idx];

    if( pInfo->item.mask & TVIF_TEXT )
    {
        if( proc.nParentId == PROCESS_ID_INVALID && proc.nProcessId == PROCESS_ID_INVALID )
        {
            StringCchCopy( pInfo->item.pszText, pInfo->item.cchTextMax, proc.szName );
        }
        else
        {
            StringCchPrintf( pInfo->item.pszText, pInfo->item.cchTextMax,
                L"%s (%d)", proc.szName, proc.nProcessId );
        }

        *pResult = PROCESSED;
    }

    if( pInfo->item.mask & TVIF_CHILDREN )
    {
        pInfo->item.cChildren = HasChild( proc.nProcessId );

        if( pInfo->item.cChildren ) { *pResult = PROCESSED; }
    }
}

// Retrieves the number of children for a process
INT CProcTreeView::GetChildCount( INT processID )
{
    INT count = 0;
    CSingleLock( &m_listLock, TRUE );

    ProcessDataList::const_iterator it = m_listProcs.begin();
    for( ; it != m_listProcs.end(); it++ )
    {
        if( processID == it->nParentId ) {
            count++;
        }
    }

    return count;
}

// Determines if a process has children
BOOL CProcTreeView::HasChild( INT processID )
{
    // Grab the lock
    CSingleLock( &m_listLock, TRUE );

    ProcessDataList::const_iterator it = m_listProcs.begin();
    for( ; it != m_listProcs.end(); it++ )
    {
        if( processID == it->nParentId ) {
            return TRUE;
        }
    }

    return FALSE;
}

void CProcTreeView::OnTvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
    ASSERT( NULL != pNMHDR && NULL != pResult );
    if( NULL == pNMHDR || NULL == pResult ) { return; }

    const LRESULT PROCESSED = 0;
    *pResult = !PROCESSED;

    LPNMTVGETINFOTIP pTipInfo = reinterpret_cast<LPNMTVGETINFOTIP>(pNMHDR);

    // Grab the lock
    CSingleLock( &m_listLock, TRUE );

    HTREEITEM hitem = pTipInfo->hItem;
    size_t idx = (size_t)GetTreeCtrl().GetItemData(hitem);
    const ProcessData& proc = m_listProcs[idx];

    const DWORD CCH = 255;
    WCHAR wsz[CCH];

    if( hitem == GetTreeCtrl().GetRootItem() )
    {
        StringCchPrintf( wsz, CCH, L"Computer" );
    }
    else
    {
        if( PROCESS_ID_INVALID != proc.nParentId )
        {
            StringCchPrintf( wsz, CCH, L"%s, Process %d, Parent %d",
                proc.szName, proc.nProcessId, proc.nParentId );
        }
        else
        {
            StringCchPrintf( wsz, CCH, L"%s, Process %d",
                proc.szName, proc.nProcessId );
        }
    }

    StringCchCopy( pTipInfo->pszText, pTipInfo->cchTextMax, wsz );

    *pResult = PROCESSED;
}

void CProcTreeView::OnTvnSelChanging(NMHDR *pNMHDR, LRESULT *pResult)
{
    ASSERT( NULL != pNMHDR && NULL != pResult );
    if( NULL == pNMHDR || NULL == pResult ) { return; }

    const LRESULT PROCESSED = 0;
    *pResult = !PROCESSED;

    LPNMTREEVIEW pInfo = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    if( !IsAttackInProgress() )
    {
        *pResult = PROCESSED;
    }
    else
    {
        size_t idx = (size_t)pInfo->itemNew.lParam;
        INT pid = m_listProcs[idx].nProcessId;
        if( pid == m_currentPid )
        {
            *pResult = PROCESSED;
        }
    }

    // Default behavior is !PROCESSED, which cancels the change
}

DWORD CProcTreeView::LaunchMonitorThread( const MonitorParams* lpParams )
{
    CWinThread* pThread = AfxBeginThread( MonitorThreadProc, (LPVOID)lpParams,
        THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL );
    DWORD dwLastError = GetLastError();

    ASSERT( NULL != pThread );
    if( NULL == pThread ) { return dwLastError; }

    // We need duplication (due to AutoDelete) so we use Assign
    m_monitorThread.Assign( pThread->m_hThread );

    pThread->m_bAutoDelete = TRUE;
    pThread->ResumeThread();

    return ERROR_SUCCESS;
}

UINT CDECL MonitorThreadProc( LPVOID lpParams )
{
    ASSERT( NULL != lpParams );
    if( NULL == lpParams ) { return ERROR_INVALID_PARAMETER; }

    // Retrieve parameters
    DWORD dwTimeout = ((MonitorParams*)lpParams)->m_dwFrequency;
    CCriticalSection& listLock = ((MonitorParams*)lpParams)->m_listLock;
    ProcessDataList& currentList = (ProcessDataList&)((MonitorParams*)lpParams)->m_currentList;
    const CEvent& shutdown = ((MonitorParams*)lpParams)->m_shutdown;

    // We got our references. Free the memory
    delete (MonitorParams*)lpParams; lpParams = NULL;

    HWND hWnd = AfxGetMainWnd()->GetSafeHwnd();
    ASSERT( NULL != hWnd );

    // Scratch for messages
    WCHAR wsz[ 255 ];

    for( ; ; )
    {
        bool bChanged = false;

        do
        {
            ProcessDataList newList;
            CCriticalSection cs;

            BOOL bResult = BuildProcessDataList( newList, cs );
            ASSERT( bResult == TRUE );
            if( !bResult ) { break; }

            // Lock the current list
            CSingleLock( &listLock, TRUE );

            // Find process which have exited
            ProcessDataList::iterator cit = currentList.begin();
            for( ; cit != currentList.end(); cit++ )
            {
                // Find the current list process in the new list
                ProcessDataList::const_iterator f = std::find( newList.begin(), newList.end(), *cit );

                if( f == newList.end() /* Not Found */ )
                {
                    bChanged = true;

                    StringCchPrintf( wsz, _countof(wsz), L"Process %d (%s) has exited.", cit->nProcessId, cit->szName );
                    if( hWnd ) { SendMessage( hWnd, MSG_LOG_MESSAGE, (WPARAM)new CString(wsz), NULL ); }
                }
            }

            // Find process which have been created
            ProcessDataList::iterator nit = newList.begin();
            for( ; nit != newList.end(); nit++ )
            {
                // Find the current list process in the new list
                ProcessDataList::const_iterator f = std::find( currentList.begin(), currentList.end(), *nit );

                if( f == currentList.end() /* Not Found */ )
                {
                    bChanged = true;

                    StringCchPrintf( wsz, _countof(wsz), L"Process %d (%s) has been created.", nit->nProcessId, nit->szName );
                    if( hWnd ) { SendMessage( hWnd, MSG_LOG_MESSAGE, (WPARAM)new CString(wsz), NULL ); }
                }
            }

            if( bChanged ) {
                if( hWnd ) { SendMessage( hWnd, MSG_PROCESS_LIST_CHANGED, NULL, NULL ); }
            }

        } while(false);

        if( WAIT_OBJECT_0 == WaitForSingleObject( shutdown.m_hObject, dwTimeout ) ) { break; }
    }

    return ERROR_SUCCESS;
}
