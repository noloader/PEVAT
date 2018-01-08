// ProcEventView.cpp : implementation file
//

#include "stdafx.h"

#include "PevatApp.h"
#include "Common.h"
#include "LogView.h"
#include "MainFrame.h"
#include "AttackView.h"
#include "ProcTreeView.h"
#include "ProcEventView.h"

using std::wostringstream;

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

IMPLEMENT_DYNCREATE(CProcEventView, CListView)

CProcEventView::CProcEventView()
: m_named(0), m_unnamed(0), m_total(0) { }

CProcEventView::~CProcEventView(){ }

BEGIN_MESSAGE_MAP(CProcEventView, CListView)
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CProcEventView::OnLvnGetDispInfo)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CProcEventView diagnostics

#ifdef _DEBUG
void CProcEventView::AssertValid() const
{
    CListView::AssertValid();
}

CPevatDoc* CProcEventView::GetDocument()
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPevatDoc)));
    return (CPevatDoc*)m_pDocument;
}

void CProcEventView::Dump(CDumpContext& dc) const
{
    CListView::Dump(dc);
}
#endif //_DEBUG

CProcEventView* CProcEventView::GetView()
{
    CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
    ASSERT( pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
    if( !pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) ) { return NULL; }

    return pFrame->GetProcEventView();
}

BOOL CProcEventView::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CListView::PreCreateWindow(cs) )
        return FALSE;

    // default is report view and full row selection
    cs.style &= ~LVS_TYPEMASK;
    cs.style |= LVS_SHOWSELALWAYS;
    cs.style |= LVS_REPORT;

    return TRUE;
}

void CProcEventView::OnInitialUpdate()
{
    CListView::OnInitialUpdate();

    DWORD dwStyle = GetListCtrl().GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP;
    GetListCtrl().SetExtendedStyle(dwStyle);

    CRect r;
    GetClientRect( &r );

    const DWORD COLUMN_MIN_WIDTH = 60;
    const DWORD COLUMN_0_WIDTH = 60;
    const DWORD COLUMN_1_WIDTH = 60;
    const DWORD COLUMN_2_WIDTH = 60;
    const DWORD COLUMN_3_WIDTH = 85;

    INT vsb = GetSystemMetrics(SM_CXVSCROLL);
    INT w = r.Width() - COLUMN_0_WIDTH - COLUMN_1_WIDTH - COLUMN_2_WIDTH - COLUMN_3_WIDTH - (vsb+1);
    w < COLUMN_MIN_WIDTH ? (w = COLUMN_MIN_WIDTH) : w;

    GetListCtrl().InsertColumn( Number, L"Number", LVCFMT_LEFT, COLUMN_0_WIDTH );
    GetListCtrl().InsertColumn( Dacl, L"DACL", LVCFMT_LEFT, COLUMN_1_WIDTH );
    GetListCtrl().InsertColumn( Access, L"Access", LVCFMT_LEFT, COLUMN_2_WIDTH );
    GetListCtrl().InsertColumn( State, L"State", LVCFMT_LEFT, COLUMN_3_WIDTH );
    GetListCtrl().InsertColumn( Name, L"Name", LVCFMT_LEFT, w );
}

void CProcEventView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    CPevatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return CListView::OnUpdate( pSender, lHint, pHint ); }

    switch( lHint )
    {
    case HINT_UPDATE_HANDLE_TABLE:
    case HINT_PROCESS_CHANGED:
        {
            RefreshEventHandles();

            if( NULL != pDoc ) {
                pDoc->UpdateAllViews( this, HINT_HANDLETABLE_CHANGED, NULL );
            }
            break;
        }
    case HINT_UPDATE_HANDLE_TABLE_ITEM:
        {
            INT item = (INT)pHint;
            ASSERT( item >= 0 );
            ASSERT( item < GetListCtrl().GetItemCount() );

            // We're reading, but not locking (I know)
            // Need to cut in SWMR locking

            EventHandle& ehandle = m_listHandles[item];
            ehandle.m_state = EventStateAsString( ehandle.m_handle );

            if( NULL != pDoc ) {
                GetListCtrl().SetItemText( item, State, ehandle.m_state.c_str() );
            }
            break;
        }
    case HINT_PRIVILEGE_CHANGED:
        {
            ResetView();
            RefreshEventHandles();

            if( NULL != pDoc ) {
                pDoc->UpdateAllViews( this, HINT_HANDLETABLE_CHANGED, NULL );
            }
            break;
        }
    case HINT_ATTACK_INVERTSELECTED:
        AttackInvertSelected();
        break;
    case HINT_ATTACK_INVERTALL:
        AttackInvertAll();
        break;
    case HINT_ATTACK_PULSESELECTED:
        AttackPulseSelected();
        break;
    case HINT_ATTACK_PULSEALL:
        AttackPulseAll();
        break;
    case HINT_ATTACK_SETSELECTED:
        AttackSetSelected();
        break;
    case HINT_ATTACK_SETALL:
        AttackSetAll();
        break;
    case HINT_ATTACK_RESETSELECTED:
        AttackResetSelected();
        break;
    case HINT_ATTACK_RESETALL:
        AttackResetAll();
        break;

    default:
        CListView::OnUpdate( pSender, lHint, pHint );
        break;
    }
}

BOOL CProcEventView::BuildAttackTable( AttackHandleList& list, BOOL selected )
{
    BOOL reverse = FALSE;
    CAttackView* pView = CAttackView::GetView();
    ASSERT( NULL != pView );
    if( pView ) {
        reverse = pView->GetReverseList();
    }

    CSingleLock( &m_listHandleLock, TRUE );
    list.clear();

    BOOL bResult = FALSE;
    if( selected ) {
        bResult = BuildAttackTableSelected(list, reverse);
        ASSERT( bResult );
    }
    else {
        bResult = BuildAttackTableAll(list, reverse);
        ASSERT( bResult );
    }

    return bResult;
}

BOOL CProcEventView::BuildAttackTableAll( AttackHandleList& list, BOOL reverse )
{
    CSingleLock( &m_listHandleLock, TRUE );

    INT count = GetListCtrl().GetItemCount();
    for( INT i = 0; i <count; i++ )
    {
        size_t idx = (size_t)GetListCtrl().GetItemData(i);
        list.push_back(AttackHandle(i, m_listHandles[idx].m_handle,m_listHandles[idx].m_name ));
    }

    if( reverse ) {
        std::reverse( list.begin(), list.end() );
    }

    return TRUE;
}

BOOL CProcEventView::BuildAttackTableSelected( AttackHandleList& list, BOOL reverse )
{
    CSingleLock( &m_listHandleLock, TRUE );

    INT selected = -1;
    INT count = GetListCtrl().GetSelectedCount();
    for( INT i = 0; i <count; i++ )
    {
        selected = GetListCtrl().GetNextItem( selected, LVNI_SELECTED );
        size_t idx = (size_t)GetListCtrl().GetItemData(selected);

        list.push_back(AttackHandle(selected, m_listHandles[idx].m_handle, m_listHandles[idx].m_name));
    }

    if( reverse ) {
        std::reverse( list.begin(), list.end() );
    }

    return TRUE;
}

BOOL CProcEventView::ResetView()
{
    CSingleLock( &m_listHandleLock, TRUE );
    m_listHandles.clear();

    m_named = 0;
    m_unnamed = 0;
    m_total = 0;

    return GetListCtrl().DeleteAllItems();
}
//
//void CProcEventView::OnRightClick(NMHDR *pNMHDR, LRESULT *pResult)
//{
// const LRESULT PROCESSED = 1;
// const DWORD INVALID_SELECTION = (DWORD)-1;
//
// ASSERT( NULL != pNMHDR && NULL != pResult );
// if( NULL == pNMHDR || NULL == pResult ) { return; }
//
// // Default result for early out
// *pResult = !PROCESSED;
//
// // http://msdn.microsoft.com/en-us/library/bb774873(VS.85).aspx
// // http://msdn.microsoft.com/en-us/library/bb774771(VS.85).aspx
// LPNMITEMACTIVATE pItem = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
// // User can click anywhere (other than the menu) to generate a -1.
// if( INVALID_SELECTION == pItem->iItem ) { return; }
// // Sanity check
// ASSERT( pItem->iItem >= 0 && pItem->iItem < GetListCtrl().GetItemCount() );
// if( !(pItem->iItem >= 0 && pItem->iItem < GetListCtrl().GetItemCount()) ) { return; }
//
// // The item is good, so we consider this as 'handling the message'
// *pResult = PROCESSED;
//
// HandleContextMenu( pItem );
//}
//
//BOOL CProcEventView::HandleContextMenu( LPNMITEMACTIVATE pItem )
//{
// ASSERT( NULL != pItem );
// if( NULL == pItem ) { return FALSE; }
//
// BOOL bResult = FALSE;
//
// do
// {
// CMenu* pMenu = AfxGetMainWnd()->GetMenu();
//
// INT pos = FindMenuPosition( pMenu, L"Attack" );
// ASSERT( MenuNotFound != pos );
// if( MenuNotFound == pos ) { break; }
//
// CMenu* pPopup = pMenu->GetSubMenu( pos );
// ASSERT( NULL != pPopup );
// if( NULL == pPopup ) { break; }
//
// CPoint p( pItem->ptAction.x, pItem->ptAction.y );
// ClientToScreen( &p );
//
// const UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD;
// INT nID = pPopup->TrackPopupMenu(nFlags, p.x, p.y, this);
// if( 0 == nID ) { break; }
//
// CPevatDoc* pDoc = GetDocument();
// ASSERT( NULL != pDoc );
// if( NULL == pDoc ) { break; }
//
// switch( nID )
// {
// case ID_ATTACK_INVERTSELECTED:
// pDoc->UpdateAllViews( this, HINT_ATTACK_INVERTSELECTED, 0 );
// break;
// case ID_ATTACK_PULSESELECTED:
// pDoc->UpdateAllViews( this, HINT_ATTACK_PULSESELECTED, 0 );
// break;
// case ID_ATTACK_SETSELECTED:
// pDoc->UpdateAllViews( this, HINT_ATTACK_SETSELECTED, 0 );
// break;
// case ID_ATTACK_RESETSELECTED:
// pDoc->UpdateAllViews( this, HINT_ATTACK_RESETSELECTED, 0 );
// break;
// default:
// ASSERT(FALSE);
// break;
// }
//
// bResult = TRUE;
//
// } while(false);
//
// return bResult;
//}

INT CProcEventView::FindMenuPosition( CMenu* pMenu, LPCWSTR pszText )
{
    ASSERT( NULL != pMenu );
    if( NULL == pMenu ) { return MenuNotFound; }

    ASSERT( NULL != pszText );
    if( NULL == pszText ) { return MenuNotFound; }

    INT count = pMenu->GetMenuItemCount();
    ASSERT( count >= 0 );
    if( !(count >= 0) ) { return MenuNotFound; }

    INT pos = MenuNotFound;
    for(INT i = 0; i < count; i++ )
    {
        // Fix up the stupid ampersands
        CString text = pszText; text.Replace(L"&", L"");
        CString str;

        if( pMenu->GetMenuString(i, str, MF_BYPOSITION) ) {
            str.Replace( L"&", L"" );
            if ( 0 == text.CompareNoCase( str ) ) {
                pos = i; break;
            }
        }
    }

    ASSERT( pos != MenuNotFound );

    return pos;
}

void CProcEventView::OnLvnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    ASSERT( NULL != pNMHDR && NULL != pResult );
    if( NULL == pNMHDR || NULL == pResult ) { return; }

    const LRESULT PROCESSED = 1;
    *pResult = !PROCESSED;

    NMLVDISPINFO* pInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

    // Handle of interest
    size_t idx = (size_t)pInfo->item.lParam;
    ASSERT( idx >= 0 && idx < m_listHandles.size() );
    const EventHandle ehandle = m_listHandles[idx];

    // Request mask
    UINT mask = pInfo->item.mask;

    if( mask & LVIF_TEXT )
    {
        *pResult = PROCESSED;

        switch( pInfo->item.iSubItem )
        {
        case Number: // Number
            StringCchPrintf( pInfo->item.pszText, pInfo->item.cchTextMax, L"%d", idx );
            break;

        case Dacl: // DACL
            StringCchCopy( pInfo->item.pszText, pInfo->item.cchTextMax,
                ehandle.m_dacl.c_str() );
            break;

        case Access: // Access
            StringCchCopy( pInfo->item.pszText, pInfo->item.cchTextMax,
                EventAccessAsString(ehandle.m_access).c_str() );
            break;

        case State: // State
            StringCchCopy( pInfo->item.pszText, pInfo->item.cchTextMax,
                EventStateAsString(ehandle.m_handle.Ref()).c_str() );
            break;

        case Name: // Name
            StringCchCopy( pInfo->item.pszText, pInfo->item.cchTextMax,
                ehandle.m_name.c_str() );
            break;

        default:
            *pResult = !PROCESSED;
        }
    }
}

BOOL CProcEventView::RefreshEventHandles( )
{
    BOOL bResult = TRUE;

    GetListCtrl().SetRedraw(FALSE);
    {
        bResult &= ResetView();
        ASSERT( bResult );

        bResult &= AddEventHandles();
        // Fires too often for ACCESS_DENIED
        // ASSERT( bResult );
    }
    GetListCtrl().SetRedraw(TRUE);

    return bResult;
}

BOOL CProcEventView::AddEventHandles()
{
    CProcTreeView* pView = CProcTreeView::GetView();
    ASSERT( NULL != pView );
    if( !pView ) { return FALSE; }

    INT pid = pView->GetSelectedProcess();

    try
    {
        /*BOOL bResult = */DuplicateEventHandles(pid);
        // Fires too often for ACCESS_DENIED
        // ASSERT( bResult );
    }
    catch( ProgramException& e )
    {
        CLogView::WriteLine( e.what() );
        return FALSE;
    }

    if( PROCESS_ID_INVALID != pid && PROCESS_ID_SYSTEM != pid )
    {
        WCHAR wsz[96];
        StringCchPrintf( wsz, _countof(wsz), L"Opened process %d (%s).\r\n" \
            L" Total: %d, Named: %d, Unnamed: %d.",
            pid, pView->GetSelectedProcessName(), m_total, m_named, m_unnamed );
        CLogView::WriteLine( wsz );
    }

    // Can we bail early?
    if( 0 == m_listHandles.size() ) { return TRUE; }

    for( unsigned i = 0; i<(unsigned)m_listHandles.size(); i++ )
    {
        LVITEM item;

        item.iItem = i;
        item.iSubItem = 0;
        item.mask = LVIF_TEXT | LVIF_NORECOMPUTE | LVIF_PARAM;
        item.pszText = LPSTR_TEXTCALLBACK;
        item.lParam = (LPARAM)i;

        GetListCtrl().InsertItem( &item );
    }

    return TRUE;
}

BOOL CProcEventView::DuplicateEventHandles( INT pid )
{
    // General purpose results
    NTSTATUS status = STATUS_SUCCESS;

    // Function pointers of interest
    assert( NULL != (HMODULE)m_hfuncs );
    if( NULL == m_hfuncs ) { return FALSE; }

    if( PROCESS_ID_INVALID == pid || PROCESS_ID_SYSTEM == pid ) { return TRUE; }

    // Open the target process
    AutoHandle hProcess;
    hProcess.Attach( OpenProcess(PROCESS_DUP_HANDLE, FALSE, pid) );
    DWORD dwLastError = GetLastError();

    // Fires too often for a standard user
    // ASSERT( NULL != hProcess );
    if( NULL == hProcess )
    {
        wostringstream err;
        err << L"Failed to open process " << pid;
        err << L". OpenProcess returned ";
        err << std::hex << dwLastError << L".";
        throw ProgramException( err.str().c_str() );
    }

    // XP has about 25, Vista has about 50
    UINT procs = 50;
    CProcTreeView* pView = CProcTreeView::GetView();
    ASSERT( NULL != pView );
    if( pView ) { procs = pView->GetProcessCount(); }

    // We can save on reallocations by estimating the required size
    size_t size = 128 * 1024 * procs;
    HandleTableAutoPtr htable( size );

    while ((status = m_hfuncs.pfnNtQuerySystemInformation( SystemHandleInformation,
        (LPVOID)htable, (ULONG)htable.Size(), NULL )) == STATUS_INFO_LENGTH_MISMATCH)
    {
        // Default for Reallocate is to double current size
        htable.Reallocate();
        OutputDebugString( L"SystemHandleInformation reallocation\n" );
    }

    assert( NT_SUCCESS(status) );
    if( !NT_SUCCESS(status) )
    {
        wostringstream err;
        err << L"Failed to query handle table. NtQuerySystemInformation ";
        err << L"returned " << std::hex << status << L".";
        throw ProgramException( err.str().c_str() );
    }

    // Back to C interface
    PSYSTEM_HANDLE_INFORMATION pcbHandleInfo = (PSYSTEM_HANDLE_INFORMATION)(LPVOID)htable;
# ifndef WIN64
    if( 0 == pcbHandleInfo->HandleCount ) {
        wostringstream err;
        err << L"Handle count is 0." << endl;
        err << L" - Are you using an x86 build on an x64 system?" << endl;
        err << L" - Is an Antivirus program blocking access?" << endl;
        throw ProgramException( err.str().c_str() );
    }
# endif

    // Lock the document for modification
    CSingleLock( &m_listHandleLock, TRUE );
    m_listHandles.clear();

    for(ULONG i = 0; i < pcbHandleInfo->HandleCount; i++)
    {
        const SYSTEM_HANDLE& handle = pcbHandleInfo->Handles[i];
        if( (ULONG)pid != handle.ProcessId ) { continue; }

        DuplicateEventHandle(hProcess, handle);
    }

    HandleTableList::iterator it = m_listHandles.begin();
    for( ; it != m_listHandles.end(); it++ )
    {
        QueryEventHandleName( *it );
        QueryEventHandleDacl( *it );
        QueryEventHandleState( *it );

        m_total++;

        if( !it->m_name.empty() )
        {
            m_named++;
        }
        else
        {
            m_unnamed++;
        }
    }

    //ASSERT( m_total == (INT)m_listHandles.size() );
    return 0 != m_listHandles.size();
}

BOOL CProcEventView::DuplicateEventHandle(const HANDLE& hProcess, const SYSTEM_HANDLE& syshandle)
{
    // Function pointers of interest
    assert( NULL != (HMODULE)m_hfuncs );
    if( NULL == m_hfuncs ) { return FALSE; }

    // General purpose results
    NTSTATUS status = STATUS_SUCCESS;

    // Returned to caller
    BOOL bResult = FALSE;

    do
    {
        DWORD dwAccess = 0;
        AutoHandle dup;

        for( INT i=0; i < _countof(EVENT_ACCESS); i++ )
        {
            dwAccess = EVENT_ACCESS[i];
            status = m_hfuncs.pfnNtDuplicateObject( hProcess, (HANDLE)syshandle.Handle,
                GetCurrentProcess(), &dup.Ref(), dwAccess, 0, 0 );
            if(NT_SUCCESS(status) ) { break; }
        }
        if(!NT_SUCCESS(status) ) { dwAccess=0; }

        // Query the object type
        AutoBuffer info( 0x100 );

        while( (status = m_hfuncs.pfnNtQueryObject( dup, ObjectTypeInformation,
            (PVOID)info, (ULONG)info.Size(), NULL )) == STATUS_INFO_LENGTH_MISMATCH )
        {
            // Default for Reallocate is to double current size
            info.Reallocate();
            OutputDebugString( L"ObjectTypeInformation reallocation\n" );
        }

        // Fires too often for objects which we do not care about
        // ASSERT( NT_SUCCESS(status) );
        if( !NT_SUCCESS(status) ) { break; }

        POBJECT_TYPE_INFORMATION pObjectTypeInfo =
            (POBJECT_TYPE_INFORMATION)(LPVOID)info;

        wstring type( pObjectTypeInfo->Name.Buffer, pObjectTypeInfo->Name.Length );
        if( 0 != wcscmp( L"Event", type.c_str() ) ) { break; }

        // Assignment duplicates the handle
        EventHandle ehandle;
        ehandle.m_handle = dup;
        ehandle.m_access = dwAccess;

        m_listHandles.push_back( ehandle );

        bResult = TRUE;

    } while( false );

    return bResult;
}

BOOL CProcEventView::QueryEventHandleName( EventHandle& handle )
{
    // Function pointers of interest
    assert( NULL != (HMODULE)m_hfuncs );
    if( NULL == m_hfuncs ) { return FALSE; }

    // General purpose results
    NTSTATUS status = STATUS_SUCCESS;

    // Query the object type
    AutoBuffer info( 0x100 );

    // Query object name (reuse info buffer)
    while( (status = m_hfuncs.pfnNtQueryObject( handle.m_handle, ObjectNameInformation,
        (PVOID)info, (ULONG)info.Size(), NULL )) == STATUS_INFO_LENGTH_MISMATCH )
    {
        // Default for Reallocate is to double current size
        info.Reallocate();
        OutputDebugString( L"ObjectNameInformation reallocation\n" );
    }

    ASSERT( NT_SUCCESS(status) );
    if( NT_SUCCESS(status) )
    {
        const UNICODE_STRING& name = *((PUNICODE_STRING)(LPVOID)info);
        if( name.Length ) {
            handle.m_name = wstring( name.Buffer, name.Length );
        }
    }

    return NT_SUCCESS(status);
}

BOOL CProcEventView::QueryEventHandleState( EventHandle& handle )
{
    DWORD dwWait = WAIT_FAILED;
    DWORD dwError = ERROR_SUCCESS;

    dwWait = WaitForSingleObject( handle.m_handle, 0 );
    dwError = GetLastError();

    if( dwWait == WAIT_OBJECT_0 ) {
        handle.m_state = L"Signaled";
    } else if( dwWait == WAIT_TIMEOUT ) {
        handle.m_state = L"Nonsignaled";
    } else {
        handle.m_state = L"Unknown";
    }

    return dwError == ERROR_SUCCESS;
}

BOOL CProcEventView::QueryEventHandleDacl( EventHandle& handle )
{
    PSECURITY_DESCRIPTOR pSecDesc = NULL;
    PACL pDacl = NULL;

    DWORD dwReturn = ERROR_SUCCESS;

    __try
    {
        // Retrieve the SD and ACL for the object
        dwReturn = GetSecurityInfo( handle.m_handle, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION,
            NULL /*&pOwner*/, NULL /*&pGroup*/, &pDacl, NULL/*&pSacl*/, &pSecDesc );
        if( dwReturn != ERROR_SUCCESS ) { __leave; }

        if( NULL == pDacl ) { handle.m_dacl = L"NULL"; }
    }
    __finally
    {
        if( NULL != pSecDesc ) {
            LocalFree( pSecDesc ); pSecDesc = NULL;
        }
    }

    return dwReturn == ERROR_SUCCESS;
}


void CProcEventView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    if( 0 == GetListCtrl().GetItemCount() ) { return; }

    do
    {
        // [0 to n-1]
        CMenu* pMenu = dynamic_cast<CMenu*>(AfxGetMainWnd()->GetMenu());
        ASSERT( NULL != pMenu );
        if( NULL == pMenu ) { break; }

        INT pos = FindMenuPosition( pMenu, L"Attack" );
        ASSERT( MenuNotFound != pos );
        if( MenuNotFound == pos ) { break; }

        CMenu* pPopup = pMenu->GetSubMenu( pos );
        ASSERT( NULL != pPopup );
        if( NULL == pPopup ) { break; }

        const UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD;
        INT nID = pPopup->TrackPopupMenu(nFlags, point.x, point.y, AfxGetMainWnd());
        if( 0 == nID ) { break; }

        switch( nID )
        {
        case ID_ATTACK_INVERTSELECTED:
            AttackInvertSelected();
            break;
        case ID_ATTACK_INVERTALL:
            AttackInvertAll();
            break;
        case ID_ATTACK_PULSESELECTED:
            AttackPulseSelected();
            break;
        case ID_ATTACK_PULSEALL:
            AttackPulseAll();
            break;
        case ID_ATTACK_SETSELECTED:
            AttackSetSelected();
            break;
        case ID_ATTACK_SETALL:
            AttackSetAll();
            break;
        case ID_ATTACK_RESETSELECTED:
            AttackResetSelected();
            break;
        case ID_ATTACK_RESETALL:
            AttackResetAll();
            break;
        default:
            ASSERT(FALSE);
            break;
        }

    } while(false);
}

BOOL CProcEventView::AttackInvertAll()
{
    // Start high, pull low
    BOOL bResult = TRUE;

    bResult &= BuildAttackTable( m_listAttack, false );
    bResult &= (ERROR_SUCCESS == LauchAttackThread( CAttackParams::Invert ));

    ASSERT( bResult );
    return bResult;
}

BOOL CProcEventView::AttackInvertSelected()
{
    // Start high, pull low
    BOOL bResult = TRUE;

    bResult &= BuildAttackTable( m_listAttack, true );
    bResult &= (ERROR_SUCCESS == LauchAttackThread( CAttackParams::Invert ));

    ASSERT( bResult );
    return bResult;
}

BOOL CProcEventView::AttackPulseAll()
{
    // Start high, pull low
    BOOL bResult = TRUE;

    bResult &= BuildAttackTable( m_listAttack, false );
    bResult &= (ERROR_SUCCESS == LauchAttackThread( CAttackParams::Pulse ));

    ASSERT( bResult );
    return bResult;
}

BOOL CProcEventView::AttackPulseSelected()
{
    // Start high, pull low
    BOOL bResult = TRUE;

    bResult &= BuildAttackTable( m_listAttack, true );
    bResult &= (ERROR_SUCCESS == LauchAttackThread( CAttackParams::Pulse ));

    ASSERT( bResult );
    return bResult;
}

BOOL CProcEventView::AttackSetAll()
{
    // Start high, pull low
    BOOL bResult = TRUE;

    bResult &= BuildAttackTable( m_listAttack, false );
    bResult &= (ERROR_SUCCESS == LauchAttackThread( CAttackParams::Set ));

    ASSERT( bResult );
    return bResult;
}

BOOL CProcEventView::AttackSetSelected()
{
    // Start high, pull low
    BOOL bResult = TRUE;

    bResult &= BuildAttackTable( m_listAttack, true );
    bResult &= (ERROR_SUCCESS == LauchAttackThread( CAttackParams::Set ));

    ASSERT( bResult );
    return bResult;
}

BOOL CProcEventView::AttackResetAll()
{
    // Start high, pull low
    BOOL bResult = TRUE;

    bResult &= BuildAttackTable( m_listAttack, false );
    bResult &= (ERROR_SUCCESS == LauchAttackThread( CAttackParams::Reset ));

    ASSERT( bResult );
    return bResult;
}

BOOL CProcEventView::AttackResetSelected()
{
    // Start high, pull low
    BOOL bResult = TRUE;

    bResult &= BuildAttackTable( m_listAttack, true );
    bResult &= (ERROR_SUCCESS == LauchAttackThread( CAttackParams::Reset ));

    ASSERT( bResult );
    return bResult;
}

DWORD CProcEventView::LauchAttackThread( DWORD dwOptions )
{
    CProcTreeView* pTreeView = CProcTreeView::GetView();
    ASSERT( pTreeView != NULL );
    if( pTreeView == NULL ) { return ERROR_INVALID_PARAMETER; }

    CAttackView* pAttackView = CAttackView::GetView();
    ASSERT( NULL != pAttackView );
    if( NULL == pAttackView ) { return ERROR_INVALID_PARAMETER; }

    CAttackParams* lpParams = new CAttackParams( m_listAttack, m_listHandleLock );
    ASSERT( NULL != lpParams );
    if( NULL == lpParams ) { return GetLastError(); }

    lpParams->nPid = pTreeView->GetSelectedProcess();
    lpParams->bExcludeKnown = pAttackView->GetExcludeKnown();
    lpParams->dwDelay = pAttackView->GetDelay();
    lpParams->dwOptions = dwOptions;
    lpParams->bAutoStop = pAttackView->GetAutoStop();

    CWinThread* pThread = AfxBeginThread( AttackThreadProc, (LPVOID)lpParams,
        THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL );
    DWORD dwLastError = GetLastError();

    ASSERT( NULL != pThread );
    if( NULL == pThread ) { return dwLastError; }

    // Attack thread is (nearly) running
    SetEvent( g_hEvntProcessing );
    // Attack thread should continue processing (until this event is pushed high)
    ResetEvent( g_hEvntCancelProcessing );

    pThread->m_bAutoDelete = TRUE;
    pThread->ResumeThread();

    return ERROR_SUCCESS;
}

UINT CDECL AttackThreadProc( LPVOID lpParams )
{
    ASSERT( NULL != lpParams );
    if( NULL == lpParams ) { return ERROR_INVALID_PARAMETER; }

    // Scratch
    WCHAR wsz[255];
    BOOL bResult = FALSE, bAttackResult = FALSE;
    DWORD dwLastError = ERROR_SUCCESS, dwAttackError = ERROR_SUCCESS;

    // Grab parameters, delete the memory, and NULL the pointer
    CAttackParams* pAttackParams = (CAttackParams*)lpParams;
    AttackHandleList& list = pAttackParams->listAttack;
    CCriticalSection& lock = pAttackParams->listLock;
    DWORD dwPid = pAttackParams->nPid;
    BOOL bExcludeKnown = pAttackParams->bExcludeKnown;
    DWORD dwDelay = pAttackParams->dwDelay;
    DWORD dwOptions = pAttackParams->dwOptions;
    BOOL bAutoStop = pAttackParams->bAutoStop;
    delete pAttackParams; lpParams = pAttackParams = NULL;

    HWND hWnd = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
    ASSERT( NULL != hWnd );

    // Signal that we have begun processing
    bResult = SetEvent( g_hEvntProcessing );
    ASSERT( bResult );

    // Notify others
    if( hWnd ) {
        ::SendMessage( hWnd, MSG_ATTACK_START, NULL, NULL );
    }

    AutoHandle hProcess;
    hProcess.Ref() = OpenProcess(SYNCHRONIZE, FALSE, dwPid);
    dwLastError = GetLastError();

    if( NULL == hProcess && bAutoStop )
    {
        if( hWnd ) {
            StringCchPrintf(wsz, _countof(wsz), L"OpenProcess with SYNCHRONIZE failed for process %d. LastError=%d.", dwPid, dwLastError);
            ::SendMessage( hWnd, MSG_LOG_MESSAGE, WPARAM(new CString(wsz)), NULL );
        }
    }
    else if( hProcess && bAutoStop )
    {
        if( hWnd ) {
            StringCchPrintf(wsz, _countof(wsz), L"Opened process %d with SYNCHRONIZE.", dwPid);
            ::SendMessage( hWnd, MSG_LOG_MESSAGE, WPARAM(new CString(wsz)), NULL );
        }
    }

    // Lock the handle and attack table
    CSingleLock( &lock, TRUE );

    AttackHandleList::iterator it = list.begin();
    for( unsigned i = 0; it != list.end(); it++, i++ )
    {
        if( bExcludeKnown && IsWellKnownEvent(it->m_name.c_str()))
        {
            if( hWnd ) {
                StringCchPrintf( wsz, _countof(wsz), L"Skipped event %d (well known)", it->m_pos );
                ::SendMessage( hWnd, MSG_LOG_MESSAGE, WPARAM(new CString(wsz)), NULL );
                StringCchPrintf( wsz, _countof(wsz), L" %s", it->m_name.c_str() );
                ::SendMessage( hWnd, MSG_LOG_MESSAGE, WPARAM(new CString(wsz)), NULL );
            }
            continue;
        }

        wstring pre, post;
        WCHAR* pAction = NULL;

        // Test object before attack
        pre = EventStateAsString( it->m_handle );

        switch( dwOptions & CAttackParams::ActionMask )
        {
        case CAttackParams::Invert:
            {
                pAction = L"Invert event";
                if( pre == L"Signaled" ) {
                    bAttackResult = ResetEvent( it->m_handle );
                }
                else if( pre == L"Nonsignaled" ) {
                    bAttackResult = SetEvent( it->m_handle );
                } else {
                    // What do we do?
                    bResult = FALSE;
                }
                dwAttackError = GetLastError();
                break;
            }
        case CAttackParams::Pulse:
            {
                pAction = L"Pulse event";
                bAttackResult = PulseEvent( it->m_handle );
                dwAttackError = GetLastError();
                break;
            }
        case CAttackParams::Set:
            {
                pAction = L"Set event";
                bAttackResult = SetEvent( it->m_handle );
                dwAttackError = GetLastError();
                break;
            }
        case CAttackParams::Reset:
            {
                pAction = L"Reset event";
                bAttackResult = ResetEvent( it->m_handle );
                dwAttackError = GetLastError();
                break;
            }
        default:
            ASSERT( FALSE );
            break;
        }

        // Give the event time to stabilize
        Sleep(50);

        // Test object after attack
        post = EventStateAsString(it->m_handle);

        StringCchPrintf(wsz, _countof(wsz), L"%s %d", pAction, it->m_pos);

        if( bAttackResult ) {
            StringCchCat( wsz, _countof(wsz), L" succeeded." );
        }
        else
        {
            WCHAR err[255];
            StringCchPrintf( err, _countof(err), L" failed. LastError=%d.", dwAttackError );
            StringCchCat( wsz, _countof(wsz), err );
        }

        // Log result of Invert/Puse/Set/Reset
        if( hWnd ) {
            ::SendMessage( hWnd, MSG_LOG_MESSAGE, WPARAM(new CString(wsz)), NULL );
            // Update the list view of handles
            ::SendMessage( hWnd, MSG_UPDATE_HANDLE_TABLE_ITEM, (WPARAM)it->m_pos, NULL );
        }

        // Pre/Post state logging
        {
            post = EventStateAsString( it->m_handle );
            if( hWnd ) {
                StringCchPrintf( wsz, _countof(wsz), L" Pre=%s, Post=%s.", pre.c_str(), post.c_str() );
                ::SendMessage( hWnd, MSG_LOG_MESSAGE, WPARAM(new CString(wsz)), NULL );
            }
        }

        DWORD dwWait = WAIT_FAILED;
        if( NULL != hProcess )
        {
            dwWait = WaitForSingleObject( hProcess, dwDelay );
            if( dwWait == WAIT_OBJECT_0 )
            {
                // One time message
                static bool logged = false;
                if( !logged ) {
                    if( hWnd ) {
                        StringCchPrintf(wsz, _countof(wsz), L"** Process %d has terminated **", dwPid);
                        ::SendMessage( hWnd, MSG_LOG_MESSAGE, WPARAM(new CString(wsz)), NULL );
                    }
                    logged = true;
                }

                if( bAutoStop ) {

                    // break the for( ) loop
                    break;
                }
            }
            else if ( dwWait == WAIT_FAILED )
            {
                // One time message
                static bool logged = false;
                if( !logged ) {
                    if( hWnd ) {
                        StringCchPrintf(wsz, _countof(wsz), L"** Process %d - Wait failed **", dwPid);
                        ::SendMessage( hWnd, MSG_LOG_MESSAGE, WPARAM(new CString(wsz)), NULL );
                    }
                    logged = true;
                }
            }
        }

        // Have we been instructed to stop?
        dwWait = WaitForSingleObject( g_hEvntCancelProcessing, dwDelay );
        if( dwWait != WAIT_TIMEOUT ) { break; }
    }

    // Signal that we are finished with processing
    ResetEvent( g_hEvntProcessing );

    if( hWnd ) {
        // Notify others
        ::SendMessage( hWnd, MSG_ATTACK_STOP, NULL, NULL );
    }

    // We completed successfully. Hopefully, the target crashed.
    return ERROR_SUCCESS;

}

BOOL IsWellKnownEvent( LPCWSTR pwsz )
{
    if( NULL == pwsz || pwsz[0] == L'\0' ) { return FALSE; }

    static vector<wstring> events;
    static bool init = false;

    if( !init )
    {
        events.push_back( L"TermSrvReadyEvent" );
        events.push_back( L"LowMemoryCondition" );
        events.push_back( L"DINPUTWINMM" );
        events.push_back( L"userenv: User Profile setup event" );
        events.push_back( L"userenv: Machine Group Policy has been applied" );
        events.push_back( L"userenv: User Group Policy has been applied" );
        events.push_back( L"crypt32LogoffEvent" );

        stable_sort( events.begin(), events.end() );
        init = true;
    }

    bool found = false;
    vector<wstring>::const_iterator it = events.begin();
    for( ; it != events.end(); it++ )
    {
        const wstring& str(pwsz);
        if( wstring::npos != str.find( *it ) )
        {
            found = true; break; 
        }
    }

    return found;
}