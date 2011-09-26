#include "StdAfx.h"
#include "Application.h"
#include "b2Rander.h"
#include "GameLevel.h"
#include "Setting.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

bool RenderFunc()
{
	return Application::Instance().RenderFunc();
}

bool FrameFunc()
{
	return Application::Instance().FrameFunc();
}

CApplication::CApplication(void)
: m_hge(NULL)
, m_World(NULL)
, m_zoomView(10.0f)
, m_ptOffset( 0.0f, 0.0f )
, m_ptMonseDown( 0, 0 )
, m_bDebug( true )
, m_bEdit( false )
{
}

CApplication::~CApplication(void)
{
}

bool CApplication::FrameFunc()
{
	float dt=m_hge->Timer_GetDelta();
	// Process keys

	switch(m_hge->Input_GetKey())
	{
	case HGEK_F10:
		m_bDebug = !m_bDebug;
		break;
	case HGEK_F9:
		{
			if( m_bEdit )
			{
				FreeConsole();
				m_hge->System_SetState( HGE_DONTSUSPEND, false );
				printf( "UICommander closed. Quit edit mode.\n" );
			}
			else
			{
				AllocConsole();
				freopen("CONOUT$","w+t",stdout); 
				freopen("CONIN$","r+t",stdin); 

				m_hge->System_SetState( HGE_DONTSUSPEND, true );
				printf( "UICommander start successful. Enter edit mode.\n" );
			}
			m_bEdit = !m_bEdit;
		}
		break;
	case HGEK_ESCAPE:
		return true;
	}

	hgeInputEvent e;
	while( m_hge->Input_GetEvent( &e ) )
	{
		switch( e.type )
		{
		case INPUT_MOUSEWHEEL:
			if( e.wheel < 0 )
				m_zoomView *= 0.9f;
			else if( e.wheel > 0 )
				m_zoomView *= 1.1f;
			break;
		case INPUT_MBUTTONDOWN:
			if( e.key == HGEK_RBUTTON )
				m_ptMonseDown = m_ptMouseUp = fPoint( e.x, e.y );
			break;
		case INPUT_MOUSEMOVE:
			if( e.flags & MK_RBUTTON )
			{
				m_ptOffset += fPoint(e.x,e.y) - m_ptMouseUp;
				m_ptMouseUp = fPoint(e.x,e.y);
			}
			break;
		}

	}

	if( m_bEdit )
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
						}
						HANDLE hOutput = GetStdHandle( STD_OUTPUT_HANDLE );
						SetConsoleActiveScreenBuffer( hOutput );
					}
				}
				break;
			}
		}
	}

	UpdateLogic( dt );
	return false;
}

bool CApplication::Initialize()
{
	setlocale( LC_ALL, "chs" );

	m_hge = hgeCreate(HGE_VERSION);

	m_hge->System_SetState(HGE_LOGFILE, "StarGame.log");
	m_hge->System_SetState(HGE_FRAMEFUNC, ::FrameFunc);
	m_hge->System_SetState(HGE_RENDERFUNC, ::RenderFunc);
	m_hge->System_SetState(HGE_TITLE, "StarGame");
	m_hge->System_SetState(HGE_USESOUND, false);
	m_hge->System_SetState(HGE_WINDOWED, true);
	m_hge->System_SetState(HGE_SCREENWIDTH, SCREEN_WIDTH);
	m_hge->System_SetState(HGE_SCREENHEIGHT, SCREEN_HEIGHT);
	m_hge->System_SetState(HGE_SCREENBPP, 32);
	m_hge->System_SetState(HGE_SCREENBPP, 32);
	m_hge->System_SetState(HGE_HIDEMOUSE, false);

	// m_hge->System_SetState(HGE_DONTSUSPEND, true );

	// 初始化回调函数
	if( !m_hge->System_Initiate())
	{
		return false;
	}

	Setting::Instance().Initialize( "config.lua" );
	_lpcstr resource = Setting::Instance().getResourcePath();

	b2Vec2 g(0.0f, -10.0f);

	m_World = new b2World( g, true );
	if( m_World == NULL )
		return false;

	m_Render = new b2Render();
	m_Render->SetFlags( b2Draw::e_shapeBit | b2Draw::e_jointBit );
	m_World->SetDebugDraw( m_Render );
	
	m_pLevel = new CGameLevel(m_World);
	if( m_pLevel )
	{
		m_pLevel->Load( "" );
	}
	return true;
}

void CApplication::Run()
{
	m_hge->System_Start();
}

void CApplication::UnInitialize()
{
	m_hge->System_Shutdown();
	m_hge->Release();
}

bool CApplication::UpdateLogic( float fDelta )
{
	if( m_pLevel )
		m_pLevel->UpdateLogic( fDelta );
	m_World->Step( fDelta, 6, 2 );
	return false;
}

bool CApplication::RenderFunc()
{
	m_hge->Gfx_BeginScene();
	m_hge->Gfx_Clear(0);

	m_hge->Gfx_SetTransform( 0, 0, 0, 0, 0.0f, 0.0f, 0.0f );

	if( m_pLevel )
		m_pLevel->Render();

	wchar_t szInfomation[256];
	_snwprintf( szInfomation, _countof(szInfomation), L"Zoom : %.2f, Offset : %.2f, %.2f, FPS : %d ", m_zoomView, m_ptOffset.x, m_ptOffset.y, m_hge->Timer_GetFPS() );
	Setting::Instance().getFont()->Print( 10, 10, 0, szInfomation );

	if( m_bDebug )
	{
		m_hge->Gfx_SetTransform( 0, 0, SCREEN_WIDTH/2 + m_ptOffset.x, SCREEN_HEIGHT/2 + m_ptOffset.y, b2_pi, m_zoomView, m_zoomView );
		m_hge->Gfx_RenderLine( -SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, 0 );
		m_hge->Gfx_RenderLine( 0, -SCREEN_HEIGHT/2, 0, SCREEN_HEIGHT/2 );
		m_World->DrawDebugData();
	}

	m_hge->Gfx_EndScene();

	return false;
}