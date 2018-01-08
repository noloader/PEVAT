#pragma once

#include "Resource.h"

// CUpdateWarnDlg dialog

class CUpdateWarnDlg : public CDialog
{
	DECLARE_DYNAMIC(CUpdateWarnDlg)

public:
	CUpdateWarnDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUpdateWarnDlg();

// Dialog Data
	enum { IDD = IDD_UPDATEWARNDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    CButton m_chkNoShow;
};
