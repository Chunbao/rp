
// t2Doc.cpp : implementation of the Ct2Doc class
//

#include "stdafx.h"
#include "t2.h"

#include "t2Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ct2Doc

IMPLEMENT_DYNCREATE(Ct2Doc, CDocument)

BEGIN_MESSAGE_MAP(Ct2Doc, CDocument)
END_MESSAGE_MAP()


// Ct2Doc construction/destruction

Ct2Doc::Ct2Doc()
{
	// TODO: add one-time construction code here

}

Ct2Doc::~Ct2Doc()
{
}

BOOL Ct2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// Ct2Doc serialization

void Ct2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// Ct2Doc diagnostics

#ifdef _DEBUG
void Ct2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ct2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Ct2Doc commands
