// EulaDlg.cpp : implementation file
//

#include "stdafx.h"

#include "PevatApp.h"
#include "EulaDlg.h"
#include "VersionInfo.h"

// CEulaDlg dialog

IMPLEMENT_DYNAMIC(CEulaDlg, CDialog)

CEulaDlg::CEulaDlg(CWnd* pParent /*=NULL*/)
: CDialog(CEulaDlg::IDD, pParent) { }

CEulaDlg::~CEulaDlg(){ }

void CEulaDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RICHEDIT_EULA, m_reditEula);
    DDX_Control(pDX, IDC_BUTTON_DECLINE, m_btnDecline);
}

BEGIN_MESSAGE_MAP(CEulaDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_ACCEPT, &CEulaDlg::OnBnClickedButtonAccept)
    ON_BN_CLICKED(IDC_BUTTON_DECLINE, &CEulaDlg::OnBnClickedButtonDecline)
END_MESSAGE_MAP()

// CEulaDlg message handlers

BOOL CEulaDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetDialogTitle();
    SetEulaText();

    m_btnDecline.SetFocus();

    return FALSE;
}

BOOL CEulaDlg::SetDialogTitle()
{
    BOOL bResult = FALSE;

    do
    {
        CString title;

        WCHAR module[MAX_PATH];
        bResult = GetModuleFileName( NULL, module, _countof(module) );
        ASSERT( bResult );
        if( !bResult ) { break; }

        CVersionInfo info;
        bResult = info.Load( module );
        ASSERT( bResult );
        if( !bResult ) { break; }

        title = info.GetProductName();
        title += L" License Agreement";

        SetWindowText(title);

        bResult = TRUE;

    } while(false);

    return bResult;
}

BOOL CEulaDlg::SetEulaText()
{
    BOOL bResult = FALSE;

    HRSRC hResource = NULL;
    HGLOBAL hGlobal = NULL;
    LPCSTR lpText = NULL;

    do
    {
        hResource = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_EULA_TEXT), L"TEXT");
        ASSERT( NULL != hResource );
        if( NULL == hResource ) { break; }

        hGlobal = LoadResource(AfxGetResourceHandle(), hResource);
        ASSERT( NULL != hGlobal );
        if( NULL == hGlobal ) { break; }

        lpText = (LPCSTR)LockResource(hGlobal);
        ASSERT( NULL != lpText );
        if( NULL == lpText ) { break; }

        INT len = SizeofResource(AfxGetResourceHandle(), hResource);
        ASSERT(len);
        if( !len ) { break; }

        CString text(lpText, len);
        m_reditEula.SetWindowText(text);

        bResult = TRUE;

    } while(false);

    if( lpText ) {
        UnlockResource(hGlobal);
    }

    return bResult;
}

void CEulaDlg::OnBnClickedButtonAccept()
{
    CDialog::OnOK();
}

void CEulaDlg::OnBnClickedButtonDecline()
{
    CDialog::OnCancel();
}
