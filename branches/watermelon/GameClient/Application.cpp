#include "StdAfx.h"
#include "Application.h"
#include "Canvas.h"
#include "NetworkService.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

CApplication::CApplication(void)
: m_pMessageQueue( NULL )
{
	init_canvas();
}

CApplication::~CApplication(void)
{
}

bool CApplication::FrameFunc()
{
	float dt=Application::Instance()->Timer_GetDelta();
	// Process keys

	switch(Application::Instance()->Input_GetKey())
	{
	case HGEK_F10:
		XGC::ui::SetupDebuger();
		break;
	case HGEK_F9:
		{
			AllocConsole();
			freopen("CONOUT$","w+t",stdout); 
			freopen("CONIN$","r+t",stdin); 

			Application::Instance()->System_SetState( HGE_DONTSUSPEND, true );
			XUI::Instance().SetEditMode( true );
			printf( "UICommander start successful. Enter edit mode.\n" );
		}
		break;
	case HGEK_F8:
		FreeConsole();
		Application::Instance()->System_SetState( HGE_DONTSUSPEND, false );
		XUI::Instance().SetEditMode( false );
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

	Application::Instance().UpdateLogic( dt );
	XUI::Instance().Update( Application::Instance()->Timer_GetDelta() );
	return false;
}

bool CApplication::RenderFunc()
{
	// Render the scene

	Application::Instance()->Gfx_BeginScene();
	Application::Instance()->Gfx_Clear(0);
	Application::Instance().Render();
	XUI::Instance().Render();
	Application::Instance()->Gfx_EndScene();

	return false;
}

bool CApplication::Initialize()
{
	setlocale( LC_ALL, "chs" );

	m_hge = hgeCreate(HGE_VERSION);
	CreateNetwork(_T("asio"));
	if( InitNetwork() == false )
		return false;

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

	// 初始化回调函数
	if( !m_hge->System_Initiate())
	{
		return false;
	}

	// 初始化UI系统
	XUI::Instance().Initialize( m_hge->System_GetState(HGE_HWND), "..\\Resource\\UI\\" );

	// 设置默认字体
	XUI::Instance().SetDefaultFont( XUI_CreateFont( "宋体", 18, XUI_ARGB(255, 255, 255, 255), false, false, false ) );

	UICommander::Instance().Load( "main.xml" );

	return true;
}

void CApplication::Run()
{
	// 系统开始运作
	CNetworkService Client;
	Client.Start();
	ConnectServer( "127.0.0.1", 18890, &m_pMessageQueue, 0 );
	m_hge->System_Start();
	Client.Stop();
}

void CApplication::UnInitialize()
{
	delete XUI::Instance().GetDefaultFont();
	XUI::Instance().Unitialize();
	TextureManager::Instance().Clear();
	m_hge->System_Shutdown();
	m_hge->Release();
	m_pMessageQueue->Release();
	FiniNetwork();
	DestroyNetwork();
}

bool CApplication::UpdateLogic( float fDelta )
{
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
	XUI_DrawRect( iRect( 1, 1, SCREEN_WIDTH, SCREEN_HEIGHT ), -1, ARGB( 0xcc, 0xcc, 0xcc, 0xcc ) );
}