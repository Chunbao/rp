
// t2View.h : interface of the Ct2View class
//


#pragma once


class Ct2View : public CListView
{
protected: // create from serialization only
	Ct2View();
	DECLARE_DYNCREATE(Ct2View)

// Attributes
public:
	Ct2Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~Ct2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in t2View.cpp
inline Ct2Doc* Ct2View::GetDocument() const
   { return reinterpret_cast<Ct2Doc*>(m_pDocument); }
#endif

