#pragma once

#include "PevatApp.h"
#include "PevatDoc.h"
#include "StrSafe.h"

class CMainFrame;

class CLogView: public CEditView
{
    DECLARE_DYNCREATE(CLogView)

    // CMainFrame needs access to OnUpdate
    friend CMainFrame;

public:
    static CLogView* GetView();
    static void WriteLine( LPCWSTR );

protected:
    CLogView();
    virtual ~CLogView();

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

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
    afx_msg void OnUpdateLogClearAll(CCmdUI *pCmdUI);
    afx_msg void OnLogClearAll();

    DECLARE_MESSAGE_MAP()

protected:
    void HandleScrollBar(int cx, int cy);

private:
    INT m_fontHeight;
    CBrush m_whiteBrush;
    CFont m_font;    
public:
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

#ifndef _DEBUG// debug version in LogView.cpp
inline CPevatDoc* CLogView::GetDocument()
{ return reinterpret_cast<CPevatDoc*>(m_pDocument); }
#endif
