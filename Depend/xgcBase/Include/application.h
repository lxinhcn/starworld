#pragma once

#ifdef EXPORT_API
#define EXPORT_LIB __declspec(dllexport)
#else
#define EXPORT_LIB __declspec(dllimport)
#endif

#define MAX_LOADSTRING 100

class EXPORT_LIB CApplication
{
protected:
	CApplication(void);
	virtual ~CApplication(void);

public:
	LRESULT HandlerMessage(HWND, UINT, WPARAM, LPARAM);

	int Initialize(	HINSTANCE hInstance, int nCmdShow, WNDPROC pfnWndProc );
	int Go();

protected:
	// Forward declarations of functions included in this code module:
	ATOM		MyRegisterClass(HINSTANCE hInstance, WNDPROC pfnWndProc);
	BOOL		InitInstance(HINSTANCE, int);

	virtual bool	OnMessage( HWND, UINT, WPARAM, LPARAM ){ return false; }
	virtual bool	OnCreate( HWND ){ return false; }
	virtual bool	OnCommand( INT nID, INT nEvent, LPARAM lParam ){ return false; }
	virtual void	OnDestroy(){ return ; }
	virtual void	OnPaint( PAINTSTRUCT &ps ){ return ; }
	virtual void	OnSize( DWORD dwType, WORD nWidth, WORD nHeight ){ return; }
protected:
	HWND m_hWnd;
	HINSTANCE m_hInst;								// current instance
	HACCEL m_hAccelTable;
	HICON m_hIcon, m_hIconSm;
	HMENU m_hMenu;
	TCHAR m_szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR m_szWindowClass[MAX_LOADSTRING];			// the main window class name

protected:
	long m_WindowWidth;
	long m_WindowHeight;

	BOOL m_bContinue;
};