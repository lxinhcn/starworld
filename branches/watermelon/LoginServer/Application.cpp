#include "StdAfx.h"
#include "Application.h"
#include "resource.h"
extern LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

CApplication::CApplication(void)
	: m_bContinue( TRUE )
	, m_WindowWidth( 800 )
	, m_WindowHeight( 600 )
{
}

CApplication::~CApplication(void)
{
}

ATOM CApplication::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LOGINSERVER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= m_szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL CApplication::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   m_hInst = hInstance; // Store instance handle in our global variable

   m_hWnd = CreateWindow(
	   m_szWindowClass, 
	   m_szTitle, 
	   WS_OVERLAPPEDWINDOW, 
	   CW_USEDEFAULT, 0, 
	   CW_USEDEFAULT, 0, 
	   NULL, 
	   NULL, 
	   hInstance, 
	   NULL);

   if (!m_hWnd)
   {
		DWORD dwErr = GetLastError();
		return FALSE;
   }

   ShowWindow(m_hWnd, nCmdShow);
   UpdateWindow(m_hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CApplication::HandlerMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_NCCREATE:
		if( !OnCreate() )
			return DefWindowProc( hWnd, message, wParam, lParam );
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		if( !OnCommand( wmId, wmEvent, lParam ) )
			return DefWindowProc( hWnd, message, wParam, lParam );
		break;
	case WM_PAINT:
		hdc = BeginPaint(m_hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(m_hWnd, &ps);
		break;
	case WM_DESTROY:
		m_bContinue = FALSE;
		OnDestroy();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int CApplication::Go(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, m_szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APPLICATION, m_szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	m_hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

	// Main message loop:
	while(m_bContinue)
	{
		if(GetMessage(&msg, NULL, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	return (int) msg.wParam;
}

bool CApplication::OnCreate()
{
	return false;
}

bool CApplication::OnDestroy()
{
	return false;
}

bool CApplication::OnCommand( INT nID, INT nEvent, LPARAM lParam )
{
	return false;
}
