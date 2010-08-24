#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include "singleton.h"

#define MAX_LOADSTRING 100

class CApplication
{
friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
friend struct CreateUsingNew< CApplication >;
private:
	CApplication(void);
	virtual ~CApplication(void);

public:
	int Go(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

protected:
	// Forward declarations of functions included in this code module:
	ATOM		MyRegisterClass(HINSTANCE hInstance);
	BOOL		InitInstance(HINSTANCE, int);
	LRESULT 	HandlerMessage(HWND, UINT, WPARAM, LPARAM);

	bool	OnCreate();
	bool	OnDestroy();
	bool	OnCommand( INT nID, INT nEvent, LPARAM lParam );

protected:
	HWND m_hWnd;
	HINSTANCE m_hInst;								// current instance
	HACCEL m_hAccelTable;
	TCHAR m_szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR m_szWindowClass[MAX_LOADSTRING];			// the main window class name

protected:
	long m_WindowWidth;
	long m_WindowHeight;

	BOOL m_bContinue;
};

typedef SingletonHolder< CApplication, CreateUsingNew > Application;