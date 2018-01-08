// MainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"

#include "PevatApp.h"
#include "EulaDlg.h"
#include "PevatDoc.h"
#include "StrSafe.h"
#include "LogView.h"
#include "InfoView.h"
#include "MainFrame.h"
#include "AttackView.h"
#include "VersionInfo.h"
#include "ProcTreeView.h"
#include "ProcEventView.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()

    ON_MESSAGE(MSG_LOG_MESSAGE, &CMainFrame::OnMsgLogMessage)
    ON_MESSAGE(MSG_ATTACK_START, &CMainFrame::OnMsgAttackStart)
    ON_MESSAGE(MSG_ATTACK_STOP, &CMainFrame::OnMsgAttackStop)
    ON_MESSAGE(MSG_UPDATE_AVAILABLE, &CMainFrame::OnMsgAppUpdateAvailable)
    ON_MESSAGE(MSG_UPDATE_HANDLE_TABLE, &CMainFrame::OnMsgUpdateHandleTable)
    ON_MESSAGE(MSG_UPDATE_HANDLE_TABLE_ITEM, &CMainFrame::OnMsgUpdateHandleTableItem)
    ON_MESSAGE(MSG_PROCESS_LIST_CHANGED, &CMainFrame::OnMsgProcessListChanged)

    ON_COMMAND(ID_EDIT_COPY, &CMainFrame::OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CMainFrame::OnUpdateEditCopy)

    ON_COMMAND(ID_VIEW_REFRESH, &CMainFrame::OnViewRefresh)
    ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESH, &CMainFrame::OnUpdateViewRefresh)

    ON_COMMAND(ID_PRIVILEGES_DEBUG, &CMainFrame::OnPrivilegesDebug)
    ON_COMMAND(ID_PRIVILEGES_SECURITY, &CMainFrame::OnPrivilegesSecurity)
    ON_COMMAND(ID_PRIVILEGES_BACKUP, &CMainFrame::OnPrivilegesBackup)
    ON_COMMAND(ID_PRIVILEGES_ASSIGNPRITOKEN, &CMainFrame::OnUpdatePrivilegesAssignToken)
    ON_COMMAND(ID_PRIVILEGES_TCB, &CMainFrame::OnPrivilegesTcb)

    ON_UPDATE_COMMAND_UI(ID_PRIVILEGES_DEBUG, &CMainFrame::OnUpdatePrivilegesDebug)
    ON_UPDATE_COMMAND_UI(ID_PRIVILEGES_BACKUP, &CMainFrame::OnUpdatePrivilegesBackup)
    ON_UPDATE_COMMAND_UI(ID_PRIVILEGES_ASSIGNPRITOKEN, &CMainFrame::OnUpdatePrivilegesAssignToken)
    ON_UPDATE_COMMAND_UI(ID_PRIVILEGES_TCB, &CMainFrame::OnUpdatePrivilegesActAsOS)
    ON_UPDATE_COMMAND_UI(ID_PRIVILEGES_SECURITY, &CMainFrame::OnUpdatePrivilegesSecurity)

    ON_UPDATE_COMMAND_UI(ID_INDICATOR_EVENT_TOTAL, &CMainFrame::OnUpdateTotalEvents)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_EVENT_NAMED, &CMainFrame::OnUpdateNamedEvents)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_EVENT_UNNAMED, &CMainFrame::OnUpdateUnnamedEvents)

    ON_COMMAND(ID_ATTACK_INVERTSELECTED, &CMainFrame::OnAttackInvert)
    ON_COMMAND(ID_ATTACK_PULSESELECTED, &CMainFrame::OnAttackPulse)
    ON_COMMAND(ID_ATTACK_SETSELECTED, &CMainFrame::OnAttackSet)
    ON_COMMAND(ID_ATTACK_RESETSELECTED, &CMainFrame::OnAttackReset)

    ON_UPDATE_COMMAND_UI(ID_ATTACK_INVERTSELECTED, &CMainFrame::OnUpdateAttackInvert)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_PULSESELECTED, &CMainFrame::OnUpdateAttackPulse)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_SETSELECTED, &CMainFrame::OnUpdateAttackSet)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_RESETSELECTED, &CMainFrame::OnUpdateAttackReset)

    ON_COMMAND(ID_ATTACK_INVERTALL, &CMainFrame::OnAttackInvertAll)
    ON_COMMAND(ID_ATTACK_PULSEALL, &CMainFrame::OnAttackPulseAll)
    ON_COMMAND(ID_ATTACK_SETALL, &CMainFrame::OnAttackSetAll)
    ON_COMMAND(ID_ATTACK_RESETALL, &CMainFrame::OnAttackResetAll)

    ON_UPDATE_COMMAND_UI(ID_ATTACK_INVERTALL, &CMainFrame::OnUpdateAttackInvertAll)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_PULSEALL, &CMainFrame::OnUpdateAttackPulseAll)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_SETALL, &CMainFrame::OnUpdateAttackSetAll)
    ON_UPDATE_COMMAND_UI(ID_ATTACK_RESETALL, &CMainFrame::OnUpdateAttackResetAll)

END_MESSAGE_MAP()

static UINT indicators[] =
{
    ID_SEPARATOR, // status line indicator
    ID_INDICATOR_EVENT_TOTAL,
    ID_INDICATOR_EVENT_NAMED,
    ID_INDICATOR_EVENT_UNNAMED
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame(){ }

CMainFrame::~CMainFrame(){ }

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
        sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1; // fail to create
    }

    return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
    BOOL bReturn = FALSE;

    // What we are trying to accomplish:
    //
    // |.....|...........|.....|
    // |.....|.....B.....|.....|
    // |.....|...........|.....|
    // |.....|===========|.....|
    // |.....|...........|.....|
    // |..A..|.....C.....|..E..|
    // |.....|...........|.....|
    // |.....|===========|.....|
    // |.....|...........|.....|
    // |.....|.....D.....|.....|
    // |.....|...........|.....|
    //
    // A: ProcTreeView
    // B: InfoView
    // C: ProcEventView
    // D: LogView
    // E: AttackView

    CRect r;
    GetClientRect( &r );

    INT a_w = r.Width()/5;
    INT a_h = r.Height();

    INT b_w = r.Width()*3/5;
    INT b_h = r.Height()/6;

    INT c_w = r.Width()*3/5;
    INT c_h = r.Height()*1/2;

    INT d_w = r.Width()*3/5;
    INT d_h = r.Height()*2/5;

    INT e_w = r.Width()/5 - 20;
    INT e_h = r.Height();

    do
    {
        // Create the vertical splitter window
        if (!m_VertSplitter.CreateStatic(this, 1, 3,
            WS_VISIBLE | WS_CHILD, AFX_IDW_PANE_FIRST)) {
                break;
        }

        if (!m_VertSplitter.CreateView(0, 0,
            RUNTIME_CLASS(CProcTreeView), CSize(a_w, a_h), pContext)) {
                break;
        }

        if (!m_VertSplitter.CreateView(0, 2,
            RUNTIME_CLASS(CAttackView), CSize(e_w, e_h), pContext)) {
                break;
        }

        // Add the second splitter pane - which is a nested splitter with 3 rows
        if(!m_HorzSplitter.CreateStatic(&m_VertSplitter, 3, 1,
            WS_CHILD | WS_VISIBLE, m_VertSplitter.IdFromRowCol(0, 1))) {
                break;
        }

        if (!m_HorzSplitter.CreateView(0, 0,
            RUNTIME_CLASS(CInfoView), CSize(b_w, b_h), pContext)) {
                break;
        }

        if (!m_HorzSplitter.CreateView(1 , 0,
            RUNTIME_CLASS(CProcEventView), CSize(c_w, c_h), pContext)) {
                break;
        }

        if (!m_HorzSplitter.CreateView(2, 0,
            RUNTIME_CLASS(CLogView), CSize(d_w, d_h), pContext)) {
                break;
        }

        // We need to set the preferred size; otherwise
        // views A and E will dominate
        m_VertSplitter.SetColumnInfo( 0, a_w, 0 );
        m_VertSplitter.SetColumnInfo( 1, c_w, 0 ); // b, c, or d will do
        m_VertSplitter.SetColumnInfo( 2, max(185,e_w), 0 );

        bReturn = TRUE;

    } while(false);

    if( !bReturn )
    {
        if( IsWindow( m_HorzSplitter.GetSafeHwnd() ) ) {
            m_HorzSplitter.DestroyWindow();
        }
        if( IsWindow( m_VertSplitter.GetSafeHwnd() ) ) {
            m_VertSplitter.DestroyWindow();
        }
    }

    return bReturn;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style &= ~FWS_ADDTOTITLE;

    if( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;

    return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

CPevatDoc* CMainFrame::GetDocument()
{
    CDocument* pDoc = GetActiveDocument();
    ASSERT(pDoc);
    if( !pDoc ) { return NULL; }

    ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CPevatDoc)));
    return (CPevatDoc*)pDoc;
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG

CProcTreeView* CMainFrame::GetProcTreeView()
{
    CWnd* pWnd = m_VertSplitter.GetPane(0, 0);
    CProcTreeView* pView = DYNAMIC_DOWNCAST(CProcTreeView, pWnd);

    ASSERT( NULL != pView );
    return pView;
}

CProcEventView* CMainFrame::GetProcEventView()
{
    CWnd* pWnd = m_HorzSplitter.GetPane(1, 0);
    CProcEventView* pView = DYNAMIC_DOWNCAST(CProcEventView, pWnd);

    ASSERT( NULL != pView );
    return pView;
}

CLogView* CMainFrame::GetLogView()
{
    CWnd* pWnd = m_HorzSplitter.GetPane(2, 0);
    CLogView* pView = DYNAMIC_DOWNCAST(CLogView, pWnd);

    ASSERT( NULL != pView );
    return pView;
}

CAttackView* CMainFrame::GetAttackView()
{
    CWnd* pWnd = m_VertSplitter.GetPane(0, 2);
    CAttackView* pView = DYNAMIC_DOWNCAST(CAttackView, pWnd);

    ASSERT( NULL != pView );
    return pView;
}

LRESULT CMainFrame::OnMsgLogMessage( WPARAM wparam, LPARAM /*lparam*/ )
{
    const LRESULT PROCESSED = 1;

    CLogView* pView = CLogView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return !PROCESSED; }

    pView->OnUpdate( NULL, HINT_LOG_MESSAGE, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgAttackStart( WPARAM /*wparam*/, LPARAM /*lparam*/ )
{
    const LRESULT PROCESSED = 1;

    CPevatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return !PROCESSED; }

    pDoc->UpdateAllViews( NULL, HINT_ATTACK_START, NULL );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgAttackStop( WPARAM /*wparam*/, LPARAM /*lparam*/ )
{
    const LRESULT PROCESSED = 1;

    CPevatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return !PROCESSED; }

    pDoc->UpdateAllViews( NULL, HINT_ATTACK_STOP, NULL );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgUpdateHandleTable(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CPevatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return !PROCESSED; }

    pDoc->UpdateAllViews( NULL, HINT_UPDATE_HANDLE_TABLE, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgUpdateHandleTableItem(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CPevatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return !PROCESSED; }

    pDoc->UpdateAllViews( NULL, HINT_UPDATE_HANDLE_TABLE_ITEM, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgProcessListChanged(WPARAM wparam, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    CPevatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return !PROCESSED; }

    pDoc->UpdateAllViews( NULL, HINT_PROCESS_LIST_CHANGED, (CObject*)wparam );

    return PROCESSED;
}

LRESULT CMainFrame::OnMsgAppUpdateAvailable(WPARAM /*wparam*/, LPARAM /*lparam*/)
{
    const LRESULT PROCESSED = 1;

    AfxMessageBox(L"An update is available. Please visit www.softwareintegrity.com.");

    return PROCESSED;
}

void CMainFrame::OnEditCopy()
{
    CProcEventView* pView = GetProcEventView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return; }

    // Clipboard string
    CString str;

    INT selected = -1;
    INT count = pView->GetListCtrl().GetSelectedCount();
    for( INT i = 0; i <count; i++ )
    {
        selected = pView->GetListCtrl().GetNextItem( selected, LVNI_SELECTED );

        CString line = pView->GetListCtrl().GetItemText( selected, 4 );
        if( !line.IsEmpty() ) {
            str += line;
            if( i < count-1 ) { str += L"\n"; }
        }
    }

    if( str.IsEmpty() ) { return; }

    if (!OpenClipboard())
    {
        AfxMessageBox(L"Cannot open the Clipboard.", MB_ICONERROR);
        return;
    }

    do
    {
        // Remove the current Clipboard contents
        if(!EmptyClipboard())
        {
            AfxMessageBox(L"Cannot empty the Clipboard.", MB_ICONERROR);
            break;
        }

        // Get the currently selected data, hData handle to
        // global memory of data
        size_t cbStr = (str.GetLength() + 1) * sizeof(WCHAR);
        HGLOBAL hData = GlobalAlloc(GMEM_MOVEABLE, cbStr);
        ASSERT( NULL != hData );
        if (NULL == hData)
        {
            AfxMessageBox(L"Cannot allocate Clipboard memory.", MB_ICONERROR);
            break;
        }

        memcpy_s(GlobalLock(hData), cbStr, str.LockBuffer(), cbStr);
        GlobalUnlock(hData);
        str.UnlockBuffer();

        // For the appropriate data formats...
        if (::SetClipboardData(CF_UNICODETEXT, hData) == NULL)
        {
            AfxMessageBox(L"Unable copy data to Clipboard.", MB_ICONERROR);
            break;
        }

    } while(false);

    CloseClipboard();
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
    CString product;

    do
    {
        BOOL bResult = FALSE;
        WCHAR module[MAX_PATH];

        bResult = GetModuleFileName( NULL, module, _countof(module) );
        ASSERT( bResult );
        if( !bResult ) { break; }

        CVersionInfo info;
        bResult = info.Load( module );
        ASSERT( bResult );
        if( !bResult ) { break; }

        product = info.GetProductName();

#if defined WIN64
        product += L" (x64)";
#elif defined WIN32
        product += L" (x86)";
#else
        product += L" (unknown)";
#endif

    } while(false);

    SetWindowText(product);

    CFrameWnd::OnUpdateFrameTitle(bAddToTitle);
}
