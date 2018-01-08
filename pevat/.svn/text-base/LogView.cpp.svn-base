// LogView.cpp : implementation file
//

#include "stdafx.h"
#include "PevatApp.h"
#include "PevatDoc.h"
#include "LogView.h"
#include "MainFrame.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

IMPLEMENT_DYNCREATE(CLogView, CEditView)

BEGIN_MESSAGE_MAP(CLogView, CEditView)
    ON_WM_SIZE()
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_CTLCOLOR()
    ON_UPDATE_COMMAND_UI(ID_EDIT_CLEARALL, &CLogView::OnUpdateLogClearAll)
    ON_COMMAND(ID_EDIT_CLEARALL, &CLogView::OnLogClearAll)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// Serialized logging
CCriticalSection g_logLock;

CLogView::CLogView(): m_fontHeight(0)
{
    m_whiteBrush.CreateSolidBrush( RGB(255,255,255) );

    LOGFONT lf;
    if( !SystemParametersInfo( SPI_GETICONTITLELOGFONT, sizeof(lf), &lf, 0 ) )
    {
        ZeroMemory( &lf, sizeof(lf) );
        lf.lfWidth = 9;
        StringCchCopy( lf.lfFaceName, _countof(lf.lfFaceName), L"Shell UI" );
    }

    m_font.CreateFontIndirect( &lf );

    // TODO...
    m_font.GetLogFont( &lf );
    m_fontHeight = abs( lf.lfHeight );
    if( 0 == m_fontHeight ) { m_fontHeight = 12; }
}

CLogView::~CLogView(){ }

// CLogView message handlers
void CLogView::DoDataExchange(CDataExchange* pDX)
{
    CEditView::DoDataExchange(pDX);
}

int CLogView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CEditView::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

#ifdef _DEBUG
void CLogView::AssertValid() const
{
    CEditView::AssertValid();
}

void CLogView::Dump(CDumpContext& dc) const
{
    CEditView::Dump(dc);
}

CPevatDoc* CLogView::GetDocument()
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPevatDoc)));
    return (CPevatDoc*)m_pDocument;
}
#endif

void CLogView::OnInitialUpdate()
{
    CEditView::OnInitialUpdate();

    GetEditCtrl().SetFont( &m_font );
}

BOOL CLogView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= ES_READONLY;
    cs.style |= ES_MULTILINE;
    cs.style |= ES_AUTOVSCROLL;
    cs.style &= ~ES_AUTOHSCROLL;

    return CEditView::PreCreateWindow(cs);
}

HBRUSH CLogView::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    pDC->SetBkColor(RGB(255,255,255));
    return (HBRUSH)m_whiteBrush.GetSafeHandle();
}

CLogView* CLogView::GetView()
{
    CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
    ASSERT( pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
    if( !pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) ) { return NULL; }

    return pFrame->GetLogView();
}

void CLogView::WriteLine( LPCWSTR pwsz )
{
    // static bool vbar = false;

    ASSERT( NULL != pwsz );
    if( NULL == pwsz ) { return; }
    if( L'\0' == pwsz[0] ) { return; }

    CLogView* pView = CLogView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return; }

    CSingleLock(&g_logLock, TRUE);

    CEdit& edit = pView->GetEditCtrl();
    edit.LockWindowUpdate();
    {
        edit.SetSel( edit.GetWindowTextLength(), -1 );
        edit.ReplaceSel( pwsz );

        INT len = lstrlen( pwsz );
        if( pwsz[len-1] != L'\n' ) {
            edit.SetSel( edit.GetWindowTextLength(), -1 );
            edit.ReplaceSel( L"\r\n" );
        }
    }
    edit.UnlockWindowUpdate();
}

void CLogView::HandleScrollBar(int /*cx*/, int cy)
{
    CEdit& edit = GetEditCtrl();

    if( 0 == m_fontHeight )
    {
        edit.ShowScrollBar(SB_VERT, TRUE);
    }
    else
    {
        if((edit.GetLineCount()+2)*m_fontHeight >= cy)
        {
            edit.ShowScrollBar(SB_VERT, TRUE);
        }
        else
        {
            edit.ShowScrollBar(SB_VERT, FALSE);
        }
    }
}

void CLogView::OnUpdateLogClearAll(CCmdUI *pCmdUI)
{
    if( !pCmdUI ) { return; }

    if( GetEditCtrl().GetWindowTextLength() ) {
        pCmdUI->Enable(FALSE);
    }
    else {
        pCmdUI->Enable(TRUE);
    }
}

void CLogView::OnLogClearAll()
{
    GetEditCtrl().Clear();
}

void CLogView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    switch( lHint )
    {
    case HINT_LOG_MESSAGE:
        {
            ASSERT( NULL != pHint );
            if( NULL != pHint ) {
                CLogView::WriteLine( *((CString*)pHint) );
                delete ((CString*)pHint);
            }
            break;
        }
    case HINT_ATTACK_START:
        {
            CLogView::WriteLine( L"== Attack Start ==" );
            break;
        }
    case HINT_ATTACK_STOP:
        {
            CLogView::WriteLine( L"== Attack Stop ==" );
            break;
        }
    default:
        {
            CEditView::OnUpdate(pSender, lHint, pHint);
            break;
        }
    }

    CRect rect;
    GetClientRect( &rect );
    HandleScrollBar( rect.Width(), rect.Height() );
}

void CLogView::OnSize(UINT nType, int cx, int cy)
{
    CEditView::OnSize(nType, cx, cy);

    HandleScrollBar(cx, cy);
}

void CLogView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    // Anything to copy or clear?
    if( 0 == GetEditCtrl().GetWindowTextLength() ) { return; }

    do
    {
        CMenu menu;
        menu.LoadMenu(IDR_MENU_EDIT);

        CMenu* pPopup = menu.GetSubMenu(0);
        ASSERT( 0 != pPopup );
        if( NULL == pPopup ) { break; }

        const UINT nFlags = TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD;
        INT nID = pPopup->TrackPopupMenu( nFlags, point.x, point.y, this);
        if( 0 == nID ) { break; }

        switch( nID )
        {
        case ID_EDIT_CLEARALL:
            {
                GetEditCtrl().SetWindowText(L"");
                break;
            }
        case ID_EDIT_COPYALL:
            {
                GetEditCtrl().SetSel( 0, -1 );
                GetEditCtrl().Copy();
                GetEditCtrl().SetSel( 0, 0 );
                break;
            }
        default:
            break;
        }

    } while(false);
}
