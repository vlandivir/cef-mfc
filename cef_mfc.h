
// cef_mfc.h : main header file for the cef_mfc application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "include/cef.h"


// CCEFApp:
// See cef_mfc.cpp for the implementation of this class
//

class CCEFApp : public CWinAppEx
{
public:
	CCEFApp();

	CefRefPtr<CefApp> app;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern CCEFApp theApp;
