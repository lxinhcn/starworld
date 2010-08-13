#pragma once

class CClientMap;
#include "Canvas.h"

class CApplication
{
	friend struct CreateStatic< CApplication >;
private:
	CApplication(void);
	virtual ~CApplication(void);

public:
	bool Initialize();
	void Run();
	void UnInitialize();

	HGE* operator->(){ return m_hge; }
	HGE* getEngine(){ return m_hge; }

	IMessageQueue* GetMessageQueue(){ return m_pMessageQueue; }
protected:
	bool UpdateLogic( float fDelta );
	void Render();

private:
	static 
		LRESULT __stdcall UIWindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

	static
		LRESULT __stdcall UIDispatchMessage( CONST MSG *lpMsg );

	static 
		WORD __stdcall UIRegisterClass( CONST WNDCLASSA *lpWndClass );

	static
	bool RenderFunc();

	static
	bool FrameFunc();

private:
	typedef LRESULT (CALLBACK *pfnDefWindowProc)( __in HWND hWnd, __in UINT Msg, __in WPARAM wParam, __in LPARAM lParam);
	typedef LRESULT (CALLBACK *pfnDispatchMessage)( __in CONST MSG *lpMsg );
	typedef ATOM (CALLBACK *pfnRegisterClass)( __in CONST WNDCLASSA *lpWndClass );

	friend LRESULT CALLBACK _DefWindowProc( __in HWND hWnd, __in UINT Msg, __in WPARAM wParam, __in LPARAM lParam);

	pfnDefWindowProc	m_pDefWindowProc;
	pfnRegisterClass	m_pRegisterClass;
	pfnDispatchMessage	m_pDispatchMessage;

	HGE			*m_hge;
	CClientMap	*m_pMap;
	IMessageQueue *m_pMessageQueue;
	struct		mouse_cursor;
	static		mouse_cursor m_cursor_table;
};

typedef SingletonHolder< CApplication, CreateStatic > Application;