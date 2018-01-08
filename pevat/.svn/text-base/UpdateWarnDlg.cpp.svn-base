// UpdateBeginDlg.cpp : implementation file
//

#include "stdafx.h"

#include "PevatApp.h"
#include "UpdateCheck.h"
#include "UpdateWarnDlg.h"

// CUpdateWarnDlg dialog

IMPLEMENT_DYNAMIC(CUpdateWarnDlg, CDialog)

CUpdateWarnDlg::CUpdateWarnDlg(CWnd* pParent /*=NULL*/)
: CDialog(CUpdateWarnDlg::IDD, pParent) { }

CUpdateWarnDlg::~CUpdateWarnDlg() { }

void CUpdateWarnDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHECK_NOSHOW, m_chkNoShow);
}

BEGIN_MESSAGE_MAP(CUpdateWarnDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CUpdateWarnDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CUpdateWarnDlg message handlers

void CUpdateWarnDlg::OnBnClickedOk()
{
    INT value = 0;
    if( BST_CHECKED == m_chkNoShow.GetCheck() ) { value = 1; }

    CWinApp* pApplication = AfxGetApp();
    ASSERT( NULL != pApplication );
    if( NULL != pApplication )
    {
        pApplication->WriteProfileInt( L"Settings", L"No Update Warning", value );
    }

    OnOK();
}
