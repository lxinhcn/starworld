#pragma once

class LuaDebugCommander
{
public:
	LuaDebugCommander(void);
	~LuaDebugCommander(void);

	bool	initialize( LPCTSTR lpszPipename );
	bool	command( LPCTSTR cmd );
	bool	waitSignal();
	void	Signal();

protected:
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
};
