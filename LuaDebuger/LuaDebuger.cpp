#include "StdAfx.h"
#include "LuaDebuger.h"
#include "critical_lock.h"

struct LuaDebuger::Impl
{
	Impl()
		: call_level( 0 )
		, stop_level( 0 )
		, runmode( run )
	{
	}

	~Impl()
	{
	}

	typedef std::set< int >	line_set;
	typedef std::map< _string, line_set >	break_map;

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

	break_map	breakpoints;
	run_mode	runmode;

	int			call_level;	// 当前堆栈深度
	int			stop_level;	// 暂停时的堆栈深度

	luastack	lstack;		// lua 栈

	CCritical	breakmap_lock;
};

struct LuaDebuger::ThreadParam
{
	ThreadParam( LuaDebuger* p, bool (LuaDebuger::* c)( LPCTSTR lpszCmd ) )
		: in( INVALID_HANDLE_VALUE )
		, out( INVALID_HANDLE_VALUE )
		, pThis( p )
		, command( c )
	{
		debug_signal = CreateEvent( NULL, TRUE, TRUE, NULL );
	}

	~ThreadParam()
	{
		WaitForSingleObject( debug_signal, INFINITE );
		CloseHandle( debug_signal );
	}

	bool _call( LPCTSTR lpszCmd )
	{
		return (pThis->*command)( lpszCmd );
	}

	typedef bool (LuaDebuger::* instruct)( LPCTSTR lpszParams );
	typedef struct _instruct_map : public std::map< _string, instruct >
	{
		_instruct_map()
		{
			// insert( std::make_pair( _T(""), &LuaDebuger::cmd_ ) );
			insert( std::make_pair( _T("bp"),		&LuaDebuger::cmd_breakpoint ) );
			insert( std::make_pair( _T("step"),		&LuaDebuger::cmd_step ) );
		}
	}instruct_map;

	HANDLE			in, out;
	HANDLE			debug_signal;
	HANDLE			pipe;
	static instruct_map	instructs;	// 指令映射表

private:
	LuaDebuger*	pThis;
	bool (LuaDebuger::* command)( LPCTSTR lpszCmd )	;
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

void LuaDebuger::bp( LPCTSTR name, int line )
{
	if( name != NULL && line >= 0 )
	{
		CCriticalLock _l( m_pImpl->breakmap_lock);
		m_pImpl->breakpoints[name].insert( line );
	}
}

bool LuaDebuger::judgeBreak( const char* name, int line )
{
	CCriticalLock _l( m_pImpl->breakmap_lock);
#if defined( _UNICODE )
	LPTSTR path = (TCHAR*)_alloca( (_MAX_DIR + _MAX_FNAME + _MAX_EXT)*sizeof(wchar_t) );
	mbstowcs( path, name, (_MAX_DIR + _MAX_FNAME + _MAX_EXT) );
	_string filename = path;
#else
	_string filename = name;
#endif

	Impl::break_map::const_iterator citer = m_pImpl->breakpoints.find( filename );
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
	if( m_thread_param == NULL ) 
	{
		return false;
	}

	DWORD dwRet = WaitForSingleObject( m_thread_param->debug_signal, INFINITE );
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
		if( ar->source[0] == '@' && pDebuger->judgeBreak( ar->source+1, ar->currentline ) )
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

unsigned int __stdcall LuaDebuger::guard( void *param )
{
	LuaDebuger::ThreadParam* p = ( LuaDebuger::ThreadParam* )param;

	BYTE	buffer[BUFSIZE];
	DWORD	dwRead, dwWrite;
	BOOL	fConnected; 
	LPTSTR	lpszPipename = TEXT("\\\\.\\pipe\\lua_debuger"); 

	// The main loop creates an instance of the named pipe and 
	// then waits for a client to connect to it. When the client 
	// connects, a thread is created to handle communications 
	// with that client, and the loop is repeated. 

	for (;;) 
	{ 
		p->pipe = CreateNamedPipe( 
			lpszPipename,             // pipe name 
			PIPE_ACCESS_DUPLEX,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			1,						  // max. instances  
			BUFSIZE,                  // output buffer size 
			BUFSIZE,                  // input buffer size 
			NMPWAIT_USE_DEFAULT_WAIT, // client time-out 
			NULL);                    // default security attribute 

		if (p->pipe == INVALID_HANDLE_VALUE) 
		{
			printf("CreatePipe failed"); 
			return 0;
		}

		// Wait for the client to connect; if it succeeds, 
		// the function returns a nonzero value. If the function
		// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

		fConnected = ConnectNamedPipe(p->pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

		while(fConnected)
		{ 
			BOOL ret = ReadFile( p->pipe, buffer, sizeof(buffer), &dwRead, NULL);
			if( ret )
			{
				p->_call( (LPCTSTR)buffer );
				WriteFile( p->pipe, buffer, dwRead, &dwWrite, NULL );
			}
			else if( GetLastError() == ERROR_BROKEN_PIPE )
			{
				break;
			}
		}
	}

	return 0;
}

bool LuaDebuger::initialize( lua_State* L )
{
	m_thread_param = new ThreadParam( this, &LuaDebuger::command );
	
	lua_sethook( L, Debug, LUA_MASKCALL|LUA_MASKLINE|LUA_MASKRET|LUA_MASKCOUNT, 1 );
	lua_pushlightuserdata( L, this );
	lua_setglobal( L, "__debuger" );

	m_thread_param->debug_signal = (HANDLE)_beginthreadex( NULL, 0, guard, m_thread_param, 0, NULL );

	// _tsystem( _T("LuaDebugConsole.exe") );
	return true;
}

bool LuaDebuger::command( LPCTSTR lpszCmd )
{
	return true;
}

bool LuaDebuger::cmd_breakpoint( LPCTSTR lpszParam )
{
	TCHAR	szFilename[_MAX_PATH+_MAX_FNAME+_MAX_EXT];
	int		line;
	_stscanf( lpszParam, _T("%255s %d"), &szFilename, &line );
	bp( szFilename, line );
	return true;
}

bool LuaDebuger::cmd_step( LPCTSTR lpszParam )
{
	return true;
}
