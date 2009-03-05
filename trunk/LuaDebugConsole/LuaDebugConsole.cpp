// LuaDebugConsole.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "LuaDebugCommander.h"

int _tmain(int argc, _TCHAR* argv[])
{
	LuaDebugCommander commander;

	LPTSTR	lpszPipename = TEXT("\\\\.\\pipe\\lua_debuger"); 

	commander.initialize( lpszPipename );
	return 0;
}

