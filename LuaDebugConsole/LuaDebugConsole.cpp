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
		SetEvent( GetStdHandle( STD_INPUT_HANDLE ) );
		work = false;
	}
	return TRUE;
}

void PrintResult( LPBYTE data )
{
	printf( (const char*)data );
}

int _tmain(int argc, _TCHAR* argv[])
{
	SetConsoleCtrlHandler( HandlerRoutine, TRUE );
	
	int ConsoleWidth = 120;
	int ConsoleHeight = 40;
	HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD size;
	size.X = ConsoleWidth;
	size.Y = ConsoleHeight;
	SetConsoleScreenBufferSize(hStd, size);
	SMALL_RECT rc = { 0, 0, ConsoleWidth -1, ConsoleHeight -1 };

	SetConsoleWindowInfo( hStd, TRUE, &rc );

	LuaDebugCommander commander;
	setlocale( LC_ALL, "chs");
	const char*	lpszPipename = "\\\\.\\pipe\\lua\\ui";
	commander.initialize( lpszPipename, PrintResult );

	_tprintf( _T("连接LuaDebuger成功。\n") );
	TCHAR szCommand[256];
	DWORD dwSize = 0;
	commander.command( "cd ..\\Resource\\Scripts\\ui" );
	commander.waitSignal();
	commander.command( "open utility.lua" );
	commander.waitSignal();
	commander.command( "list" );
	commander.waitSignal();
	while(work)
	{
		_tprintf( _T("\n>>") );
		ReadConsole( GetStdHandle( STD_INPUT_HANDLE ), szCommand, _countof(szCommand), &dwSize, NULL );
		if( dwSize >= _countof(szCommand) )
		{
			break;
		}
		while( !_istprint( szCommand[dwSize-1] ) ) --dwSize;
		szCommand[dwSize] = 0;

		if( commander.command( szCommand ) )
		{
			commander.waitSignal();
		}
		Sleep(1);
	}
	return 0;
}
