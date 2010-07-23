// LoginServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <conio.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include "Application.h"
#include "NetworkService.h"
#include "singleton.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return Application::Instance().HandlerMessage(hWnd, nMsg, wParam, lParam);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	db::initial( db::mssql );

	net::CreateNetwork( _T("asio") );
	net::InitNetwork();

	CNetworkService *pService = new CNetworkService( 18890 );
	Application::Instance().Go( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
	SAFE_DELETE( pService );
	net::FiniNetwork();
	net::DestroyNetwork();
	db::final();
}