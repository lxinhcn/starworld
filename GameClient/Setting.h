#pragma once
#include "singleton.h"
class hgeFontEx;
class hgeSprite;

struct AppSetting
{
	AppSetting(void);
	~AppSetting(void);

	bool Initialize(const char* filename);

	_lpcstr getResourcePath();
	_lpcstr getFontName();
	int		getFontSize();

	Script&		getScript(){ return mScript; }
	LuaObject	getSetting(){ return mSetting; }
	LuaObject	getTexture(){ return mTexture; }
	LuaObject	getSprites(){ return mSprites; }
	hgeFontEx*	getFont();
	hgeSprite*	getSprite();
private:
	Script		mScript;
	LuaObject	mSetting;
	LuaObject	mTexture;
	LuaObject	mSprites;

	hgeFontEx*	mFont;
};

typedef SingletonHolder< AppSetting, CreateStatic > Setting;