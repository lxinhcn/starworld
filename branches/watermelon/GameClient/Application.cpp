#include "StdAfx.h"
#include "Application.h"
#include "b2Rander.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

CApplication::CApplication(void)
: m_hge(NULL)
, m_World(NULL)
{
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
		break;
	case HGEK_F9:
		{
			AllocConsole();
			freopen("CONOUT$","w+t",stdout); 
			freopen("CONIN$","r+t",stdin); 

			Application::Instance()->System_SetState( HGE_DONTSUSPEND, true );
			printf( "UICommander start successful. Enter edit mode.\n" );
		}
		break;
	case HGEK_F8:
		FreeConsole();
		Application::Instance()->System_SetState( HGE_DONTSUSPEND, false );
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
	return false;
}

bool CApplication::RenderFunc()
{
	// Render the scene

	Application::Instance()->Gfx_BeginScene();
	Application::Instance()->Gfx_Clear(0);
	Application::Instance().Render();
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
	m_hge->System_SetState(HGE_HIDEMOUSE, false);

	// m_hge->System_SetState(HGE_DONTSUSPEND, true );

	// 初始化回调函数
	if( !m_hge->System_Initiate())
	{
		return false;
	}

	b2Vec2 g(0.0f, 10.0f);

	m_World = new b2World( g, true );
	if( m_World == NULL )
		return false;

	m_Render = new b2Render();
	m_Render->SetFlags( b2Draw::e_shapeBit );
	m_World->SetDebugDraw( m_Render );
	
	b2Body* ground = NULL;
	{
		b2BodyDef bd;
		ground = m_World->CreateBody(&bd);

		b2EdgeShape shape;
		shape.Set(b2Vec2(-20.0f, 0.0f), b2Vec2(20.0f, 0.0f));

		b2FixtureDef fd;
		fd.shape = &shape;

		ground->CreateFixture(&fd);
	}

	b2Body* attachment;
	// Define attachment
	{
		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(0.0f, 3.0f);
		attachment = m_World->CreateBody(&bd);

		b2PolygonShape shape;
		shape.SetAsBox(0.5f, 2.0f);
		attachment->CreateFixture(&shape, 2.0f);
	}

	b2Body* platform;
	// Define platform
	{
		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(-4.0f, 5.0f);
		platform = m_World->CreateBody(&bd);

		b2PolygonShape shape;
		shape.SetAsBox(0.5f, 4.0f, b2Vec2(4.0f, 0.0f), 0.5f * b2_pi);

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.friction = 0.6f;
		fd.density = 2.0f;
		platform->CreateFixture(&fd);

		b2RevoluteJointDef rjd;
		rjd.Initialize(attachment, platform, b2Vec2(0.0f, 5.0f));
		rjd.maxMotorTorque = 50.0f;
		rjd.enableMotor = true;
		m_World->CreateJoint(&rjd);

		b2PrismaticJointDef pjd;
		pjd.Initialize(ground, platform, b2Vec2(0.0f, 5.0f), b2Vec2(1.0f, 0.0f));

		pjd.maxMotorForce = 1000.0f;
		pjd.enableMotor = true;
		pjd.lowerTranslation = -10.0f;
		pjd.upperTranslation = 10.0f;
		pjd.enableLimit = true;

		(b2PrismaticJoint*)m_World->CreateJoint(&pjd);
	}

	// Create a payload
	{
		b2BodyDef bd;
		bd.type = b2_dynamicBody;
		bd.position.Set(0.0f, 8.0f);
		b2Body* body = m_World->CreateBody(&bd);

		b2PolygonShape shape;
		shape.SetAsBox(0.75f, 0.75f);

		b2FixtureDef fd;
		fd.shape = &shape;
		fd.friction = 0.6f;
		fd.density = 2.0f;

		body->CreateFixture(&fd);
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
	m_World->DrawDebugData();
}