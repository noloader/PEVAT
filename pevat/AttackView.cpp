// AttackView.cpp : implementation file
//

#include "stdafx.h"
#include "PevatApp.h"
#include "Common.h"
#include "LogView.h"
#include "PevatDoc.h"
#include "MainFrame.h"
#include "AttackView.h"
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

BEGIN_MESSAGE_MAP(CAttackView, CFormView)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_ATTACK_START, &CAttackView::OnBnClickedButtonAttackStart)
    ON_BN_CLICKED(IDC_BUTTON_ATTACK_STOP, &CAttackView::OnBnClickedButtonAttackStop)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CAttackView, CFormView)

CAttackView::CAttackView()
: CFormView(CAttackView::IDD) { }

CAttackView::~CAttackView(){ }

CAttackView* CAttackView::GetView()
{
    CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
    ASSERT( pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) );
    if( !pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)) ) { return NULL; }

    return pFrame->GetAttackView();
}

void CAttackView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_LABEL_ATTACK, m_labelAttack);

    DDX_Control(pDX, IDC_GROUP_TARGETS, m_groupTargets);
    DDX_Control(pDX, IDC_RADIO_TARGET_SELECTED, m_radioSelected);
    DDX_Control(pDX, IDC_RADIO_TARGET_ALL, m_radioAll);
    DDX_Control(pDX, IDC_CHECK_TARGET_REVERSE, m_checkReverse);
    DDX_Control(pDX, IDC_CHECK_EXCLUDE_WELLKNOWN, m_checkExcludeKnown);

    DDX_Control(pDX, IDC_GROUP_ATTACK, m_groupAttack);
    DDX_Control(pDX, IDC_RADIO_ATTACK_INVERT, m_radioInvert);
    DDX_Control(pDX, IDC_RADIO_ATTACK_PULSE, m_radioPulse);
    DDX_Control(pDX, IDC_RADIO_ATTACK_SET, m_radioSet);
    DDX_Control(pDX, IDC_RADIO_ATTACK_RESET, m_radioReset);

    DDX_Control(pDX, IDC_GROUP_DELAY, m_groupDelay);
    DDX_Control(pDX, IDC_SPIN_MILLISECONDS, m_spinMilliseconds);
    DDX_Control(pDX, IDC_EDIT_MILLISECONDS, m_editMilliseconds);

    DDX_Control(pDX, IDC_CHECK_AUTOSTOP, m_checkAutoStop);
    DDX_Control(pDX, IDC_BUTTON_ATTACK_START, m_buttonStart);
    DDX_Control(pDX, IDC_BUTTON_ATTACK_STOP, m_buttonStop);
}

// CAttackView diagnostics

#ifdef _DEBUG
void CAttackView::AssertValid() const
{
    CFormView::AssertValid();
}

void CAttackView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}

CPevatDoc* CAttackView::GetDocument()
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPevatDoc)));
    return (CPevatDoc*)m_pDocument;
}
#endif //_DEBUG

void CAttackView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    m_checkExcludeKnown.SetCheck( BST_CHECKED );

    m_radioAll.SetCheck( BST_CHECKED );
    m_radioInvert.SetCheck( BST_CHECKED );

    m_checkAutoStop.SetCheck( BST_CHECKED );

    m_spinMilliseconds.SetBuddy( &m_editMilliseconds );
    m_spinMilliseconds.SetRange32( 50, UD_MAXVAL );
    m_spinMilliseconds.SetPos32(500);

    SetStartStopButtonState();
}

void CAttackView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

    const INT LMARGIN = 10;
    const INT RMARGIN = LMARGIN*2;

    // 'Physical' size of the view
    CRect cl;
    GetClientRect( &cl );
    INT WIDTH = cl.Width();

    // 'Virtual' size of the view
    CSize size = GetTotalSize();
    if( WIDTH <= size.cx ) { WIDTH = size.cx; }

    WIDTH -= RMARGIN;

    if( IsWindow( m_labelAttack.GetSafeHwnd() ) ) {
        CRect ctl;
        m_labelAttack.GetWindowRect( &ctl );
        ScreenToClient( &ctl );
        m_labelAttack.SetWindowPos( NULL, ctl.left, ctl.top, WIDTH, ctl.Height(), 0 );
    }

    if( IsWindow( m_groupTargets.GetSafeHwnd() ) ) {
        CRect ctl;
        m_groupTargets.GetWindowRect( &ctl );
        ScreenToClient( &ctl );
        m_groupTargets.SetWindowPos( NULL, ctl.left, ctl.top, WIDTH, ctl.Height(), 0 );
    }

    if( IsWindow( m_groupAttack.GetSafeHwnd() ) ) {
        CRect ctl;
        m_groupAttack.GetWindowRect( &ctl );
        ScreenToClient( &ctl );
        m_groupAttack.SetWindowPos( NULL, ctl.left, ctl.top, WIDTH, ctl.Height(), 0 );
    }

    if( IsWindow( m_groupDelay.GetSafeHwnd() ) ) {
        CRect ctl;
        m_groupDelay.GetWindowRect( &ctl );
        ScreenToClient( &ctl );
        m_groupDelay.SetWindowPos( NULL, ctl.left, ctl.top, WIDTH, ctl.Height(), 0 );
    }

    if( IsWindow( m_checkAutoStop.GetSafeHwnd() ) ) {
        CRect ctl;
        m_checkAutoStop.GetWindowRect( &ctl );
        ScreenToClient( &ctl );
        m_checkAutoStop.SetWindowPos( NULL, ctl.left, ctl.top, WIDTH, ctl.Height(), 0 );
    }
}

DWORD CAttackView::GetDelay() const
{
    BOOL bUnused = FALSE;
    return m_spinMilliseconds.GetPos32( &bUnused );
}

void CAttackView::SetStartStopButtonState()
{
    // No ASSERT
    if( !IsWindow(m_buttonStart.GetSafeHwnd()) ||
        !IsWindow(m_buttonStop.GetSafeHwnd()) ) { return; }

    do
    {
        CProcEventView* pView = CProcEventView::GetView();
        ASSERT( NULL != pView );
        if( pView == NULL ) { break; }

        INT count = pView->GetTotalEvents();
        if( 0 == count )
        {
            m_buttonStart.EnableWindow( FALSE );
            m_buttonStop.EnableWindow( FALSE );
            break;
        }

        if( IsAttackInProgress() )
        {
            m_buttonStart.EnableWindow( FALSE );
            m_buttonStop.EnableWindow( TRUE );
        }
        else
        {
            m_buttonStart.EnableWindow( TRUE );
            m_buttonStop.EnableWindow( FALSE );
        }

    } while(false);
}
void CAttackView::OnBnClickedButtonAttackStart()
{
    BOOL all = FALSE, selected = FALSE;
    BOOL invert = FALSE, pulse = FALSE, set = FALSE, reset = FALSE;

    all = (BST_CHECKED == m_radioAll.GetCheck());
    selected = (BST_CHECKED == m_radioSelected.GetCheck());
    ASSERT( all == !selected );

    invert = (BST_CHECKED == m_radioInvert.GetCheck());
    pulse = (BST_CHECKED == m_radioPulse.GetCheck());
    set = (BST_CHECKED == m_radioSet.GetCheck());
    reset = (BST_CHECKED == m_radioReset.GetCheck());

    UINT message = 0;

    if( all )
    {
        if( invert ) { message = HINT_ATTACK_INVERTALL; }
        else if( pulse ) { message = HINT_ATTACK_PULSEALL; }
        else if( set ) { message = HINT_ATTACK_SETALL; }
        else if( reset ) { message = HINT_ATTACK_RESETALL; }
    }
    else if( selected )
    {
        if( invert ) { message = HINT_ATTACK_INVERTSELECTED; }
        else if( pulse ) { message = HINT_ATTACK_PULSESELECTED; }
        else if( set ) { message = HINT_ATTACK_SETSELECTED; }
        else if( reset ) { message = HINT_ATTACK_RESETSELECTED; }
    }
    else
    {
        ASSERT( FALSE );
    }

    CPevatDoc* pDoc = GetDocument();
    ASSERT( NULL != pDoc );
    if( NULL == pDoc ) { return; }

    pDoc->UpdateAllViews( this, message, NULL );

    SetStartStopButtonState();
}

void CAttackView::OnBnClickedButtonAttackStop()
{
    BOOL bResult = SetEvent( g_hEvntCancelProcessing );
    ASSERT( bResult ); UNUSED( bResult );

    SetStartStopButtonState();
}

void CAttackView::AttackComplete()
{
    // Scratch
    BOOL bResult = FALSE;

    bResult = ResetEvent( g_hEvntProcessing );
    ASSERT( bResult );

    bResult = ResetEvent( g_hEvntCancelProcessing );
    ASSERT( bResult );

    SetStartStopButtonState();
}

void CAttackView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    switch( lHint )
    {
    case HINT_HANDLETABLE_CHANGED:
        {
            SetStartStopButtonState();
            break;
        }
    case HINT_ATTACK_INVERTSELECTED:
        {
            // TODO...
            SelectAttackButton(m_radioSelected);
            SelectAttackButton(m_radioInvert);
            OnBnClickedButtonAttackStart();
            break;
        }
    case HINT_ATTACK_PULSESELECTED:
        {
            // TODO...
            SelectAttackButton(m_radioSelected);
            SelectAttackButton(m_radioPulse);
            OnBnClickedButtonAttackStart();
            break;
        }
    case HINT_ATTACK_SETSELECTED:
        {
            // TODO...
            SelectAttackButton(m_radioSelected);
            SelectAttackButton(m_radioSet);
            OnBnClickedButtonAttackStart();
            break;
        }
    case HINT_ATTACK_RESETSELECTED:
        {
            // TODO...
            SelectAttackButton(m_radioSelected);
            SelectAttackButton(m_radioReset);
            OnBnClickedButtonAttackStart();
            break;
        }
    case HINT_ATTACK_START:
        {
            SetStartStopButtonState();
            break;
        }
    case HINT_ATTACK_STOP:
        {
            AttackComplete();
            SetStartStopButtonState();
            break;
        }
    default:
        {
            CFormView::OnUpdate(pSender, lHint, pHint);
            break;
        }
    }
}

void CAttackView::SelectAttackButton(CButton& button)
{
    button.SendMessage( WM_LBUTTONDOWN, 0, 0 );
    button.SendMessage( WM_LBUTTONUP, 0, 0 );
}
