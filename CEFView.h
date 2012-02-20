
// CEFView.h : interface of the CCEFView class
//

#pragma once


class CCEFView : public CView
{
protected: // create from serialization only
	CCEFView();
	DECLARE_DYNCREATE(CCEFView)

// Attributes
public:
	CCEFDoc* GetDocument() const;
	CefRefPtr<ClientHandler> m_clientHandler;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CCEFView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in CEFView.cpp
inline CCEFDoc* CCEFView::GetDocument() const
   { return reinterpret_cast<CCEFDoc*>(m_pDocument); }
#endif

