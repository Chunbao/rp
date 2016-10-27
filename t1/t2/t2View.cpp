
// t2View.cpp : implementation of the Ct2View class
//

#include "stdafx.h"
#include "t2.h"

#include "t2Doc.h"
#include "t2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ct2View

IMPLEMENT_DYNCREATE(Ct2View, CListView)

BEGIN_MESSAGE_MAP(Ct2View, CListView)
	ON_WM_STYLECHANGED()
END_MESSAGE_MAP()

// Ct2View construction/destruction

Ct2View::Ct2View()
{
	// TODO: add construction code here

}

Ct2View::~Ct2View()
{
}

BOOL Ct2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

void Ct2View::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

void Ct2View::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void Ct2View::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// Ct2View diagnostics

#ifdef _DEBUG
void Ct2View::AssertValid() const
{
	CListView::AssertValid();
}

void Ct2View::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

Ct2Doc* Ct2View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ct2Doc)));
	return (Ct2Doc*)m_pDocument;
}
#endif //_DEBUG


// Ct2View message handlers
void Ct2View::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//TODO: add code to react to the user changing the view style of your window	
	CListView::OnStyleChanged(nStyleType,lpStyleStruct);	
}
