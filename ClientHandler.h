#pragma once
#include "include/cef.h"
#include "include/cef_runnable.h"

#define REQUIRE_UI_THREAD()   ASSERT(CefCurrentlyOn(TID_UI));
#define REQUIRE_IO_THREAD()   ASSERT(CefCurrentlyOn(TID_IO));
#define REQUIRE_FILE_THREAD() ASSERT(CefCurrentlyOn(TID_FILE));

class ClientHandler :
	public CefClient,
	public CefLifeSpanHandler,
	public CefV8ContextHandler,
  public CefLoadHandler,
  public CefRequestHandler,
  public CefContentFilter
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
  virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE
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

  // CefRequestHandler methods
  virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefRequest> request,
                              NavType navType,
                              bool isRedirect) OVERRIDE;

  virtual bool OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefRequest> request,
                                    CefString& redirectUrl,
                                    CefRefPtr<CefStreamReader>& resourceStream,
                                    CefRefPtr<CefResponse> response,
                                    int loadFlags) OVERRIDE;

  virtual void OnResourceResponse(CefRefPtr<CefBrowser> browser,
                                  const CefString& url,
                                  CefRefPtr<CefResponse> response,
                                  CefRefPtr<CefContentFilter>& filter) OVERRIDE;
  
  virtual void ProcessData(const void* data, int data_size,
                           CefRefPtr<CefStreamReader>& substitute_data) OVERRIDE;

  virtual void Drain(CefRefPtr<CefStreamReader>& remainder) OVERRIDE;

  void GetURL();
  void JavaScriptTest();

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(ClientHandler);
  // Include the default locking implementation.
  IMPLEMENT_LOCKING(ClientHandler);
};

