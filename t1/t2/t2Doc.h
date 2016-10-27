
// t2Doc.h : interface of the Ct2Doc class
//


#pragma once


class Ct2Doc : public CDocument
{
protected: // create from serialization only
	Ct2Doc();
	DECLARE_DYNCREATE(Ct2Doc)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~Ct2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


