#pragma once
typedef void (*ProcessRetCmd)( LPBYTE lpszCommand );

class LuaDebugCommander
{
public:
	LuaDebugCommander(void);
	~LuaDebugCommander(void);

	bool	initialize( LPCTSTR lpszPipename, ProcessRetCmd fn );
	bool	command( LPCTSTR cmd );

protected:
	bool	waitSignal();
	void	Signal();
	void	command();
	bool	result();

	LPBYTE	buffer()const{ return szBuffer; }
private:
	static unsigned int __stdcall pipe( void* param );
	HANDLE	m_hPipe;
	HANDLE	m_hThread;
	HANDLE	m_hSignal;
	LPBYTE	szBuffer;
	BOOL	m_bWork;
	ProcessRetCmd	m_RetFunc;
};
