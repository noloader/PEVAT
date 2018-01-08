#pragma once

#include "PevatApp.h"
#include "Common.h"
#include "PevatDoc.h"
#include "StrSafe.h"

class CInfoView: public CEditView
{
    DECLARE_DYNCREATE(CInfoView)

public:
    static CInfoView* GetView();

protected:
    CInfoView();
    virtual ~CInfoView();

protected:
    CPevatDoc* GetDocument();

#ifdef _DEBUG
    void AssertValid() const;
    void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnInitialUpdate();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);    

    DECLARE_MESSAGE_MAP()

protected:
    BOOL ResetView();    
    BOOL UpdateVersionInformation();
    BOOL DisplayComputerAndUserInformation();
    BOOL DisplayModuleInformation();

    void HandleScrollBar(int cx, int cy);

private:
    ProcessFunctions m_pfuncs;

    INT m_fontHeight;
    CBrush m_whiteBrush;
    CFont m_font;
public:
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

#ifndef _DEBUG// debug version in InfoView.cpp
inline CPevatDoc* CInfoView::GetDocument()
{ return reinterpret_cast<CPevatDoc*>(m_pDocument); }
#endif
