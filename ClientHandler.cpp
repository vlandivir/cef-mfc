#include "StdAfx.h"
#include "ClientHandler.h"
#include "CEFView.h"

ClientHandler::ClientHandler(void)
{
}


ClientHandler::~ClientHandler(void)
{
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	REQUIRE_UI_THREAD();
	m_Browser = browser;
}

bool ClientHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefRequest> request,
                                   NavType navType,
                                   bool isRedirect)
{
  CefString url = request->GetURL();
  return false;
}

void ClientHandler::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefV8Context> context)
{
  REQUIRE_UI_THREAD();
  CefString url = frame->GetURL();
  //frame->ExecuteJavaScript("alert('JavaScript Alert!!! OnContextCreated')", url, 0);
}

void ClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame) 
{
}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              int httpStatusCode) 
{
  REQUIRE_UI_THREAD();
  CefString url = frame->GetURL();
  //frame->ExecuteJavaScript("alert('JavaScript Alert!!! OnLoadEnd')", url, 0);
}

void ClientHandler::GetURL(void)
{
  REQUIRE_UI_THREAD();
  CCEFView *pView = CCEFView::GetView();
  CefRefPtr<CefBrowser> browser = pView->m_clientHandler->GetBrowser();
  CefRefPtr<CefFrame> frame = browser->GetMainFrame();
  CefString url = frame->GetURL();  
}

void ClientHandler::JavaScriptTest(void)
{
  REQUIRE_UI_THREAD();
  CCEFView *pView = CCEFView::GetView();
  CefRefPtr<CefBrowser> browser = pView->m_clientHandler->GetBrowser();
  CefRefPtr<CefFrame> frame = browser->GetMainFrame();

  CefRefPtr<CefV8Context> v8Context = frame->GetV8Context();
  CefString url = frame->GetURL();

  if (!v8Context.get()) 
  {
    frame->ExecuteJavaScript("alert('Failed to get V8 context!');", url, 0);
  } 
  else if (v8Context->Enter()) 
  {
    CefRefPtr<CefV8Value> globalObj = v8Context->GetGlobal();
    CefRefPtr<CefV8Value> evalFunc = globalObj->GetValue("eval");

    CefRefPtr<CefV8Value> arg0 = CefV8Value::CreateString("1+2");

    CefV8ValueList args;
    args.push_back(arg0);

    CefRefPtr<CefV8Value> retVal;
    CefRefPtr<CefV8Exception> exception;
    if (evalFunc->ExecuteFunctionWithContext(v8Context, globalObj, args, retVal, exception, false)) 
    {
      if (retVal.get()) 
      {
        frame->ExecuteJavaScript(
            std::string("alert('InvokeScript returns ") +
            retVal->GetStringValue().ToString() + "!');",
            url, 0);
      } 
      else 
      {
        frame->ExecuteJavaScript(
            std::string("alert('InvokeScript returns exception: ") +
            exception->GetMessage().ToString() + "!');",
            url, 0);
      }
    } 
    else 
    {
      frame->ExecuteJavaScript("alert('Failed to execute function!');", url, 0);
    }

    v8Context->Exit();
  } 
  else 
  {
    frame->ExecuteJavaScript("alert('Failed to enter into V8 context!');", url, 0);
  }
}
