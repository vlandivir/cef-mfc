#pragma once
#include "include/cef.h"
class ClientHandler :
	public CefClient
{
public:
	ClientHandler(void);
	~ClientHandler(void);

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(ClientHandler);

};

