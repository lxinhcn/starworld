// LuaDebugConsole.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "LuaDebugCommander.h"
#include <stdlib.h>
#include <locale>
bool work = true;

BOOL WINAPI HandlerRoutine( DWORD dwCtrlType )
{
	if( dwCtrlType == CTRL_CLOSE_EVENT )
	{
		DWORD dwWrite;
		WriteConsole( GetStdHandle( STD_INPUT_HANDLE ), "", 0, &dwWrite, NULL );
		work = false;
	}
	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleCtrlHandler( HandlerRoutine, TRUE );
	LuaDebugCommander commander;
	setlocale( LC_ALL, "chs");
	LPTSTR	lpszPipename = TEXT("\\\\.\\pipe\\lua_debuger"); 
	commander.initialize( lpszPipename );

	_tprintf( _T("连接LuaDebuger成功。\n") );

	TCHAR szCommand[256];
	DWORD dwSize = 0;
	while(work)
	{
		_tprintf( _T(">>") );
		ReadConsole( GetStdHandle( STD_INPUT_HANDLE ), szCommand, _countof(szCommand), &dwSize, NULL  );
		if( dwSize >= _countof(szCommand) )
		{
			break;
		}
		szCommand[dwSize] = 0;

		if( commander.command( szCommand, dwSize ) )
		{
			while( commander.result( szCommand, dwSize ) )
			{
				_tprintf( (LPCTSTR)commander.buffer() );
			}
		}
		Sleep(1);
	}
	return 0;
}
