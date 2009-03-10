#include "StdAfx.h"
#include "LuaDebuger.h"
#include "critical_lock.h"
#include <io.h>
#include <vector>
struct LuaDebuger::Impl
{
	Impl()
		: call_level( 0 )
		, stop_level( 0 )
		, runmode( run )
		, begin( 0 )
	{
	}

	~Impl()
	{
	}

	struct breakinfo
	{
		std::vector< _string >	file;
		std::set< int >			breakline;
	};

	typedef std::set< int >					line_set;
	typedef std::map< _string, breakinfo >	break_map;

	// 运行模式
	enum run_mode { run = 0, stop, step, stepin, stepout, mode_count };

	struct variant
	{
		variant( const char* n, int si )
			: name( n )
			, stackindex( si )
		{

		}

		std::string	name;
		std::string value;
		int			stackindex;
		std::list< variant >*	variants;
	};

	struct stackframe
	{
		int						currentline;
		std::string				filename;
		std::string				funcname;
		std::string				what;
		std::list< variant >	variants;
	};

	struct luastack :public std::vector< stackframe* >
	{
		lua_State	*L;
		int			current;	// 当前查看的堆栈位置
	};

	break_map	breakpoints;
	run_mode	runmode;

	int			call_level;	// 当前堆栈深度
	int			stop_level;	// 暂停时的堆栈深度
	size_t		begin;		// 当前文件游标

	luastack	lstack;			// lua 栈
	_string		strFilename;	// 当前文件
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
		debug_signal = CreateEvent( NULL, FALSE, TRUE, NULL );
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

	typedef void (LuaDebuger::* instruct)( LPCTSTR lpszParams );
	typedef struct instruct_map : public std::map< _string, instruct >
	{
		instruct_map()
		{
			// insert( std::make_pair( _T(""), &LuaDebuger::cmd_ ) );
			insert( std::make_pair( _T("bp"),		&LuaDebuger::cmd_breakpoint ) );
			insert( std::make_pair( _T("step"),		&LuaDebuger::cmd_step ) );
			insert( std::make_pair( _T("stepin"),	&LuaDebuger::cmd_stepin ) );
			insert( std::make_pair( _T("stepout"),	&LuaDebuger::cmd_stepout ) );
			insert( std::make_pair( _T("run"),		&LuaDebuger::cmd_run ) );
			insert( std::make_pair( _T("open"),		&LuaDebuger::cmd_open ) );
			insert( std::make_pair( _T("dir"),		&LuaDebuger::cmd_dir ) );
			insert( std::make_pair( _T("cd"),		&LuaDebuger::cmd_cd ) );
			insert( std::make_pair( _T("list"),		&LuaDebuger::cmd_list ) );
		}
	};

	HANDLE			in, out;
	HANDLE			debug_signal;
	HANDLE			pipe;
	static instruct_map	instructs;	// 指令映射表

private:
	LuaDebuger*	pThis;
	bool (LuaDebuger::* command)( LPCTSTR lpszCmd )	;
};

LuaDebuger::ThreadParam::instruct_map	LuaDebuger::ThreadParam::instructs;	// 指令映射表

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
		Impl::break_map::iterator iter = m_pImpl->breakpoints.find( name );
		if( iter != m_pImpl->breakpoints.end() )
		{
			iter->second.breakline.insert( line );
			output( _T("break point set at %s, line %d\n"), name, line );
		}
		else
		{
			output( _T("file %s are not open!"), name );
		}
	}
}

void LuaDebuger::run( int mode )
{
	if( mode >= 0 && mode < Impl::mode_count )
	{
		m_pImpl->runmode = Impl::run_mode( mode );
		Signal();
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
		const Impl::line_set &lineset = citer->second.breakline;
		Impl::line_set::const_iterator cline = lineset.find( line );
		if( cline != lineset.end() )
		{
			output( _T("break at %s %d\n"), citer->first.c_str(), *cline );
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

void LuaDebuger::Signal()
{
	SetEvent( m_thread_param->debug_signal );
}

static void line_hook( LuaDebuger* pDebuger, lua_State *L, lua_Debug *ar )
{
	switch( pDebuger->m_pImpl->runmode )
	{
	case LuaDebuger::Impl::stop:
		luaL_error( L, "debug stop." );
		break;
	case LuaDebuger::Impl::step:
		pDebuger->make(L,ar);
		pDebuger->waitSignal(L);
		break;
	case LuaDebuger::Impl::stepin:
		if( pDebuger->m_pImpl->stop_level <= pDebuger->m_pImpl->call_level )
		{
			pDebuger->make(L,ar);
			pDebuger->waitSignal(L);
		}
		break;
	case LuaDebuger::Impl::stepout:
		if( pDebuger->m_pImpl->stop_level > pDebuger->m_pImpl->call_level )
		{
			pDebuger->make(L,ar);
			pDebuger->waitSignal(L);
		}
		break;
	case LuaDebuger::Impl::run:
		lua_getinfo( L, "S", ar );
		if( ar->source[0] == '@' && pDebuger->judgeBreak( ar->source+1, ar->currentline ) )
		{
			pDebuger->make(L,ar);
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
		line_hook( pDebuger, L, ar );
		break;
	}
}

void LuaDebuger::make( lua_State *L, lua_Debug *ar )
{
	// clear all old stackframe;
	for( Impl::luastack::iterator i = m_pImpl->lstack.begin(); i !=  m_pImpl->lstack.end(); ++i )
	{
		delete *i;
	}
	m_pImpl->lstack.clear();

	m_pImpl->begin = ar->currentline;

	m_pImpl->lstack.L = L;
	m_pImpl->stop_level = m_pImpl->call_level;

	for ( int level = 0; lua_getstack(L, level, ar ); level++)
	{
		lua_getinfo( L, "Slnu", ar );
		Impl::stackframe* sf = new Impl::stackframe();
		sf->currentline	= ar->currentline;
		sf->funcname	= ar->name;
		char* source	= _strdup(ar->source+1);
		sf->filename	= ar->source[0] == '@'?_strlwr(source):"";
		free( source );
		sf->what		= ar->what;

		const char* varname = NULL;
		for( int index = 0; varname = lua_getlocal( L, ar, index ); ++index )
		{
			sf->variants.push_back( Impl::variant( varname, lua_gettop( L ) ) );
		}

		m_pImpl->lstack.push_back( sf );
	}
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
				WriteFile( p->pipe, "~!@#$%^&*()?", 12, &dwWrite, NULL );
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
	LPCTSTR pCmd = lpszCmd;
	while( *pCmd && _istalnum( *pCmd ) ) ++pCmd;

	_string strCmd( lpszCmd, pCmd - lpszCmd );
	while( *pCmd == _T(' ') ) ++pCmd;

	ThreadParam::instruct_map::iterator iter = m_thread_param->instructs.find( strCmd );
	if( iter != m_thread_param->instructs.end() )
	{
		(this->* iter->second)( pCmd );
	}
	else
	{
		output( _T("command not exist.\n") );
	}
	return true;
}

int	 LuaDebuger::output( LPCTSTR szFmt, ... )
{
	TCHAR tszLog[4096];
	va_list args;
	va_start(args, szFmt);

	size_t nSize = _countof( tszLog );
	int size = _vsntprintf( tszLog, nSize, szFmt, args );
	va_end(args);
	if( size < 0 )	return 0;
	tszLog[nSize-1] = 0;

	WriteFile( m_thread_param->pipe, tszLog, size * sizeof(TCHAR), (DWORD*)&size, NULL );
	return size;
}

void LuaDebuger::cmd_breakpoint( LPCTSTR lpszParam )
{
	int		line;
	_stscanf( lpszParam, _T("%d"), &line );
	bp( m_pImpl->strFilename.c_str(), line );
}

void LuaDebuger::cmd_step( LPCTSTR lpszParam )
{
	run( Impl::step );
}

void LuaDebuger::cmd_stepout( LPCTSTR lpszParam )
{
	run( Impl::stepout );
}

void LuaDebuger::cmd_stepin( LPCTSTR lpszParam )
{
	run( Impl::stepin );
}

void LuaDebuger::cmd_run( LPCTSTR lpszParam )
{
	run( Impl::run );
}

void LuaDebuger::cmd_stack( LPCTSTR lpszParam )
{
}

void LuaDebuger::cmd_open( LPCTSTR lpszParam )
{
	if( _tcslen( lpszParam ) == 0 )
	{
		CCriticalLock _l( m_pImpl->breakmap_lock );
		Impl::break_map::const_iterator c = m_pImpl->breakpoints.begin();
		while( c != m_pImpl->breakpoints.end() )
		{
			output( _T("%s\n"), c->first.c_str() );
			++c;
		}
	}
	else if( _taccess( lpszParam, 0 ) != -1 )
	{
		TCHAR szFull[_MAX_PATH];
		if( _tfullpath( szFull, lpszParam, _MAX_PATH ) )
		{
			_tcslwr( szFull );
			Impl::break_map::const_iterator c = m_pImpl->breakpoints.find( szFull );
			if( c == m_pImpl->breakpoints.end() )
			{
				FILE* fp = _tfopen( szFull, _T("r") );
				// 读取文件
				if( fp != NULL )
				{
					TCHAR szLine[1024*4];
					
					Impl::breakinfo &info = m_pImpl->breakpoints[szFull];
					info.file.clear();
					info.breakline.clear();

					while( !feof(fp) )
					{
						_fgetts( szLine, _countof(szLine), fp );
						info.file.push_back( szLine );
					}

					fclose( fp );
					output( _T("file %s opened!\n"), szFull );
				}
			}
		}
		m_pImpl->strFilename = szFull;
		output( _T("now set current file is %s\n"), szFull );
	}
	else
	{
		output( _T("file %s are not exist\n"), lpszParam );
	}
}

void LuaDebuger::cmd_cd( LPCTSTR lpszParam )
{
	TCHAR szFull[_MAX_PATH];
	if( _tfullpath( szFull, lpszParam, _MAX_PATH ) != NULL )
	{
		_tchdir( szFull );
		output( _T("change director at :%s\n"), szFull );
	}
}

void LuaDebuger::cmd_dir( LPCTSTR lpszParam )
{
	struct _tfinddata_t c_file;
	intptr_t hFile;

	// Find first .c file in current directory 
	if( (hFile = _tfindfirst( _T("*.*"), &c_file )) != -1L )
	{
		output( _T("Listing of all files\n\n") );
		output( _T("RDO HID SYS ARC  DATA%25c SIZE      FILE\n"), _T(' ') );
		output( _T("--- --- --- ---  ----%25c ----      ----\n"), _T(' ') );
		do {
			TCHAR buffer[30];
			output( ( c_file.attrib & _A_RDONLY ) ? _T(" Y  ") : _T(" N  ") );
			output( ( c_file.attrib & _A_SYSTEM ) ? _T(" Y  ") : _T(" N  ") );
			output( ( c_file.attrib & _A_HIDDEN ) ? _T(" Y  ") : _T(" N  ") );
			output( ( c_file.attrib & _A_ARCH )   ? _T(" Y  ") : _T(" N  ") );
			_tctime_s( buffer, _countof(buffer), &c_file.time_write );
			output( _T(" %.24s  %9ld %s \n"), buffer, c_file.size, c_file.name );
		} while( _tfindnext( hFile, &c_file ) == 0 );
		_findclose( hFile );
	}
}

void LuaDebuger::cmd_list( LPCTSTR lpszParam )
{
	Impl::break_map::const_iterator c = m_pImpl->breakpoints.find( m_pImpl->strFilename );
	if( c != m_pImpl->breakpoints.end() )
	{
		size_t begin	= m_pImpl->begin;
		size_t end		= __min( begin + 20 , c->second.file.size() );
		size_t n = _stscanf( lpszParam, _T("%d %d"), &begin, &end );

		switch( n )
		{
		case 0:
			break;
		case 1:
			// 输入了显示行数
			end		= __min( m_pImpl->begin + begin, c->second.file.size() );
			begin	= m_pImpl->begin;
			break;
		case 2:
			if( end < begin ) 
			{
				output( _T("error for input begin > end.\n") );
				return;
			}
			break;
		}

		for( size_t i = begin; i < end; ++i )
		{
			const _string& l = c->second.file[i];
			output( _T("%04u> %s"), i, l.c_str() );
		}
		m_pImpl->begin = __min( end + 1, c->second.file.size() );
	}
}