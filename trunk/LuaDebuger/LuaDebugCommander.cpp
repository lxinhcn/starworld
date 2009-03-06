#include "StdAfx.h"
#include "LuaDebugCommander.h"

LuaDebugCommander::LuaDebugCommander(void)
{
	szBuffer = new BYTE[BUFSIZE];
}

LuaDebugCommander::~LuaDebugCommander(void)
{
	delete[] szBuffer;
}

bool LuaDebugCommander::initialize( LPCTSTR lpszPipename )
{
	int retry = 10;
	while( retry )
	{
		m_hPipe = CreateFile( lpszPipename, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
		if( m_hPipe != INVALID_HANDLE_VALUE )
		{
			break;
		}

		DWORD dwRet = GetLastError();

		if( dwRet == ERROR_FILE_NOT_FOUND )
		{
			Sleep(1000);
			--retry;
			continue;
		}
		else if( dwRet != ERROR_PIPE_BUSY) 
		{
			return false;
		}

		if( !WaitNamedPipe(lpszPipename, 10000) )
		{ 
			return false;
		}
	}

	DWORD dwMode = PIPE_READMODE_MESSAGE;
	if( !SetNamedPipeHandleState( m_hPipe, &dwMode, NULL, NULL ) )
	{
		printf("SetNamedPipeHandleState failed"); 
		return false;
	}

	return true;
}

bool LuaDebugCommander::command( LPCTSTR cmd, DWORD &dwWrite )
{
	return WriteFile( m_hPipe, cmd, (DWORD)(_tcslen(cmd)+1)*sizeof(TCHAR), &dwWrite, NULL ) == TRUE;
}

bool LuaDebugCommander::result( LPCTSTR end, DWORD dwWrite )
{
	DWORD dwRead = 0;
	if( ReadFile( m_hPipe, szBuffer, BUFSIZE, &dwRead, NULL ) )
	{
		return !( dwRead == dwWrite && memcmp( szBuffer, end, dwRead ) == 0 );
	}
	return false;
}