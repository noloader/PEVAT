// PevatDoc.h : interface of the CPevatDoc class
//

#pragma once

#include "Common.h"
#include "Aclapi.h"

class CPevatDoc: public CDocument
{
protected: // create from serialization only
    CPevatDoc();
    DECLARE_DYNCREATE(CPevatDoc)
    virtual ~CPevatDoc();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void Serialize(CArchive& ar);
    virtual BOOL SaveModified();

    DECLARE_MESSAGE_MAP()

public:
    BOOL ResetDocument();

};


