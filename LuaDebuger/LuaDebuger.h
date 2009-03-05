#pragma once

#include "LuaDebuger.h"
struct lua_State;
class LuaDebuger
{
public:
	LuaDebuger();
	~LuaDebuger();

	void set_path( const char* path );
	void set_breakpoint( const char* name, int line );
	void set_output_handle( HANDLE out );
	void set_input_handle( HANDLE in );
	bool initialize( lua_State* L );

protected:
	friend void Debug( lua_State *L, lua_Debug* ar );
	bool is_break( const char* name, int line );
private:
	struct Impl;
	Impl*	m_pImpl;
};
