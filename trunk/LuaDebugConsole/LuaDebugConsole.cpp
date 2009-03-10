// LuaDebugConsole.cpp : �������̨Ӧ�ó������ڵ㡣
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
		SetEvent( GetStdHandle( STD_INPUT_HANDLE ) );
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

	_tprintf( _T("����LuaDebuger�ɹ���\n") );

	TCHAR szCommand[256];
	DWORD dwSize = 0;
	while(work)
	{
		_tprintf( _T(">>") );
		ReadConsole( GetStdHandle( STD_INPUT_HANDLE ), szCommand, _countof(szCommand), &dwSize, NULL );
		if( dwSize >= _countof(szCommand) )
		{
			break;
		}
		szCommand[dwSize-2] = 0;

		if( commander.command( szCommand ) )
		{
			commander.waitSignal();
		}
		Sleep(1);
	}
	return 0;
}