// InfoView.cpp : implementation file
//

#include "stdafx.h"

#include "VersionInfo.h"
#include "InfoView.h"
#include "ProcTreeView.h"
#include "UserPrivilege.h"
#include "UserCredentials.h"
#include "ProcEventView.h"
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

IMPLEMENT_DYNCREATE(CInfoView, CEditView)

BEGIN_MESSAGE_MAP(CInfoView, CEditView)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_CTLCOLOR()
    ON_WM_SIZE()
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

CInfoView::~CInfoView()
{
    ASSERT( NULL != m_pfuncs );
}

CInfoView::CInfoView(): m_fontHeight(0)
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

// CInfoView message handlers
void CInfoView::DoDataExchange(CDataExchange* pDX)
{
    CEditView::DoDataExchange(pDX);
}

#ifdef _DEBUG
void CInfoView::AssertValid() const
{
    CEditView::AssertValid();
}

void CInfoView::Dump(CDumpContext& dc) const
{
    CEditView::Dump(dc);
}

CPevatDoc* CInfoView::GetDocument()
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPevatDoc)));
    return (CPevatDoc*)m_pDocument;
}
#endif

int CInfoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CEditView::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

void CInfoView::OnInitialUpdate()
{
    CEditView::OnInitialUpdate();

    GetEditCtrl().SetFont( &m_font );
}

BOOL CInfoView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= ES_READONLY;
    cs.style |= ES_MULTILINE;
    cs.style &= ~ES_AUTOHSCROLL;

    return CEditView::PreCreateWindow(cs);
}

HBRUSH CInfoView::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
    pDC->SetBkColor(RGB(255,255,255));
    return (HBRUSH)m_whiteBrush.GetSafeHandle();
}


void CInfoView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    switch( lHint )
    {
    case HINT_PRIVILEGE_CHANGED:
        {
            ResetView();
            UpdateVersionInformation();
            break;
        }
    case HINT_PROCESS_CHANGED:
        {
            ResetView();
            UpdateVersionInformation();
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

BOOL CInfoView::ResetView()
{
    GetEditCtrl().SetWindowText( L"" );

    return TRUE;
}

BOOL CInfoView::UpdateVersionInformation()
{
    CProcTreeView* pView = CProcTreeView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return FALSE; }

    if( pView->GetSelectedProcess() == PROCESS_ID_INVALID )
    {
        return DisplayComputerAndUserInformation();
    }

    return DisplayModuleInformation();
}

BOOL CInfoView::DisplayComputerAndUserInformation()
{
    // Returned to caller
    BOOL bResult = FALSE;
    // Scratch
    CString t, str;

    WCHAR computer[MAX_COMPUTERNAME_LENGTH+1] = { 0 };
    DWORD dwLength = _countof(computer);

    bResult = GetComputerName( computer, &dwLength );
    ASSERT( bResult );
    if( bResult ) {
        str += L"Computer: ";
        t += computer; t += L"\r\n";
        str += t;
    }

    CUserCredentials user;
    t = user.GetDomainAndUsername();
    if( !t.IsEmpty() ) {
        str += L"User: ";
        t += L"\r\n";
        str += t;
    }

    if( IsUserAnAdmin() ) {
        str += L"Administrator: true\r\n";
    }
    else {
        str += L"Administrator: false\r\n";
    }

    str += L"\r\n";

    CUserPrivilege::Status status = CUserPrivilege::Error;
    if( CUserPrivilege::Error != ( status = CUserPrivilege::HasTcbPrivilege() ) )
    {
        if( CUserPrivilege::Enabled == status )
        {
            str += L"Act as Operating System: true\r\n";
        }
        else
        {
            str += L"Act as Operating System: false\r\n";
        }
    }

    if( CUserPrivilege::Error != ( status = CUserPrivilege::HasAssignPrimaryTokenPrivilege() ) )
    {
        if( CUserPrivilege::Enabled == status )
        {
            str += L"Assign primary token: true\r\n";
        }
        else
        {
            str += L"Assign primary token: false\r\n";
        }
    }

    if( CUserPrivilege::Error != ( status = CUserPrivilege::HasBackupPrivilege() ) )
    {
        if( CUserPrivilege::Enabled == status )
        {
            str += L"Backup files: true\r\n";
        }
        else
        {
            str += L"Backup files: false\r\n";
        }
    }

    if( CUserPrivilege::Error != ( status = CUserPrivilege::HasDebugPrivilege() ) )
    {
        if( CUserPrivilege::Enabled == status )
        {
            str += L"Debug programs: true\r\n";
        }
        else
        {
            str += L"Debug programs: false\r\n";
        }
    }

    if( CUserPrivilege::Error != ( status = CUserPrivilege::HasSecurityPrivilege() ) )
    {
        if( CUserPrivilege::Enabled == status )
        {
            str += L"Security audit/log: true\r\n";
        }
        else
        {
            str += L"Security audit/log: false\r\n";
        }
    }

    GetEditCtrl().SetWindowText( str );

    return bResult;
}

BOOL CInfoView::DisplayModuleInformation()
{
    ASSERT( NULL != m_pfuncs );
    ASSERT( NULL != m_pfuncs.pfnCreateToolhelp32Snapshot );
    ASSERT( NULL != m_pfuncs.pfnModule32First );

    CProcTreeView* pView = CProcTreeView::GetView();
    ASSERT( NULL != pView );
    if( NULL == pView ) { return FALSE; }

    CString str, res, strName = pView->GetSelectedProcessName();
    INT pid = pView->GetSelectedProcess();

    do
    {
        if( PROCESS_ID_INVALID == pid || PROCESS_ID_SYSTEM == pid ) { break; }

        AutoHandle hModule;
        const DWORD dwFlags = TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32;

        hModule.Ref() = m_pfuncs.pfnCreateToolhelp32Snapshot( dwFlags, pid );
        ASSERT( NULL != hModule );
        if( NULL == hModule ) { break; }

        MODULEENTRY32 entry;
        ZeroMemory( &entry, sizeof(entry) );
        entry.dwSize = sizeof(entry);

        BOOL bResult = FALSE;
        bResult = m_pfuncs.pfnModule32First( hModule, &entry );
        // Fires too often ACCESS_DENIED
        // ASSERT( TRUE == bResult );
        if( FALSE == bResult ) { break; }

        CVersionInfo info;
        if( !info.Load( entry.szExePath ) ) { break; }

        strName = info.GetProductName();
        if( !strName.IsEmpty() ) { str += strName; str += L"\r\n"; }

        res = entry.szExePath;
        if( ! res.IsEmpty() ) {
            str += res; str += L"\r\n";
        }

        res = info.GetCompanyName();
        if( ! res.IsEmpty() ) {
            str += L"Company: "; str += res; str += L"\r\n";
        }

        res = info.GetProductVersionAsString();
        if( !res.IsEmpty() ) { str += L"Version: "; str += res; str += L"\r\n"; }

    } while(false);

    if( str.IsEmpty() ) { str = strName; }

    GetEditCtrl().SetWindowText( str );

    return !str.IsEmpty();
}

void CInfoView::HandleScrollBar(int /*cx*/, int cy)
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

void CInfoView::OnSize(UINT nType, int cx, int cy)
{
    CEditView::OnSize(nType, cx, cy);

    HandleScrollBar(cx, cy);
}

void CInfoView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
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
