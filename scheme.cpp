#include "stdafx.h"
#include "include/cef_base.h"
#include "ClientHandler.h"

#include <algorithm>

// Implementation of the schema handler for client:// requests.
class ClientSchemeHandler : public CefSchemeHandler
{
public:
  ClientSchemeHandler() : offset_(0) {}

  UINT BeginThread(LPVOID lParam)
  {
    HANDLE h = CreateEvent(0, false, false, L"DUMMY_EVENT");
    WaitForSingleObject(h, 10000);  
    callback_->HeadersAvailable();
    return 0;
  }

  static UINT InitThread(LPVOID lParam)
  {
    ClientSchemeHandler *handler = static_cast<ClientSchemeHandler*>(lParam);  
    handler->BeginThread(lParam);
    return 0;
  }

  virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
                              CefRefPtr<CefSchemeHandlerCallback> callback)
                              OVERRIDE
  {
    REQUIRE_IO_THREAD();

    AutoLock lock_scope(this);

    bool handled = true;
    mime_type_ = "text/html";
    data_ = "{\"content\": \"replaced by scheme handler\"}";
    callback_ = callback;
    if (handled) 
    {
      // Indicate the headers are available.
      //callback->HeadersAvailable();
      CWinThread *pThread = AfxBeginThread(ClientSchemeHandler::InitThread, static_cast<LPVOID>(this));
      return true;
    }
    return false;
  }

  virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
                                  int64& response_length,
                                  CefString& redirectUrl) OVERRIDE
  {
    REQUIRE_IO_THREAD();

    ASSERT(!data_.empty());

    response->SetMimeType(mime_type_);
    response->SetStatus(200);

    // Set the resulting response length
    response_length = data_.length();
  }

  virtual void Cancel() OVERRIDE
  {
    REQUIRE_IO_THREAD();
  }

  virtual bool ReadResponse(void* data_out,
                            int bytes_to_read,
                            int& bytes_read,
                            CefRefPtr<CefSchemeHandlerCallback> callback)
                            OVERRIDE
  {
    REQUIRE_IO_THREAD();

    bool has_data = false;
    bytes_read = 0;

    AutoLock lock_scope(this);

    if(offset_ < data_.length()) 
    {
      // Copy the next block of data into the buffer.
      int transfer_size =
          (std::min)(bytes_to_read, static_cast<int>(data_.length() - offset_));
      memcpy(data_out, data_.c_str() + offset_, transfer_size);
      offset_ += transfer_size;
      bytes_read = transfer_size;
      has_data = true;
    }

    return has_data;
  }

private:
  std::string data_;
  std::string mime_type_;
  size_t offset_;
  CefRefPtr<CefSchemeHandlerCallback> callback_;

  IMPLEMENT_REFCOUNTING(ClientSchemeHandler);
  IMPLEMENT_LOCKING(ClientSchemeHandler);
};

// Implementation of the factory for for creating schema handlers.
class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory
{
public:
  // Return a new scheme handler instance to handle the request.
  virtual CefRefPtr<CefSchemeHandler> Create(CefRefPtr<CefBrowser> browser,
                                             const CefString& scheme_name,
                                             CefRefPtr<CefRequest> request)
                                             OVERRIDE
  {
    REQUIRE_IO_THREAD();
    return new ClientSchemeHandler();
  }

  IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);
};

void InitSchemeHandler()
{
  CefRegisterSchemeHandlerFactory("http", "server", new ClientSchemeHandlerFactory());
}
