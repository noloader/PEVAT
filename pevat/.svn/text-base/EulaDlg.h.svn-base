#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CEulaDlg dialog

class CEulaDlg: public CDialog
{
    DECLARE_DYNAMIC(CEulaDlg)

public:
    CEulaDlg(CWnd* pParent = NULL); // standard constructor
    virtual ~CEulaDlg();

    // Dialog Data
    enum{ IDD = IDD_EULADLG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support

    DECLARE_MESSAGE_MAP()
protected:
    virtual BOOL OnInitDialog();

    afx_msg void OnBnClickedButtonAccept();
    afx_msg void OnBnClickedButtonDecline();

    BOOL SetDialogTitle();
    BOOL SetEulaText();

private:
    CRichEditCtrl m_reditEula;
    CButton m_btnDecline;
};
