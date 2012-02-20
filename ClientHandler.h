#pragma once
#include "include/cef.h"
class ClientHandler :
	public CefClient,
	public CefLifeSpanHandler
{
protected:
	CefRefPtr<CefBrowser> m_Browser;

public:
	ClientHandler(void);
	~ClientHandler(void);

  CefRefPtr<CefBrowser> GetBrowser() { return m_Browser; }

  // CefClient methods
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE
      { return this; }

  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(ClientHandler);
  // Include the default locking implementation.
  IMPLEMENT_LOCKING(ClientHandler);
};

