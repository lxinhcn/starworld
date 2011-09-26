#include "StdAfx.h"
#include "Setting.h"
#include "hgefontex.h"
#include "hgefontmanager.h"
HTEXTURE LoadTexture( _lpcstr lpszTexture )
{
	HGE* hge = hgeCreate(HGE_VERSION);
	if( hge )
	{
		return hge->Texture_Load( lpszTexture );
	}
	hge->Release();

	return 0;
}

AppSetting::AppSetting(void)
{
}

AppSetting::~AppSetting(void)
{
}

bool AppSetting::Initialize( const char* filename )
{
	Class< HTEXTURE >( "HGE::Texture")
		.constructor( LoadTexture );
	;

	Class< hgeSprite >( "HGE::Sprite" )
		.constructor< HTEXTURE, float, float, float, float >()
		.set("setTexture", &hgeSprite::SetTexture)
		.set("setTextureRect", &hgeSprite::SetTextureRect)
		.set("setColor", &hgeSprite::SetColor)
		.set("setHotSpot", &hgeSprite::SetHotSpot)
		;

	Class< hgeAnimation >( "HGE::Animation" )
		.constructor< HTEXTURE, int, float, float, float, float, float >()
		.set( "play", &hgeAnimation::Play )
		.set( "stop", &hgeAnimation::Stop )
		.set( "resume", &hgeAnimation::Resume )
		.set( "setSpeed", &hgeAnimation::SetSpeed )
		.set( "setFrames", &hgeAnimation::SetFrames )
		.set( "setMode", &hgeAnimation::SetMode )
		;

	try
	{
		mScript.doFile( "config.lua" );
		mSetting = mScript.get< LuaObject >( "setting" );
		if( !mSetting.isvalid() )
			return false;

		mTexture = mScript.get< LuaObject >( "texture" );
		if( !mTexture.isvalid() )
			return false;

		mSprites = mScript.get< LuaObject >( "sprites" );
		if( !mSprites.isvalid() )
			return false;

		return true;
	}
	catch( std::runtime_error e )
	{
		XGC_ASSERT_MSGA( true, e.what() );
		return false;
	}

}

_lpcstr AppSetting::getResourcePath()
{
	if( mSetting.isvalid() && mSetting.istable() )
	{
		return mSetting.get< _lpcstr, _lpcstr >( "resource" );
	}

	return "..\\Resource";
}

_lpcstr AppSetting::getFontName()
{
	if( mSetting.isvalid() && mSetting.istable() )
	{
		LuaObject font = mSetting.get< LuaObject >( "font" );
		if( font.isvalid() && font.istable() )
		{
			return font.get< _lpcstr >( "fontname" );
		}
	}

	return "C:\\Windows\\SIMSUN.TTF";
}

int AppSetting::getFontSize()
{
	if( mSetting.isvalid() && mSetting.istable() )
	{
		LuaObject font = mSetting.get< LuaObject >( "font" );
		if( font.isvalid() && font.istable() )
		{
			return font.get< int >( "fontsize" );
		}
	}

	return 16;
}

hgeFontEx* AppSetting::getFont()
{
	if( mFont == NULL )
	{
		mFont = hgeFontHelper( getFontName(), getFontSize() ).getFont();
		if( mFont )
		{
			mFont->SetColor( -1 );
		}
	}

	return mFont;
}