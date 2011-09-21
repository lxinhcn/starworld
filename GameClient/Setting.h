#pragma once
#include "singleton.h"

class hgeFontEx;
struct AppSetting
{
	AppSetting(void);
	~AppSetting(void);

	bool Load( const char* filename );

	hgeFontEx*	pFont;
};

typedef SingletonHolder< AppSetting, CreateStatic > Setting;