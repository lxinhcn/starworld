#include "StdAfx.h"
#include "LuaDebuger.h"

struct LuaDebuger::Impl
{
	Impl()
		: in( INVALID_HANDLE_VALUE )
		, out( INVALID_HANDLE_VALUE )
		, call_level( 0 )
		, stop_level( 0 )
		, runmode( run )
	{
		debug_signal = CreateEvent( NULL, TRUE, TRUE, NULL );
	}

	~Impl()
	{
		CloseHandle( debug_signal );
	}

	typedef std::set< int >	line_set;
	typedef std::map< std::string, line_set >	break_map;
	typedef std::set< std::string >	path_set;

	// 运行模式
	enum run_mode { run, stop, step, stepover, stepout };

	struct variant
	{
		std::string		name;
		unsigned char	type;
		int				stackindex;
	};

	struct stackframe
	{
		int						currentline;
		std::string				filename;
		std::list< variant >	variants;
		std::list< variant >	upvalues;
	};

	typedef std::stack< stackframe >	luastack;

	path_set	paths;
	break_map	breakpoints;

	HANDLE		in, out;
	HANDLE		debug_signal;
	run_mode	runmode;

	int			call_level;	// 当前堆栈深度
	int			stop_level;	// 暂停时的堆栈深度

	luastack	lstack;		// lua 栈
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

void LuaDebuger::set_breakpoint( const char* name, int line )
{
	if( name != NULL && line >= 0 )
	{
		m_pImpl->breakpoints[name].insert( line );
	}
}

void LuaDebuger::set_stream_handle( HANDLE in, HANDLE out )
{
	m_pImpl->in		= in;
	m_pImpl->out	= out;
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

bool LuaDebuger::waitSignal( lua_State *L )
{
	DWORD dwRet = WaitForSingleObject( m_pImpl->debug_signal, INFINITE );
	if( dwRet != WAIT_OBJECT_0 )
	{
		luaL_error( L, "debug signal error." );
		return false;
	}
	return true;
}

static void line_hook( LuaDebuger* pDebuger, lua_State *L, lua_Debug *ar )
{
	switch( pDebuger->m_pImpl->runmode )
	{
	case LuaDebuger::Impl::stop:
		pDebuger->waitSignal(L);
		break;
	case LuaDebuger::Impl::step:
		if( pDebuger->m_pImpl->stop_level >= pDebuger->m_pImpl->call_level )
		{
			pDebuger->waitSignal(L);
		}
		break;
	case LuaDebuger::Impl::stepout:
		if( pDebuger->m_pImpl->stop_level > pDebuger->m_pImpl->call_level )
		{
			pDebuger->waitSignal(L);
		}
		break;
	case LuaDebuger::Impl::run:
		if( ar->source[0] == '@' && pDebuger->is_break( ar->source+1, ar->currentline ) )
		{
			lua_getinfo( L, "nu", ar );
			pDebuger->waitSignal(L);
		}
		break;
	}
}

static void call_hook( LuaDebuger* pDebuger, lua_State *L, lua_Debug *ar )
{
	++ pDebuger->m_pImpl->call_level;
}

static void ret_hook( LuaDebuger* pDebuger, lua_State *L, lua_Debug *ar )
{
	-- pDebuger->m_pImpl->call_level;
}

static void count_hook( LuaDebuger* pDebuger, lua_State *L, lua_Debug *ar )
{

}

static void Debug(lua_State *L, lua_Debug *ar)
{
	lua_getglobal( L, "__debuger" );
	LuaDebuger* pDebuger = (LuaDebuger*)lua_touserdata( L, -1 );

	switch( ar->event )
	{
	case LUA_HOOKCOUNT:
		count_hook( pDebuger, L, ar);
		break;

	case LUA_HOOKCALL:
		call_hook( pDebuger, L, ar);
		break;

	case LUA_HOOKRET:
	case LUA_HOOKTAILRET:	//verify
		ret_hook( pDebuger, L, ar);
		break;

	case LUA_HOOKLINE:
		lua_getinfo( L, "Sl", ar );
		line_hook( pDebuger, L, ar );
		break;
	}

	//char szFilename[_MAX_FNAME];
	//char szExt[_MAX_EXT];
	//std::stringstream filename;
	//lua_getstack(L, 0, &debug );
	//if( lua_getinfo(L, "Sln", &debug) )
	//{
	//	_splitpath_s( debug.source, NULL, 0, NULL, 0, szFilename, _countof(szFilename), szExt, _countof(szExt) );
	//	{
	//		filename << szFilename << szExt;
	//		pDebuger->is_break( filename.str().c_str(), debug.currentline );
	//	}
	//}
}

bool LuaDebuger::initialize( lua_State* L, HANDLE in, HANDLE out )
{
	set_stream_handle( in, out );

	lua_sethook( L, Debug, LUA_MASKCALL|LUA_MASKLINE|LUA_MASKRET|LUA_MASKCOUNT, 1 );
	lua_pushlightuserdata( L, this );
	lua_setglobal( L, "__debuger" );

	return true;
}
