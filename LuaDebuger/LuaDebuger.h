#pragma once

#include "LuaDebuger.h"
struct lua_State;
class LuaDebuger
{
public:
	LuaDebuger();
	~LuaDebuger();

	bool initialize( lua_State* L, LPCTSTR lpszPipename );
	void terminal();

protected:
	friend void Debug( lua_State *L, lua_Debug* ar );
	friend void line_hook( LuaDebuger *pDebuger, lua_State *L, lua_Debug *ar );
	friend void call_hook( LuaDebuger *pDebuger, lua_State *L, lua_Debug *ar );
	friend void ret_hook( LuaDebuger *pDebuger, lua_State *L, lua_Debug *ar );
	friend void count_hook( LuaDebuger *pDebuger, lua_State *L, lua_Debug *ar );

	bool command( LPCTSTR lpszCmd );

	void bp( LPCTSTR name, int line );
	void run( int mode );

	void cmd_breakpoint( LPCTSTR lpszParam );
	void cmd_clearpoint( LPCTSTR lpszParam );
	void cmd_step( LPCTSTR lpszParam );
	void cmd_stepout( LPCTSTR lpszParam );
	void cmd_stepin( LPCTSTR lpszParam );
	void cmd_run( LPCTSTR lpszParam );

	void cmd_stack( LPCTSTR lpszParam );
	void cmd_open( LPCTSTR lpszParam );
	void cmd_cd( LPCTSTR lpszParam );
	void cmd_dir( LPCTSTR lpszParam );
	void cmd_list( LPCTSTR lpszParam );

	void makestack( lua_State *L, lua_Debug *ar );
	void clearstack();

	bool judgeBreak( const char* name, int line );
	bool waitSignal( lua_State *L );
	void Signal();
	int	 output( LPCTSTR szFmt, ... );

	static unsigned int __stdcall guard( void *param );
private:
	struct Impl;
	struct ThreadParam;
	Impl*			m_pImpl;
	ThreadParam*	m_thread_param;
};
