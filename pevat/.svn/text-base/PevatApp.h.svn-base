// Peat.h : main header file for the Peat application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "Resource.h"
#include "Common.h"

class CPevatApp: public CWinApp
{
public:
    CPevatApp();
    DECLARE_MESSAGE_MAP()

protected:
    virtual BOOL InitInstance();
    virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing

    afx_msg void OnAppAbout();
    afx_msg void OnAppLicense();

    BOOL HandleEulaPreamble() const;

#ifndef WIN64
    void HandleWow64() const;
#endif
};

extern CPevatApp theApp;

extern volatile bool g_bShutdown;

extern AutoHandle g_hEvntProcessing;
extern AutoHandle g_hEvntCancelProcessing;