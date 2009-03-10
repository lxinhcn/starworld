#include "StdAfx.h"
#include "LuaDebugCommander.h"

LuaDebugCommander::LuaDebugCommander(void)
: m_hPipe( INVALID_HANDLE_VALUE )
, m_hThread( INVALID_HANDLE_VALUE )
, m_bWork( TRUE )
{
	szBuffer = new BYTE[BUFSIZE];
}

LuaDebugCommander::~LuaDebugCommander(void)
{
	m_bWork = FALSE;
	WaitForSingleObject( m_hThread, INFINITE );
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

	m_hSignal = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hThread = (HANDLE)_beginthreadex( NULL, 0, LuaDebugCommander::pipe, this, 0, NULL );
	return true;
}

bool LuaDebugCommander::waitSignal()
{
	return WaitForSingleObject( m_hSignal, INFINITE ) == WAIT_OBJECT_0;
}

void LuaDebugCommander::Signal()
{
	SetEvent( m_hSignal );
}

bool LuaDebugCommander::command( LPCTSTR cmd )
{
	DWORD dwWrite = 0;
	return WriteFile( m_hPipe, cmd, (DWORD)(_tcslen(cmd)+1)*sizeof(TCHAR), &dwWrite, NULL ) == TRUE;
}

bool LuaDebugCommander::result()
{
	DWORD dwRead = 0;
	if( PeekNamedPipe( m_hPipe, NULL, 0, NULL, NULL, &dwRead ) && dwRead > 0 )
	{
		if( ReadFile( m_hPipe, szBuffer, BUFSIZE, &dwRead, NULL ) )
		{
			szBuffer[dwRead] = 0;
			szBuffer[dwRead+1] = 0;

			if( memcmp( "~!@#$%^&*()?", szBuffer, dwRead ) == 0 )
			{
				Signal();
				return false;
			}
			return true;
		}
	}

	return false;
}

unsigned int __stdcall LuaDebugCommander::pipe( void* param )
{
	LuaDebugCommander* pCommander = (LuaDebugCommander*)param;
	if( pCommander )
	{
		while( pCommander->m_bWork )
		{
			if( pCommander->result() )
			{
				_tprintf( (LPCTSTR)pCommander->buffer() );
			}
			else
			{
				Sleep( 1 );
			}
		}
	}
	return 0;
}