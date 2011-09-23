#include "StdAfx.h"
#include "Setting.h"
#include "hgefontex.h"

AppSetting::AppSetting(void)
{
}

AppSetting::~AppSetting(void)
{
}

_lpcstr AppSetting::getResourcePath()
{
	if( mSetting.isvalid() && mSetting.istable() )
	{
		return mSetting.get< _lpcstr, _lpcstr >( "resource" );
	}

	return "..\\Resource";
}

bool AppSetting::Load( const char* filename )
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
}