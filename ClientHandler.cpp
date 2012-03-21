#include "StdAfx.h"
#include "ClientHandler.h"
#include "CEFView.h"

#include <sstream>

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
      class RequestClient : public CefWebURLRequestClient
      {
      public:
        RequestClient(CefRefPtr<CefBrowser> browser) : browser_(browser) {}

        virtual void OnStateChange(CefRefPtr<CefWebURLRequest> requester, 
                                   RequestState state)
        {
          REQUIRE_UI_THREAD();      
          if (state == WUR_STATE_DONE) 
          {
            //buffer_ = StringReplace(buffer_, "<", "&lt;");
            //buffer_ = StringReplace(buffer_, ">", "&gt;");
            std::stringstream ss;
            ss << "<html><body>Source:<pre>" << buffer_ << "</pre></body></html>";

            browser_->GetMainFrame()->LoadString(ss.str(),
                "http://tests/weburlrequest");
          }
          
        }
    
        virtual void OnRedirect(CefRefPtr<CefWebURLRequest> requester, 
                                CefRefPtr<CefRequest> request, 
                                CefRefPtr<CefResponse> response)
        {
          REQUIRE_UI_THREAD();
        }
    
        virtual void OnHeadersReceived(CefRefPtr<CefWebURLRequest> requester, 
                                       CefRefPtr<CefResponse> response)
        {
          REQUIRE_UI_THREAD();
        }
    
        virtual void OnProgress(CefRefPtr<CefWebURLRequest> requester, 
                                uint64 bytesSent, uint64 totalBytesToBeSent)
        {
          REQUIRE_UI_THREAD();
        }
    
        virtual void OnData(CefRefPtr<CefWebURLRequest> requester, 
                            const void* data, int dataLength)
        {
          REQUIRE_UI_THREAD();
          buffer_.append(static_cast<const char*>(data), dataLength);
        }
    
        virtual void OnError(CefRefPtr<CefWebURLRequest> requester, 
                             ErrorCode errorCode)
        {
          REQUIRE_UI_THREAD();
        }

      protected:
        CefRefPtr<CefBrowser> browser_;
        std::string buffer_;

        IMPLEMENT_REFCOUNTING(CefWebURLRequestClient);
      };

      CefRefPtr<CefRequest> request(CefRequest::CreateRequest());
      request->SetURL(url);

      CefRefPtr<CefWebURLRequestClient> client(new RequestClient(browser));
      CefRefPtr<CefWebURLRequest> requester(
          CefWebURLRequest::CreateWebURLRequest(request, client));
      return true;
  }
  return false;
}

void ClientHandler::OnResourceResponse(CefRefPtr<CefBrowser> browser,
                                       const CefString& url,
                                       CefRefPtr<CefResponse> response,
                                       CefRefPtr<CefContentFilter>& filter)
{
  CefString text = response->GetStatusText();
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
