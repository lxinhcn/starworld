#pragma once
#include "singleton.h"
#include "SLB/SLB.hpp"

using namespace SLB;
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

	hgeFontEx*	getFont();
	hgeSprite*	getSprite();
private:
	Script		mScript;
	LuaObject	mSetting;

	hgeFontEx*	mFont;
};

typedef SingletonHolder< AppSetting, CreateStatic > Setting;