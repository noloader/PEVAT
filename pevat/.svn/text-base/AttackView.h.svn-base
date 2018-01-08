#pragma once

#include "PevatApp.h"
#include "PevatDoc.h"

// CAttackView form view

class CAttackView: public CFormView
{
    DECLARE_DYNCREATE(CAttackView)
    enum{ IDD = IDD_ATTACKCMDVIEW };

public:
    static CAttackView* GetView();
    DWORD GetDelay() const;
    BOOL GetReverseList() const { return BST_CHECKED == m_checkReverse.GetCheck(); }
    BOOL GetAutoStop() const { return BST_CHECKED == m_checkAutoStop.GetCheck(); }
    BOOL GetExcludeKnown() const { return BST_CHECKED == m_checkExcludeKnown.GetCheck(); }

protected:
    CAttackView(); // protected constructor used by dynamic creation
    virtual ~CAttackView();

protected:
    CPevatDoc* GetDocument();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    virtual void OnInitialUpdate();
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedButtonAttackStart();
    afx_msg void OnBnClickedButtonAttackStop();

    DECLARE_MESSAGE_MAP()

protected:
    void AttackComplete(); 
    void SetStartStopButtonState();
    void SelectAttackButton(CButton&);

private:
    // Pane title
    CStatic m_labelAttack;

    // Targets group box
    CStatic m_groupTargets;
    CButton m_radioSelected;
    CButton m_radioAll;

    // Do it in reverse
    CButton m_checkReverse;
    CButton m_checkExcludeKnown;

    // Attack group box
    CStatic m_groupAttack;
    CButton m_radioInvert;
    CButton m_radioPulse;
    CButton m_radioSet;
    CButton m_radioReset;

    // Delay group box
    CStatic m_groupDelay;
    CEdit m_editMilliseconds;
    CSpinButtonCtrl m_spinMilliseconds;

    // Start/Stop buttons
    CButton m_buttonStart;
    CButton m_buttonStop;
    CButton m_checkAutoStop;
};

#ifndef _DEBUG// debug version in ProcTreeView.cpp
inline CPevatDoc* CAttackView::GetDocument()
{ return reinterpret_cast<CPevatDoc*>(m_pDocument); }
#endif
