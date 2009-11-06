#include "StdAfx.h"
#include "Application.h"
#include "Canvas.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

CApplication::CApplication(void)
: m_pDefWindowProc( NULL )
, m_pRegisterClass( NULL )
{
	init_canvas();
}

CApplication::~CApplication(void)
{
}

ATOM CALLBACK CApplication::UIRegisterClass( __in CONST WNDCLASSA *lpWndClass )
{
	WNDCLASSA WndClass = *lpWndClass;
	Application::Instance().m_pDefWindowProc = lpWndClass->lpfnWndProc;
	WndClass.lpfnWndProc = &CApplication::UIWindowProc;
	return Application::Instance().m_pRegisterClass( &WndClass );
}

LRESULT CALLBACK CApplication::UIWindowProc( __in HWND hWnd, __in UINT Msg, __in WPARAM wParam, __in LPARAM lParam )
{
	//char szlog[1024];
	//_snprintf( szlog, sizeof(szlog), "nMsg = %x\n", Msg );
	//OutputDebugStringA( szlog );
	return UILib::GuiSystem::Instance().HandleMessage( hWnd, Msg, wParam, lParam );
}

LRESULT CALLBACK CApplication::UIDispatchMessage( __in CONST MSG *lpMsg )
{
	TranslateMessage( lpMsg );
	return Application::Instance().m_pDispatchMessage( lpMsg );
}

bool CApplication::FrameFunc()
{
	float dt=Application::Instance()->Timer_GetDelta();
	// Process keys

	switch(Application::Instance()->Input_GetKey())
	{
	case HGEK_F10:
		UILib::SetupDebuger();
		break;
	case HGEK_F9:
		{
			AllocConsole();
			freopen("CONOUT$","w+t",stdout); 
			freopen("CONIN$","r+t",stdin); 

			Application::Instance()->System_SetState( HGE_DONTSUSPEND, true );
			GuiSystem::Instance().SetEditMode( true );
			printf( "UICommander start successful. Enter edit mode.\n" );
		}
		break;
	case HGEK_F8:
		FreeConsole();
		Application::Instance()->System_SetState( HGE_DONTSUSPEND, false );
		GuiSystem::Instance().SetEditMode( false );
		printf( "UICommander closed. Quit edit mode.\n" );
		break;
		//case HGEK_UP:
		//	if(nObjects<MAX_OBJECTS) nObjects+=100; break;
		//case HGEK_DOWN:
		//	if(nObjects>MIN_OBJECTS) nObjects-=100; break;

		//case HGEK_SPACE:
		//	SetBlend(++nBlend); break;
	case HGEK_ESCAPE:
		return true;
	}

	return Application::Instance().UpdateLogic( dt );
}


bool CApplication::RenderFunc()
{
	// Render the scene

	Application::Instance()->Gfx_BeginScene();
	Application::Instance()->Gfx_Clear(0);
	Application::Instance().Render();
	GuiSystem::Instance().Render();
	Application::Instance()->Gfx_EndScene();

	return false;
}

bool CApplication::Initialize()
{
	setlocale( LC_ALL, "chs" );

	m_hge = hgeCreate(HGE_VERSION);

	m_hge->System_SetState(HGE_LOGFILE, "StarGame.log");
	m_hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	m_hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	m_hge->System_SetState(HGE_TITLE, "StarGame");
	m_hge->System_SetState(HGE_USESOUND, false);
	m_hge->System_SetState(HGE_WINDOWED, true);
	m_hge->System_SetState(HGE_SCREENWIDTH, SCREEN_WIDTH);
	m_hge->System_SetState(HGE_SCREENHEIGHT, SCREEN_HEIGHT);
	m_hge->System_SetState(HGE_SCREENBPP, 32);
	m_hge->System_SetState(HGE_SCREENBPP, 32);
	m_hge->System_SetState(HGE_HIDEMOUSE, true);

	// m_hge->System_SetState(HGE_DONTSUSPEND, true );

	GuiSystem::Instance();
	// 挂接窗口消息处理函数
	HMODULE hUser32 = GetModuleHandle( _T("User32.dll") );
	if( hUser32 )
	{
		m_pRegisterClass	= ( pfnRegisterClass )GetProcAddress( hUser32, "RegisterClassA" );
		m_pDispatchMessage	= (pfnDispatchMessage)GetProcAddress( hUser32, "DispatchMessageA" );

		patchimport( GetModuleHandle( _T("hge.dll") ), "User32.dll", NULL, "RegisterClassA", UIRegisterClass );
		patchimport( GetModuleHandle( _T("hge.dll") ), "User32.dll", NULL, "DispatchMessageA", UIDispatchMessage );
	}

	// 初始化回调函数
	if( !m_hge->System_Initiate())
	{
		return false;
	}

	// 初始化UI系统
	GuiSystem::Instance().Initialize( m_hge->System_GetState(HGE_HWND), "..\\Resource\\UI\\" );

	// 设置默认字体
	GuiSystem::Instance().SetDefaultFont( XUI_CreateFontEx( XUI_FontAttribute( "宋体", 18, false, false, false ) ) );

	// 设置光标系统
	GuiSystem::Instance().SetDefaultCursor( new CClientMouse( "Config.xml" ) );

	UICommander::Instance().ProcessCommand( _T("load main.xml") );

	restoreimport( GetModuleHandle( _T("hge") ), "User32.dll", NULL, "RegisterClassA",	m_pRegisterClass );
	return true;
}

void CApplication::Run()
{
	// 系统开始运作
	m_hge->System_Start();
}

void CApplication::UnInitialize()
{
	restoreimport( GetModuleHandle( _T("hge") ), "User32.dll", NULL, "DefWindowProcA",	m_pDefWindowProc );
	restoreimport( GetModuleHandle( _T("hge") ), "User32.dll", NULL, "DispatchMessageA", m_pDispatchMessage );
	// Clean up and shutdown

	delete GuiSystem::Instance().GetMouseCursor();
	delete GuiSystem::Instance().GetDefaultFont();
	GuiSystem::Instance().Unitialize();
	TextureManager::Instance().Clear();
	// m_hge->System_Shutdown();
	m_hge->Release();
}

bool CApplication::UpdateLogic( float fDelta )
{
	GuiSystem::Instance().Update( fDelta );

	if( m_hge->System_GetState( HGE_DONTSUSPEND ) )
	{
		int ch = 0;
		if( _kbhit() )
		{
			switch( ch = _getch() )
			{
			case 13:
				{
					_tchar szCommand[256];
					DWORD dwRead = 0;
					while(true)
					{
						HANDLE hInput = GetStdHandle( STD_INPUT_HANDLE );
						SetConsoleActiveScreenBuffer( hInput );

						_tprintf( _T(">>") );
						ReadConsole( GetStdHandle( STD_INPUT_HANDLE ), szCommand, _countof(szCommand), &dwRead, NULL  );
						if( dwRead <= 2 ) break;
						if( dwRead >= _countof(szCommand) )
						{
							ASSERT_MSG( false, _T("超长的输入串，你太牛了。") );
							break;
						}
						else
						{
							szCommand[dwRead] = 0;
							UICommander::Instance().ProcessCommand( szCommand );
						}
						HANDLE hOutput = GetStdHandle( STD_OUTPUT_HANDLE );
						SetConsoleActiveScreenBuffer( hOutput );
					}
				}
				break;
			}
		}
	}
	return false;
}

void CApplication::Render()
{
	//static byte i = 0;
	//XUI_DrawRect( xgcRect( 1, 1, SCREEN_WIDTH, SCREEN_HEIGHT ), -1, ARGB( 0xff*sin(++i*3.1415/0xff), 0xcc, 0xcc, 0xcc ) );
	// m_hge->Gfx_SetClipping( 0, 0, 100, 100 );
	XUI_DrawRect( xgcRect( 1, 1, SCREEN_WIDTH, SCREEN_HEIGHT ), -1, ARGB( 0xcc, 0xcc, 0xcc, 0xcc ) );
}