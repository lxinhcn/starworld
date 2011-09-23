#include "StdAfx.h"
#include "Setting.h"
#include "hgefontex.h"
#include "hgefontmanager.h"

AppSetting::AppSetting(void)
{
}

AppSetting::~AppSetting(void)
{
}

bool AppSetting::Initialize( const char* filename )
{
	try
	{
		mScript.doFile( "config.lua" );
		mSetting = mScript.get< LuaObject >( "setting" );

		return true;
	}
	catch( std::runtime_error e )
	{
		return false;
	}

	Class< HTEXTURE >( "HGE::Texture" )

	Class< hgeSprite >( "HGE::Sprite" )
		.constructor< HTEXTURE, float, float, float, float >()
		.set("setTexture", &hgeSprite::SetTexture)
		.set("setTextureRect", &hgeSprite::SetTextureRect)
		.set("setColor", &hgeSprite::SetColor)
		.set("setHotSpot", &hgeSprite::SetHotSpot)
	;
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