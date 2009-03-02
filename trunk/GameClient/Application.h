#pragma once

class CClientMap;
#include "Canvas.h"

class CApplication
{
	friend struct Loki::CreateStatic< CApplication >;
private:
	CApplication(void);
	virtual ~CApplication(void);

public:
	bool Initialize();
	void Run();
	void UnInitialize();

	HGE* operator->(){ return m_hge; }
	HGE* getEngine(){ return m_hge; }
protected:
	bool UpdateLogic( float fDelta );
	void Render();

private:
	static 
		LRESULT CALLBACK UIWindowProc( __in HWND hWnd, __in UINT Msg, __in WPARAM wParam, __in LPARAM lParam );

	static
		LRESULT CALLBACK UIDispatchMessage( __in CONST MSG *lpMsg );

	static 
		ATOM CALLBACK UIRegisterClass( __in CONST WNDCLASSA *lpWndClass );

	static
	bool RenderFunc();

	static
	bool FrameFunc();

private:
	typedef LRESULT (CALLBACK *pfnDefWindowProc)( __in HWND hWnd, __in UINT Msg, __in WPARAM wParam, __in LPARAM lParam);
	typedef LRESULT (CALLBACK *pfnDispatchMessage)( __in CONST MSG *lpMsg );
	typedef ATOM (CALLBACK *pfnRegisterClass)( __in CONST WNDCLASSA *lpWndClass );

	pfnDefWindowProc	m_pDefWindowProc;
	pfnRegisterClass	m_pRegisterClass;
	pfnDispatchMessage	m_pDispatchMessage;

	HGE			*m_hge;
	CClientMap	*m_pMap;

	CConsole	m_console;
};

typedef Loki::SingletonHolder< CApplication, Loki::CreateStatic > Application;