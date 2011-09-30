#include "StdAfx.h"
#include "GameLevel.h"
#include "Setting.h"

CGameLevel::CGameLevel( b2World* world )
: mWorld( world )
, hge( hgeCreate(HGE_VERSION) )
, mBackground( NULL )
{
}

CGameLevel::~CGameLevel(void)
{
	hge->Release();
}

bool CGameLevel::Load( const char* filename )
{
	try
	{
		mScript.doFile( filename );

		mTexture = mScript.get< LuaObject >( "texture" );
		if( !mTexture.isvalid() )
			return false;

		mSprites = mScript.get< LuaObject >( "sprites" );
		if( !mSprites.isvalid() )
			return false;

		mAnimation = mScript.get< LuaObject >( "animation" );
		if( !mAnimation.isvalid() )
			return false;

		mBackground = mSprites.get< hgeSprite* >( "background" );

		LuaCall< void (b2World*) > CreateLevel( &mScript, "CreateLevel" );
		CreateLevel( mWorld );
	}
	catch( std::runtime_error e )
	{
		XGC_ASSERT_MSGA( false, e.what() );
		return false;
	}

	return true;
}

bool CGameLevel::UpdateLogic( float fDelta )
{
	return true;
}

void CGameLevel::Render()
{
	if( mBackground )
		mBackground->RenderStretch( 0.0f, 0.0f, (float)hge->System_GetState( HGE_SCREENWIDTH ), (float)hge->System_GetState( HGE_SCREENHEIGHT )  );
}

void CGameLevel::Destroy()
{

}
