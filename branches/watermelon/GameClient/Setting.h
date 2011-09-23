#pragma once
#include "singleton.h"
#include "SLB/SLB.hpp"

using namespace SLB;
class hgeFontEx;
struct AppSetting
{
	AppSetting(void);
	~AppSetting(void);

	bool Load(const char* filename);

	_lpcstr getResourcePath();
	_lpcstr getFont();
private:
	Script mScript;
	LuaObject mSetting;
};

typedef SingletonHolder< AppSetting, CreateStatic > Setting;