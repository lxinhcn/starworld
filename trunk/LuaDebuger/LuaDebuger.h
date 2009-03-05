#pragma once

#include <string>
#include <map>
#include <list>
#include <set>

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

private:
	struct Impl;
	Impl*	m_pImpl;
};
