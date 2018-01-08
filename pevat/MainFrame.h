// MainFrame.h : interface of the CMainFrame class
//

#pragma once

class CLogView;
class CPevatDoc;
class CProcTreeView;
class CProcEventView;
class CAttackView;

class CMainFrame: public CFrameWnd
{

protected:
    CMainFrame();
    virtual ~CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

    // Generated message map functions
protected:
    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg LRESULT OnMsgProcessListChanged( WPARAM wparam, LPARAM lparam );
    afx_msg LRESULT OnMsgLogMessage( WPARAM wparam, LPARAM lparam );
    afx_msg LRESULT OnMsgAttackStart( WPARAM wparam, LPARAM lparam );
    afx_msg LRESULT OnMsgAttackStop( WPARAM wparam, LPARAM lparam );
    afx_msg LRESULT OnMsgUpdateHandleTable(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMsgUpdateHandleTableItem(WPARAM wparam, LPARAM lparam);
    afx_msg LRESULT OnMsgAppUpdateAvailable(WPARAM wparam, LPARAM lparam);

    afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
    afx_msg void OnUpdateViewRefresh(CCmdUI *pCmdUI);

    afx_msg void OnEditCopy();
    afx_msg void OnViewRefresh();

    afx_msg void OnUpdatePrivilegesBackup(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePrivilegesDebug(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePrivilegesAssignToken(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePrivilegesActAsOS(CCmdUI *pCmdUI);
    afx_msg void OnUpdatePrivilegesSecurity(CCmdUI *pCmdUI);
    BOOL EnablePrivilegesMenuItem(CCmdUI *pCmdUI);

    afx_msg void OnPrivilegesDebug();
    afx_msg void OnPrivilegesSecurity();
    afx_msg void OnPrivilegesBackup();
    afx_msg void OnUpdatePrivilegesAssignToken();
    afx_msg void OnPrivilegesTcb();
    void UpdatePrivilegMenuItem();

    afx_msg void OnUpdateTotalEvents(CCmdUI *pCmdUI);
    afx_msg void OnUpdateNamedEvents(CCmdUI *pCmdUI);
    afx_msg void OnUpdateUnnamedEvents(CCmdUI *pCmdUI);

    afx_msg void OnUpdateAttackInvert(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackPulse(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackSet(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackReset(CCmdUI *pCmdUI);

    afx_msg void OnUpdateTotalEventsAll(CCmdUI *pCmdUI);
    afx_msg void OnUpdateNamedEventsAll(CCmdUI *pCmdUI);
    afx_msg void OnUpdateUnnamedEventsAll(CCmdUI *pCmdUI);

    afx_msg void OnUpdateAttackInvertAll(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackPulseAll(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackSetAll(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAttackResetAll(CCmdUI *pCmdUI);

    afx_msg void OnAttackInvert();
    afx_msg void OnAttackPulse();
    afx_msg void OnAttackSet();
    afx_msg void OnAttackReset();

    afx_msg void OnAttackInvertAll();
    afx_msg void OnAttackPulseAll();
    afx_msg void OnAttackSetAll();
    afx_msg void OnAttackResetAll();

    void UpdateAttackMenuItem(CCmdUI *pCmdUI, BOOL All = FALSE);
    void OnAttackMenuItem(UINT item);

    DECLARE_MESSAGE_MAP()

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CStatusBar m_wndStatusBar;
    CSplitterWnd m_VertSplitter;
    CSplitterWnd m_HorzSplitter;

public:
    CPevatDoc* GetDocument();

    CLogView* GetLogView();
    CProcTreeView* GetProcTreeView();
    CProcEventView* GetProcEventView();
    CAttackView* GetAttackView();
};

#ifndef _DEBUG// debug version in ProcTreeView.cpp
inline CPevatDoc* CMainFrame::GetDocument()
{ return reinterpret_cast<CPevatDoc*>(GetActiveDocument()); }
#endif