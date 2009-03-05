#include "StdAfx.h"
#include "LuaDebugCommander.h"

LuaDebugCommander::LuaDebugCommander(void)
{
	szBuffer = new TCHAR( BUFSIZE );
}

LuaDebugCommander::~LuaDebugCommander(void)
{
}

bool LuaDebugCommander::initialize( LPCTSTR lpszPipename )
{
	LPTSTR lpvMessage=TEXT("Default message from client"); 

	m_hPipe = CreateFile( lpszPipename, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
	return m_hPipe != INVALID_HANDLE_VALUE;
}

const char* LuaDebugCommander::command( const char* cmd )
{
	DWORD dwWrite, dwRead;
	std::string c( cmd );
	if( WriteFile( m_hPipe, cmd, (DWORD)c.length(), &dwWrite, NULL ) )
	{
		if( ReadFile( m_hPipe, szBuffer, sizeof(szBuffer), &dwRead, NULL ) )
		{
			return (const char*)szBuffer;
		}
	}
	return NULL;
}
