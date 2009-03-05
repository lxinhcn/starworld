#pragma once

class LuaDebugCommander
{
public:
	LuaDebugCommander(void);
	~LuaDebugCommander(void);

	bool initialize( LPCTSTR lpszPipename );
	const char* command( const char* cmd );

private:
	HANDLE m_hPipe;
	TCHAR* szBuffer;
};
