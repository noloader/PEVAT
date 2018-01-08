#include "StdAfx.h"

#include "AboutDlg.h"
#include "VersionInfo.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg(): CDialog(CAboutDlg::IDD) { }

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LABEL_PRODUCT, m_labelProduct);
    DDX_Control(pDX, IDC_LABEL_VERSION, m_labelVersion);
    DDX_Control(pDX, IDC_LABEL_COMPANY, m_labelCompany);
    DDX_Control(pDX, IDC_LABEL_COPYRIGHT, m_labelCopyright);
    DDX_Control(pDX, IDC_LABEL_WEBSITE, m_labelWebsite);
    DDX_Control(pDX, IDC_LABEL_BUILD, m_labelBuild);
}

BOOL CAboutDlg::OnInitDialog()
{
    BOOL bResult = FALSE;

    CDialog::OnInitDialog();

    WCHAR module[MAX_PATH];
    bResult = GetModuleFileName( NULL, module, _countof(module) );
    ASSERT( bResult );
    if( !bResult ) { return FALSE; }

    CVersionInfo info;
    bResult = info.Load( module );
    ASSERT( bResult );
    if( !bResult ) { return FALSE; }

    CString product = info.GetProductName();

#if defined WIN64
    product += L" (x64)";
#elif defined WIN32
    product += L" (x86)";
#else
    product += L" (unknown)";
#endif

    m_labelProduct.SetWindowText( product );
    m_labelVersion.SetWindowText( L"Version " + info.GetProductVersionAsString() );

    CHAR sz[64];
    StringCbPrintfA( sz, _countof(sz), "Built %s, %s", __DATE__, __TIME__ );
    m_labelBuild.SetWindowText( CString(sz) );

    m_labelCompany.SetWindowText( info.GetCompanyName() );
    m_labelCopyright.SetWindowText( info.GetLegalCopyright() );
    m_labelWebsite.SetWindowText( L"www.softwareintegrity.com" );

    return TRUE;
}
