#pragma once
//typedef void (*ProcessRetCmd)( LPBYTE lpszCommand );
#include "DebugerInterface.h"
class LuaDebugCommander
{
public:
	LuaDebugCommander(void);
	~LuaDebugCommander(void);

	bool	initialize( const char* lpszPipename, ProcessRetCmd fn );
	bool	command( const char* cmd );

	buffer*	getBuffer();
	void	releaseBuffer( buffer* buf );
	bool	waitSignal( DWORD dwTimer = INFINITE );
protected:
	void	Signal();
	void	command();
	buffer*	result();

private:
	static unsigned int __stdcall pipe( void* param );

	HANDLE		m_hPipe;
	HANDLE		m_hThread;
	HANDLE		m_hSignal;
	buffer		*m_buffer_head;
	buffer		*m_buffer_tail;

	bool		m_bWork;
	run_mode	m_mode;
	ProcessRetCmd	m_RetFunc;

	friend bool Debug_CheckMode( LuaDebugCommander* pDebuger, run_mode m );
};
