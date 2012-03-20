#pragma once
#include "include/cef.h"
#include "include/cef_runnable.h"

#define REQUIRE_UI_THREAD()   ASSERT(CefCurrentlyOn(TID_UI));

class ClientHandler :
	public CefClient,
	public CefLifeSpanHandler,
	public CefV8ContextHandler,
  public CefLoadHandler
{
protected:
public:
	CefRefPtr<CefBrowser> m_Browser;

public:
	ClientHandler(void);
	~ClientHandler(void);

  CefRefPtr<CefBrowser> GetBrowser() { return m_Browser; }

  // CefClient methods
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE
      { return this; }
	virtual CefRefPtr<CefV8ContextHandler> GetV8ContextHandler() OVERRIDE
		{ return this; }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE
      { return this; }

  // CefLifeSpanHandler methods
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  
  // CefV8ContextHandler methods
  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) OVERRIDE;

  // CefLoadHandler methods
  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame) OVERRIDE;
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode) OVERRIDE;

  void GetURL();
  void JavaScriptTest();

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(ClientHandler);
  // Include the default locking implementation.
  IMPLEMENT_LOCKING(ClientHandler);
};

