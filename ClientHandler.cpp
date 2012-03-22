#include "StdAfx.h"
#include "ClientHandler.h"
#include "CEFView.h"

#include <sstream>
#include <vector>
#include "string_util.h"

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

bool ClientHandler::OnBeforeResourceLoad(CefRefPtr<CefBrowser> browser,
                                         CefRefPtr<CefRequest> request,
                                         CefString& redirectUrl,
                                         CefRefPtr<CefStreamReader>& resourceStream,
                                         CefRefPtr<CefResponse> response,
                                         int loadFlags)
{
  CefString url = request->GetURL();
  CefURLParts parts;
  CefParseURL(url, parts);
  if(CefString(&parts.host) == "server")
  {
    CefRefPtr<CefFrame> frame = browser->GetMainFrame();
    frame->ExecuteJavaScript("log('process ajax request');", url, 0);
    std::string dump;
    DumpRequestContents(request, dump);
    dump = StringReplace(dump, "'", "\\'");
    dump = StringReplace(dump, "\n", "<br>");
    dump = StringReplace(dump, "&", "&amp;");
    dump = "log('" + dump + "');";
    frame->ExecuteJavaScript(dump, url, 0);

    /* don't send request to real server */
    dump = "{\"replaced\":\"value\"}";
    resourceStream = CefStreamReader::CreateForData((void*)dump.c_str(), dump.size());
    response->SetMimeType("text/plain");
    response->SetStatus(200);
    /**/
    return false;
  }
  return false;
}

void ClientHandler::OnResourceResponse(CefRefPtr<CefBrowser> browser,
                                       const CefString& url,
                                       CefRefPtr<CefResponse> response,
                                       CefRefPtr<CefContentFilter>& filter)
{
  CefURLParts parts;
  CefParseURL(url, parts);
  if(CefString(&parts.host) == "server")
  {
    CefRefPtr<CefFrame> frame = browser->GetMainFrame();
    frame->ExecuteJavaScript("log('process ajax request');", url, 0);
    std::string dump;
    DumpResponseContents(response, dump);
    dump = StringReplace(dump, "'", "\\'");
    dump = StringReplace(dump, "\n", "<br>");
    dump = StringReplace(dump, "&", "&amp;");
    dump = "log('" + dump + "');";
    frame->ExecuteJavaScript(dump, url, 0);
    filter = this;
  }
}

void ClientHandler::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefV8Context> context)
{
  REQUIRE_UI_THREAD();
  CefString url = frame->GetURL();
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

void ClientHandler::ProcessData(const void* data, int data_size,
                                CefRefPtr<CefStreamReader>& substitute_data)
{
  /* change response data */
  std::vector<char> v;
  v.resize(data_size);
  memcpy(&*v.begin(), data, data_size);
  std::string s(v.begin(), v.end());
  s = StringReplace(s, "}", ",\"ts\":\"1234567890\"}");
  substitute_data = CefStreamReader::CreateForData((void*)s.c_str(), s.size());
}

void ClientHandler::Drain(CefRefPtr<CefStreamReader>& remainder)
{
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
