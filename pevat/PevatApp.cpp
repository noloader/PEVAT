// Peat.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "PevatApp.h"
#include "EulaDlg.h"
#include "PevatDoc.h"
#include "AboutDlg.h"
#include "MainFrame.h"
#include "UpdateCheck.h"
#include "ProcTreeView.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
# undef OutputDebugString
# define OutputDebugString(x) (void)0;
# endif
#endif

BEGIN_MESSAGE_MAP(CPevatApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, &CPevatApp::OnAppAbout)
    ON_COMMAND(ID_APP_LICENSE, &CPevatApp::OnAppLicense)
END_MESSAGE_MAP()

// The one and only CPevatApp object
CPevatApp theApp;

// Helps supress logging message
volatile bool g_bShutdown = false;
volatile bool g_bEulaAccepted = false;

AutoHandle g_hEvntProcessing;
AutoHandle g_hEvntCancelProcessing;

CPevatApp::CPevatApp(){ }

BOOL CPevatApp::InitInstance()
{
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles. Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES ;
    InitCommonControlsEx(&InitCtrls);

    AfxInitRichEdit2();

    CWinApp::InitInstance();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    SetRegistryKey(L"Software Integrity");

    // Does nothing if the EULA was previously accepted. If not
    // previously accepted, the function will display the EULA
    // and offer Accept/Decline. If Declined, the function
    // returns FALSE and we bail.
    if( FALSE == HandleEulaPreamble() ) { return FALSE; }

#ifndef WIN64
    HandleWow64();
#endif

    // Load standard INI file options (including MRU)
    LoadStdProfileSettings(0);

    // Register the application's document templates. Document templates
    // serve as the connection between documents, frame windows and views
    CSingleDocTemplate* pDocTemplate = NULL;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CPevatDoc),
        RUNTIME_CLASS(CMainFrame), // main SDI frame window
        RUNTIME_CLASS(CProcTreeView));

    ASSERT( NULL != pDocTemplate );
    if (!pDocTemplate) { return FALSE; }

    AddDocTemplate(pDocTemplate);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // Dispatch commands specified on the command line. Will return FALSE if
    // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    if (!ProcessShellCommand(cmdInfo)) { return FALSE; }

    // The one and only window has been initialized, so show and update it
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    g_hEvntProcessing.Ref() = CreateEvent( NULL, TRUE, FALSE, L"Attack in Progress" );
    ASSERT( NULL != g_hEvntProcessing );

    g_hEvntCancelProcessing.Ref() = CreateEvent( NULL, TRUE, FALSE, L"Cancel Attack" );
    ASSERT( NULL != g_hEvntCancelProcessing );

    // call DragAcceptFiles only if there's a suffix
    // In an SDI app, this should occur after ProcessShellCommand
    return TRUE;
}

// http://msdn.microsoft.com/en-us/library/3e077sxt(VS.80).aspx
BOOL CPevatApp::OnIdle(LONG lCount)
{
    static bool launched = false;

    CWinApp::OnIdle(lCount);

    if( !launched && g_bEulaAccepted )
    {
        LaunchUpdateThread();
        launched = true;
    }

    // Nonzero to receive more idle processing time;
    //  0 if no more idle time is needed.
    return (INT)(launched != true);
}

#ifndef WIN64
void CPevatApp::HandleWow64() const
{
    Wow64ProcFunctions funcs;
    ASSERT( NULL != funcs );

    if( NULL != funcs.pfnIsWow64Process )
    {
        BOOL bIsWow64 = FALSE;
        if( funcs.pfnIsWow64Process( GetCurrentProcess(), &bIsWow64 ) )
        {
            if( bIsWow64 )
            {
                AfxMessageBox( L"The x86 version of this program " \
                    L"does not produce correct results when run under Wow64. " \
                    L"An x64 version is available for download.", MB_ICONWARNING );
            }
        }
    }
}
#endif

BOOL CPevatApp::HandleEulaPreamble() const
{
    BOOL bResult = FALSE;

    do
    {
        CWinApp* pApplication = AfxGetApp();
        ASSERT( NULL != pApplication );
        if( NULL == pApplication ) { break; }

        INT eula = 0;
        eula = pApplication->GetProfileInt( L"Settings", L"EULA", 0 );

        // Already accepted
        if( 1 == eula ) { bResult = TRUE; break; }

        CEulaDlg dlg;
        if( IDOK == dlg.DoModal() )
        {
            bResult = pApplication->WriteProfileInt( L"Settings", L"EULA", 1 );
        }
        else
        {
            pApplication->WriteProfileInt( L"Settings", L"EULA", 0 );
        }

    } while(false);

    g_bEulaAccepted = !!bResult;

    return bResult;
}

// App command to run the dialog
void CPevatApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

void CPevatApp::OnAppLicense()
{
    CEulaDlg dlg;
    if( IDCANCEL == dlg.DoModal() )
    {
        AfxGetApp()->WriteProfileInt( L"Settings", L"EULA", 0 );
        AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
    }
}
