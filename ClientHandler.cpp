#include "StdAfx.h"
#include "ClientHandler.h"


ClientHandler::ClientHandler(void)
{
}


ClientHandler::~ClientHandler(void)
{
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	//REQUIRE_UI_THREAD();

	// Set the view size to match the plugin window size.
	m_Browser = browser;
}
