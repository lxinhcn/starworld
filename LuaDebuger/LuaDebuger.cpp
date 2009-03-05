#include "StdAfx.h"
#include "LuaDebuger.h"

struct LuaDebuger::Impl
{
	Impl()
		: in( INVALID_HANDLE_VALUE )
		, out( INVALID_HANDLE_VALUE )
	{

	}

	std::list< std::string >	path_list;
	std::map< std::string, std::set< int > >	breakpoint_map;

	HANDLE in, out;
};

LuaDebuger::LuaDebuger()
: m_pImpl( new Impl )
{

}

LuaDebuger::~LuaDebuger()
{
	delete m_pImpl;
	m_pImpl = NULL;
}

void LuaDebuger::set_path( const char* path )
{
	if( path != NULL )
	{
		m_pImpl->path_list.push_back( path );
	}
}

void LuaDebuger::set_breakpoint( const char* name, int line )
{
	if( name != NULL && line >= 0 )
	{
		m_pImpl->breakpoint_map[name].insert( line );
	}
}

void LuaDebuger::set_output_handle( HANDLE out )
{
	m_pImpl->out = out;
}

void LuaDebuger::set_input_handle( HANDLE in )
{
	m_pImpl->in = in;
}

void Debug(lua_State *L, lua_Debug *ar)
{

}

bool LuaDebuger::initialize( lua_State* L )
{
	lua_sethook( L, Debug, LUA_MASKLINE, 0 );
	return true;
}
