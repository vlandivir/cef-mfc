
// CEFView.cpp : implementation of the CCEFView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "cef_mfc.h"
#endif

#include "CEFDoc.h"
#include "CEFView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <sstream>

// CCEFView

IMPLEMENT_DYNCREATE(CCEFView, CView)

BEGIN_MESSAGE_MAP(CCEFView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCEFView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
  ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CCEFView construction/destruction

CCEFView::CCEFView()
{
	// TODO: add construction code here

}

CCEFView::~CCEFView()
{
}

BOOL CCEFView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CCEFView drawing

void CCEFView::OnDraw(CDC* /*pDC*/)
{
	CCEFDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CCEFView printing


void CCEFView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCEFView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCEFView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCEFView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CCEFView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCEFView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCEFView diagnostics

#ifdef _DEBUG
void CCEFView::AssertValid() const
{
	CView::AssertValid();
}

void CCEFView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCEFDoc* CCEFView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCEFDoc)));
	return (CCEFDoc*)m_pDocument;
}
#endif //_DEBUG


// CCEFView message handlers


void CCEFView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

    CefRefPtr<ClientHandler> client(new ClientHandler());
	m_clientHandler = client;

    CefWindowInfo info;
	RECT rect;
	GetClientRect(&rect);
	info.SetAsChild(GetSafeHwnd(), rect);

	CefBrowserSettings browserSettings;
  browserSettings.web_security_disabled = true;

	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	errno_t err;
	GetModuleFileNameA(NULL, path_buffer, sizeof(path_buffer));
	err = _splitpath_s(path_buffer, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	if (err != 0)
	{
		//TODO: Add Error Handler
	}
	std::string s = dir;
	s += "html";
	err = _makepath_s(path_buffer, _MAX_PATH, drive, s.c_str(), "index", "html");
	if (err != 0)
	{
		//TODO: Add Error Handler
	}
	CefBrowser::CreateBrowser(info, static_cast<CefRefPtr<CefClient> >(client), path_buffer, browserSettings); 

	// TODO: Add your specialized code here and/or call the base class
}


void CCEFView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	if(m_clientHandler.get())
	{
		CefRefPtr<CefBrowser> browser = m_clientHandler->GetBrowser();
		if(browser)
		{
			CefWindowHandle hwnd = browser->GetWindowHandle();
			RECT rect;
			this->GetClientRect(&rect);
			::SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);
		}
	}
	// TODO: Add your message handler code here
}

CCEFView * CCEFView::GetView(void)
{
  CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);

  CView * pView = pFrame->GetActiveView();

  if ( !pView )
  {
    return NULL;
  }
  // Fail if view is of wrong kind
  // (this could occur with splitter windows, or additional
  // views on a single document
  if ( ! pView->IsKindOf( RUNTIME_CLASS(CCEFView) ) )
  {
    return NULL;
  }
  return (CCEFView *) pView;
}


void CCEFView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  // TODO: Add your message handler code here and/or call default
  if(nChar == VK_F5)
  {
    m_clientHandler->GetBrowser()->Reload();
  }
  CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
