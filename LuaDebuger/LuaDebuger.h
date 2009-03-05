#pragma once

#include "LuaDebuger.h"
struct lua_State;
class LuaDebuger
{
public:
	LuaDebuger();
	~LuaDebuger();

	bool initialize( lua_State* L, HANDLE in, HANDLE out );

	void set_breakpoint( const char* name, int line );

	void step();
	void stepover();
	void stepout();
	void pause();
	void run();
	void stop();

protected:
	friend void Debug( lua_State *L, lua_Debug* ar );
	friend void line_hook( LuaDebuger *pDebuger, lua_State *L, lua_Debug *ar );
	friend void call_hook( LuaDebuger *pDebuger, lua_State *L, lua_Debug *ar );
	friend void ret_hook( LuaDebuger *pDebuger, lua_State *L, lua_Debug *ar );
	friend void count_hook( LuaDebuger *pDebuger, lua_State *L, lua_Debug *ar );

	// …Ë÷√ ‰»Î ‰≥ˆæ‰±˙
	void set_stream_handle( HANDLE in, HANDLE out );

	bool is_break( const char* name, int line );
	bool waitSignal( lua_State *L );
private:
	struct Impl;
	Impl*	m_pImpl;
};
