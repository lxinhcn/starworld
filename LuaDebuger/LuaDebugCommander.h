#pragma once

class LuaDebugCommander
{
public:
	LuaDebugCommander(void);
	~LuaDebugCommander(void);

	bool	initialize( LPCTSTR lpszPipename );
	bool	command( LPCTSTR cmd, DWORD &dwWrite );
	bool	result( LPCTSTR end, DWORD dwWrite );

	LPBYTE	buffer()const{ return szBuffer; }
private:
	HANDLE m_hPipe;
	LPBYTE szBuffer;
};
