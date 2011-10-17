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
: hge(NULL)
, mZoom(10.0f)
, ptOffset( 0.0f, 0.0f )
, ptMonseDown( 0, 0 )
, mDebug( false )
, mEdit( false )
{
}

CApplication::~CApplication(void)
{
}

bool CApplication::Initialize()
{
	setlocale( LC_ALL, "chs" );

	hge = hgeCreate(HGE_VERSION);

	hge->System_SetState(HGE_LOGFILE, "StarGame.log");
	hge->System_SetState(HGE_FRAMEFUNC, ::FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, ::RenderFunc);
	hge->System_SetState(HGE_TITLE, "StarGame");
	hge->System_SetState(HGE_USESOUND, false);
	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_SCREENWIDTH, SCREEN_WIDTH);
	hge->System_SetState(HGE_SCREENHEIGHT, SCREEN_HEIGHT);
	hge->System_SetState(HGE_SCREENBPP, 32);
	hge->System_SetState(HGE_SCREENBPP, 32);
	hge->System_SetState(HGE_HIDEMOUSE, true);

	// m_hge->System_SetState(HGE_DONTSUSPEND, true );

	// 初始化回调函数
	if( !hge->System_Initiate())
	{
		return false;
	}

	if( !Setting::Instance().Initialize( "config.lua" ) )
	{
		return false;
	}

	_lpcstr resource = Setting::Instance().getResourcePath();

	LuaObject animation = Setting::Instance().getAnimation();
	if( animation.isvalid() && animation.istable() )
		mCursor = animation.get< hgeAnimation* >("cursor");

	mRender = new b2Render();
	mRender->SetFlags( b2Draw::e_shapeBit | b2Draw::e_jointBit );
	
	mLevel = new CGameLevel(mRender, b2Transform( b2Vec2(SCREEN_WIDTH/2,SCREEN_HEIGHT*3/4), b2Rot(0) ));
	if( mLevel )
	{
		if( mLevel->Load( (_astring( resource ) + "level\\level1.lua").c_str() ) == false )
		{
			SAFE_DELETE( mLevel );
			return false;
		}
	}
	return true;
}

bool CApplication::FrameFunc()
{
	float dt=hge->Timer_GetDelta();
	float ft=hge->Timer_GetTime();
	// Process keys

	switch(hge->Input_GetKey())
	{
	case HGEK_F10:
		mDebug = !mDebug;
		break;
	case HGEK_F9:
		{
			if( mEdit )
			{
				FreeConsole();
				hge->System_SetState( HGE_DONTSUSPEND, false );
				printf( "Commander closed. Quit edit mode.\n" );
			}
			else
			{
				AllocConsole();
				freopen("CONOUT$","w+t",stdout); 
				freopen("CONIN$","r+t",stdin); 

				hge->System_SetState( HGE_DONTSUSPEND, true );
				printf( "Commander start successful. Enter edit mode.\n" );
			}
			mEdit = !mEdit;
		}
		break;
	case HGEK_ESCAPE:
		return true;
	}

	hgeInputEvent e;
	while( hge->Input_GetEvent( &e ) )
	{
		switch( e.type )
		{
		case INPUT_MOUSEWHEEL:
			if( e.wheel < 0 )
				mZoom *= 0.9f;
			else if( e.wheel > 0 )
				mZoom *= 1.1f;

			if( mLevel )
				mLevel->OnMouseWheel( e.x, e.y, e.wheel, e.flags, mEdit );
			break;
		case INPUT_MBUTTONDOWN:
			if( e.key == HGEK_RBUTTON )
				ptMonseDown = ptMouseUp = fPoint( e.x, e.y );

			if( e.key == HGEK_LBUTTON )
				mCursor->SetFrame(2);

			if( mLevel )
				mLevel->OnButtonDown( e.x, e.y, e.key, e.flags, mEdit );

			break;
		case INPUT_MBUTTONUP:
			if( e.key == HGEK_LBUTTON )
				mCursor->SetFrame(0);

			if( mLevel )
				mLevel->OnButtonUp( e.x, e.y, e.key, e.flags, mEdit );
			break;
		case INPUT_MOUSEMOVE:
			if( e.flags & MK_RBUTTON )
			{
				ptOffset += fPoint(e.x,e.y) - ptMouseUp;
				ptMouseUp = fPoint(e.x,e.y);
			}

			if( mLevel )
				mLevel->OnMouseMove( e.x, e.y, e.key, e.flags, mEdit );
			break;
		}

	}

	if( mEdit )
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

	UpdateLogic( ft, dt );
	return false;
}

void CApplication::Run()
{
	hge->System_Start();
}

void CApplication::UnInitialize()
{
	hge->System_Shutdown();
	hge->Release();
}

bool CApplication::UpdateLogic( float fTime, float fDelta )
{
	if( mLevel )
	{
		mLevel->UpdateLogic( fTime, fDelta );
	}
	return false;
}

bool CApplication::RenderFunc()
{
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0);

	if( mLevel )
		mLevel->Render();

	if( mCursor )
	{
		float x, y;
		hge->Input_GetMousePos(&x,&y);
		mCursor->Render( x, y );
	}

	wchar_t szInfomation[256];
	_snwprintf( szInfomation, _countof(szInfomation), L"Zoom : %.2f, Offset : %.2f, %.2f, FPS : %d ", mZoom, ptOffset.x, ptOffset.y, hge->Timer_GetFPS() );
	Setting::Instance().getFont()->Print( 10, 10, 0, szInfomation );

	hge->Gfx_EndScene();

	return false;
}