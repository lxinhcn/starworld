#include "StdAfx.h"
#include "LuaDebuger.h"

struct LuaDebuger::Impl
{
	Impl()
		: in( INVALID_HANDLE_VALUE )
		, out( INVALID_HANDLE_VALUE )
	{

	}

	typedef std::set< int >	line_set;
	typedef std::map< std::string, line_set >	break_map;
	typedef std::set< std::string >	path_set;

	path_set		paths;
	break_map		breakpoints;

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
		m_pImpl->paths.insert( path );
	}
}

void LuaDebuger::set_breakpoint( const char* name, int line )
{
	if( name != NULL && line >= 0 )
	{
		m_pImpl->breakpoints[name].insert( line );
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

bool LuaDebuger::is_break( const char* name, int line )
{
	Impl::break_map::const_iterator citer = m_pImpl->breakpoints.find( name );
	if( citer != m_pImpl->breakpoints.end() )
	{
		const Impl::line_set &lineset = citer->second;
		Impl::line_set::const_iterator cline = lineset.find( line );
		if( cline != lineset.end() )
		{
			return true;
		}
	}
	return false;
}

void Debug(lua_State *L, lua_Debug *ar)
{
	lua_Debug debug;
	lua_getglobal( L, "__debuger" );
	LuaDebuger* pDebuger = (LuaDebuger*)lua_touserdata( L, -1 );

	char szFilename[_MAX_FNAME];
	char szExt[_MAX_EXT];
	std::stringstream filename;
	lua_getstack(L, 0, &debug );
	if( lua_getinfo(L, "Sln", &debug) )
	{
		_splitpath_s( debug.source, NULL, 0, NULL, 0, szFilename, _countof(szFilename), szExt, _countof(szExt) );
		{
			filename << szFilename << szExt;
			pDebuger->is_break( filename.str().c_str(), debug.currentline );
		}
	}
}

bool LuaDebuger::initialize( lua_State* L )
{
	lua_sethook( L, Debug, LUA_MASKCALL|LUA_MASKLINE|LUA_MASKRET, 0 );
	lua_pushlightuserdata( L, this );
	lua_setglobal( L, "__debuger" );
	return true;
}
