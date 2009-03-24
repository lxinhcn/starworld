#include "StdAfx.h"
#include "LuaDebuger.h"
#include <io.h>
#include <vector>
#include <deque>
#include <algorithm>

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
		std::set< size_t >		breakline;
	};

	typedef std::set< size_t >				line_set;
	typedef std::map< _string, breakinfo >	break_map;

	// 运行模式
	enum run_mode { run = 0, stop, step, stepin, stepout, mode_count };

	struct variant
	{
		variant( LPCTSTR n, LPCTSTR v, int i )
			: name( n )		// 变量名
			, value( v )	// 变量值
			, idx( i )		// 变量索引
		{

		}

		int		idx;
		_string	name;
		_string value;
	};

	typedef std::list< variant >	variant_list;
	struct stackframe
	{
		int			currentline;
		_string		filename;
		_string		funcname;
		_string		what;
		variant_list	variants;
	};

	struct luastack :public std::deque< stackframe* >
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
		: pThis( p )
		, command( c )
	{
		debug_signal = CreateEvent( NULL, FALSE, FALSE, NULL );
	}

	~ThreadParam()
	{
		if( bWork )
		{
			bWork = false;
			HANDLE p = CreateFile( TEXT("\\\\.\\pipe\\lua_debuger"), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );
			DisconnectNamedPipe( p );
			CloseHandle( p );
		}
		CloseHandle( pipe );
		WaitForSingleObject( thread, INFINITE );
		CloseHandle( thread );
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
			insert( std::make_pair( _T("clr"),		&LuaDebuger::cmd_clearpoint ) );

			insert( std::make_pair( _T("stack"),	&LuaDebuger::cmd_stack ) );
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

	HANDLE	thread;
	HANDLE	debug_signal;
	HANDLE	pipe;
	bool	bWork;
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
	SAFE_DELETE( m_thread_param );
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

	std::transform( filename.begin(), filename.end(), filename.begin(), tolower );
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
		pDebuger->makestack(L,ar);
		pDebuger->waitSignal(L);
		pDebuger->clearstack();
		break;
	case LuaDebuger::Impl::stepin:
		if( pDebuger->m_pImpl->stop_level <= pDebuger->m_pImpl->call_level )
		{
			pDebuger->makestack(L,ar);
			pDebuger->waitSignal(L);
			pDebuger->clearstack();
		}
		break;
	case LuaDebuger::Impl::stepout:
		if( pDebuger->m_pImpl->stop_level > pDebuger->m_pImpl->call_level )
		{
			pDebuger->makestack(L,ar);
			pDebuger->waitSignal(L);
			pDebuger->clearstack();
		}
		break;
	case LuaDebuger::Impl::run:
		lua_getinfo( L, "S", ar );
		if( ar->source[0] == '@' && pDebuger->judgeBreak( ar->source+1, ar->currentline ) )
		{
			pDebuger->makestack(L,ar);
			pDebuger->waitSignal(L);
			pDebuger->clearstack();
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

void LuaDebuger::makestack( lua_State *L, lua_Debug *ar )
{
	m_pImpl->lstack.L = L;
	m_pImpl->stop_level = m_pImpl->call_level;

	lua_Debug d;
	for ( int level = 0; lua_getstack(L, level, &d ); level++)
	{
		lua_getinfo( L, "Slnu", &d );
		Impl::stackframe* sf = new Impl::stackframe();
		sf->currentline	= d.currentline;
		sf->funcname	= d.name?XA2T( d.name ):_T("");
		sf->filename	= d.source[0] == '@'?XA2T(d.source+1):_T("");
		std::transform( sf->filename.begin(), sf->filename.end(), sf->filename.begin(), tolower );
		sf->what		= d.what?XA2T( d.what ):_T("");
		const char* varname		= NULL;
		std::string varvalue;
		for( int index = 1; varname = lua_getlocal( L, &d, index ); ++index )
		{
			int top = lua_gettop(L);
			int t = lua_type( L, top );
			switch( t )
			{
			case LUA_TBOOLEAN:
				varvalue = lua_toboolean( L, top )?"true":"false";
				break;
			case LUA_TNUMBER:
				varvalue = lua_tostring( L, top );
				break;
			case LUA_TSTRING:
				varvalue = std::string("\"") + lua_tostring( L, top ) + "\"";
				break;
			case LUA_TTABLE:
				varvalue = "table";
				break;
			case LUA_TUSERDATA:
				varvalue = "userdata";
				break;
			case LUA_TLIGHTUSERDATA:
				varvalue = "lightuserdata";
				break;
			case LUA_TNIL:
				varvalue = "nil";
				break;
			case LUA_TFUNCTION:
				varvalue = "function";
				break;
			case LUA_TTHREAD:
				varvalue = "thread";
				break;
			}
			sf->variants.push_back( Impl::variant( XA2T( varname ), XA2T( varvalue ), index ) );
			lua_pop(L,1);
		}
		m_pImpl->lstack.push_back( sf );
	}

	// 输出当前行
	Impl::stackframe* sf = m_pImpl->lstack.front();
	if( sf )
	{
		m_pImpl->begin = sf->currentline;
		m_pImpl->strFilename = sf->filename;
		CCriticalLock _l( m_pImpl->breakmap_lock );

		Impl::break_map::const_iterator c = m_pImpl->breakpoints.find( sf->filename );
		if( c != m_pImpl->breakpoints.end() )
		{
			if( sf->currentline >= 0 && sf->currentline < (int)c->second.file.size() )
			{
				output( _T("\n%04d > %s"), sf->currentline, c->second.file[sf->currentline].c_str() );
			}
		}
	}
}

void LuaDebuger::clearstack()
{
	// clear all old stackframe;
	for( Impl::luastack::iterator i = m_pImpl->lstack.begin(); i !=  m_pImpl->lstack.end(); ++i )
	{
		delete *i;
	}
	m_pImpl->lstack.clear();
}

unsigned int __stdcall LuaDebuger::guard( void *param )
{
	LuaDebuger::ThreadParam* p = ( LuaDebuger::ThreadParam* )param;

	BYTE	buffer[BUFSIZE];
	DWORD	dwRead, dwWrite;
	LPTSTR	lpszPipename = TEXT("\\\\.\\pipe\\lua_debuger"); 

	p->bWork = true;
	// The main loop creates an instance of the named pipe and 
	// then waits for a client to connect to it. When the client 
	// connects, a thread is created to handle communications 
	// with that client, and the loop is repeated. 

	while( p->bWork )
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


		BOOL bConnected = ConnectNamedPipe(p->pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

		while( bConnected )
		{
			if( PeekNamedPipe( p->pipe, NULL, 0, NULL, NULL, &dwRead ) && dwRead > 0 )
			{
				BOOL ret = ReadFile( p->pipe, buffer, sizeof(buffer), &dwRead, NULL);
				if( ret )
				{
					p->_call( (LPCTSTR)buffer );
					WriteFile( p->pipe, "~!@#$%^&*()?", 12, &dwWrite, NULL );
				}
			}
			else 
			{
				DWORD dwCode = GetLastError();
				if( dwCode == ERROR_BROKEN_PIPE ||
					dwCode == ERROR_INVALID_HANDLE ||
					dwCode == ERROR_PIPE_CONNECTED
					)
				{
					break;
				}
				else
				{
					Sleep( 1 );
				}
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

	m_thread_param->thread = (HANDLE)_beginthreadex( NULL, 0, guard, m_thread_param, 0, NULL );

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

void LuaDebuger::cmd_clearpoint( LPCTSTR lpszParam )
{
	if( _tcsicmp( lpszParam, _T("all") ) == 0 )
	{
		CCriticalLock _l( m_pImpl->breakmap_lock);
		Impl::break_map::iterator iter = m_pImpl->breakpoints.find( m_pImpl->strFilename );
		if( iter != m_pImpl->breakpoints.end() )
		{
			iter->second.breakline.clear();
		}
	}
	else
	{
		int		line;
		_stscanf( lpszParam, _T("%d"), &line );

		CCriticalLock _l( m_pImpl->breakmap_lock);
		Impl::break_map::iterator iter = m_pImpl->breakpoints.find( m_pImpl->strFilename );
		if( iter != m_pImpl->breakpoints.end() )
		{
			iter->second.breakline.erase( line );
		}
	}
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
	size_t idx = 0;
	size_t n = _stscanf( lpszParam, _T("%d"), &idx );
	switch( n )
	{
	case EOF:
	case 0:
		idx = m_pImpl->lstack.size();
		output( _T("%4s|%15s|%8s|%04s|%.30s\n"), _T("idx"), _T("function name"), _T("what"), _T("line"), _T("file name") );
		output( _T("----|---------------|--------|----|----\n"), _T('-') );
		for( Impl::luastack::reverse_iterator i = m_pImpl->lstack.rbegin(); i !=  m_pImpl->lstack.rend(); ++i )
		{
			Impl::stackframe* sf = *i;
			output( _T("%04d|%15s|%8s|%04d|...%.30s\n"), --idx, sf->funcname.c_str(), sf->what.c_str(), sf->currentline, sf->filename.size() > 30?sf->filename.c_str() + sf->filename.size() - 20:sf->filename.c_str() );
		}
		break;
	case 1:
		if( idx >= 0 && idx < m_pImpl->lstack.size() )
		{
			Impl::stackframe* sf = m_pImpl->lstack[idx];
			Impl::variant_list::iterator i = sf->variants.begin();
			n = 0;
			while( i != sf->variants.end() )
			{
				Impl::variant& v = (*i);
				output( _T("%2d | %20s = %s\n"), n++, v.name.c_str(), v.value.c_str() );
				++i;
			}
		}
	}
}

void LuaDebuger::cmd_open( LPCTSTR lpszParam )
{
	if( _tcslen( lpszParam ) == 0 )
	{
		CCriticalLock _l( m_pImpl->breakmap_lock );
		Impl::break_map::const_iterator c = m_pImpl->breakpoints.begin();
		int i = 1;
		while( c != m_pImpl->breakpoints.end() )
		{
			TCHAR mark = m_pImpl->strFilename == c->first?_T('*'):_T(' ');
			output( _T("%c%02d | %s\n"), mark, i, c->first.c_str() );
			++c;
			++i;
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

					info.file.push_back( _T("\n") );
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
		LPCTSTR p = lpszParam;
		while( _istdigit( *p ) ) ++p;
		if( *p == 0 && p != lpszParam )
		{
			CCriticalLock _l( m_pImpl->breakmap_lock );
			Impl::break_map::const_iterator c = m_pImpl->breakpoints.begin();
			int i = 1;
			while( c != m_pImpl->breakpoints.end() )
			{
				if( i == _ttoi( lpszParam ) )
				{
					m_pImpl->strFilename = c->first;
					m_pImpl->begin = 0;
					break;
				}
				++c;
				++i;
			}
		}
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
		case EOF:
		case 0:
			break;
		case 1:
			// 输入了显示行数
			end	= __min( begin + 20, c->second.file.size() );
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
			bool b = ( c->second.breakline.find( i ) != c->second.breakline.end() );
			output( _T("%c %04u> %s"), b?_T('@'):_T(' '), i, l.c_str() );
		}
		m_pImpl->begin = __min( end + 1, c->second.file.size() );
	}
}