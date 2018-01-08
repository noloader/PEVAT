#include "Common.h"
#include "Resource.h"
#include "afxwin.h"

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

protected:
    DECLARE_MESSAGE_MAP()

    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();	

public:
    CStatic m_labelProduct;
    CStatic m_labelVersion;
    CStatic m_labelBuild;

    CStatic m_labelCompany;
    CStatic m_labelCopyright;
    CStatic m_labelWebsite;    
};
