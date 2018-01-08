// PevatDoc.cpp : implementation of the CPevatDoc class
//

#include "stdafx.h"

#include "PevatApp.h"
#include "Common.h"
#include "LogView.h"
#include "PevatDoc.h"
#include "VersionInfo.h"
#include "ProcTreeView.h"

#ifdef _DEBUG
# define new DEBUG_NEW
#endif

#ifdef NDEBUG
# ifdef OutputDebugString
#  undef OutputDebugString
#  define OutputDebugString(x) (void)0;
# endif
#endif

IMPLEMENT_DYNCREATE(CPevatDoc, CDocument)

BEGIN_MESSAGE_MAP(CPevatDoc, CDocument)
END_MESSAGE_MAP()

// CPevatDoc construction/destruction

CPevatDoc::CPevatDoc() { }

CPevatDoc::~CPevatDoc() { }

// CPevatDoc diagnostics

#ifdef _DEBUG
void CPevatDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CPevatDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG

BOOL CPevatDoc::ResetDocument()
{
    return TRUE;
}

BOOL CPevatDoc::SaveModified()
{
    SetModifiedFlag(FALSE);

    return CDocument::SaveModified();
}

void CPevatDoc::Serialize(CArchive& ar)
{
    if (!ar.IsStoring()) { return; }

    CFile* pFile = ar.GetFile();
    ASSERT( NULL != pFile );
    if( !pFile ) { return; }

    CLogView* pView = CLogView::GetView();
    ASSERT( NULL != pView );
    if( !pView ) { return; }

    CString sz;
    pView->GetEditCtrl().GetWindowText(sz);

    try
    {
        // Unicode BOM
        const BYTE b[] = { 0xFF, 0xFE };
        pFile->Write( b, _countof(b) );
        pFile->Write( sz.GetBuffer(), sz.GetLength()*sizeof(WCHAR) );
    }
    catch( CFileException& e )
    {
        WCHAR msg[ 128 ];
        StringCchPrintf( msg, _countof(msg), L"Failed to savelog file. LastError=%d", e.m_lOsError );
        AfxMessageBox( msg, MB_ICONERROR );    
    }
}
