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

	bool	waitSignal();
protected:
	void	Signal();
	void	command();
	bool	result();

	LPBYTE	buffer()const{ return szBuffer; }
private:
	static unsigned int __stdcall pipe( void* param );
	HANDLE		m_hPipe;
	HANDLE		m_hThread;
	HANDLE		m_hSignal;
	LPBYTE		szBuffer;
	BOOL		m_bWork;
	run_mode	m_mode;
	ProcessRetCmd	m_RetFunc;

	friend bool Debug_CheckMode( LuaDebugCommander* pDebuger, run_mode m );
};
