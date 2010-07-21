// LoginServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <conio.h>
#include <stdlib.h>
#include <process.h>
#include <time.h>

#include "NetworkService.h"
#include "singleton.h"
#include "allocator.h"

using namespace XGC::common;

class CLoginServer
{
	friend struct CreateStatic< CLoginServer >;
private:
	CLoginServer();
	~CLoginServer();

public:
	//--------------------------------------------------------//
	//	created:	3:12:2009   13:36
	//	filename: 	LoginServer
	//	author:		Albert.xu
	//
	//	purpose:	开始应用程序
	//--------------------------------------------------------//
	int Start();

	//--------------------------------------------------------//
	//	created:	8:12:2009   14:59
	//	filename: 	LoginServer
	//	author:		Albert.xu
	//
	//	purpose:	停止服务器
	//--------------------------------------------------------//
	void Stop();

protected:
	//--------------------------------------------------------//
	//	created:	8:12:2009   15:16
	//	filename: 	LoginServer
	//	author:		Albert.xu
	//
	//	purpose:	设置日志窗口
	//--------------------------------------------------------//
	HANDLE SetupLoggerWindow( int x, int y );

	//--------------------------------------------------------//
	//	created:	8:12:2009   15:20
	//	filename: 	LoginServer
	//	author:		Albert.xu
	//
	//	purpose:	开始服务
	//--------------------------------------------------------//
	int Run( HANDLE hCommand );

private:
	bool m_bWork;
};
typedef SingletonHolder< CLoginServer, CreateStatic > LoginServer;

CLoginServer::CLoginServer()
: m_bWork( true )
{

}

CLoginServer::~CLoginServer()
{
}

//--------------------------------------------------------//
//	created:	3:12:2009   13:36
//	filename: 	LoginServer
//	author:		Albert.xu
//
//	purpose:	开始应用程序
//--------------------------------------------------------//
int CLoginServer::Start()
{
	InitNetwork();
	//--------------------------------------------------------//
	//	初始化数据库
	//--------------------------------------------------------//
	db::initial( db::mssql );

	return Run( SetupLoggerWindow( 120, 40 ) );
}

void CLoginServer::Stop()
{
	m_bWork = false;
}

//--------------------------------------------------------//
//	created:	8:12:2009   15:16
//	filename: 	LoginServer
//	author:		Albert.xu
//
//	purpose:	重设窗口
//--------------------------------------------------------//
HANDLE CLoginServer::SetupLoggerWindow( int x, int y )
{
	COORD c = { x, y*4 };
	SMALL_RECT rc = { 0, 0, x-1, y-1 };

	SetConsoleScreenBufferSize( GetStdHandle( STD_OUTPUT_HANDLE ), c );
	SetConsoleWindowInfo( GetStdHandle( STD_OUTPUT_HANDLE ), TRUE, & rc );

	HANDLE hCommand = CreateConsoleScreenBuffer( GENERIC_READ|GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );

	SetConsoleScreenBufferSize( hCommand, c );
	SetConsoleWindowInfo( hCommand, TRUE, &rc );

	SetConsoleActiveScreenBuffer( hCommand );

	CConsoleAdapter *pAdapter = new CConsoleAdapter( hCommand, _T("ConsoleLog") );
	CLogger::GetInstance( LOG )->AddAdapter( pAdapter, true );

	return hCommand;
}

int CLoginServer::Run( HANDLE hCommand )
{
	_tchar szCommand[1024];
	DWORD dwRead = 0;
	// CNetworkService network( 19944 );

	while( m_bWork )
	{
		if( _kbhit() )
		{
			int ch = _gettch();
			switch( ch )
			{
			case 0x1b:
				Stop();
				break;
			case 0x0d:
				SetConsoleActiveScreenBuffer( GetStdHandle(STD_OUTPUT_HANDLE) );
				WriteConsole( GetStdHandle(STD_OUTPUT_HANDLE), _T(">"), 1, &dwRead, NULL );
				if( ReadConsole( GetStdHandle(STD_INPUT_HANDLE), szCommand, sizeof(szCommand), &dwRead, NULL ) )
				{
				}
				SetConsoleActiveScreenBuffer( hCommand );
				break;
			}
			_puttch( ch );
		}
		else
		{
			Sleep(1);
		}
	}

	CLoggerAdapter *pAdapter = CLogger::GetInstance( LOG )->RemoveAdapter( _T("ConsoleLog") );
	delete pAdapter;

	FiniNetwork();

	db::final();
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------//
//	created:	3:12:2009   13:35
//	filename: 	LoginServer
//	author:		Albert.xu
//
//	purpose:	退出应用程序
//--------------------------------------------------------//
BOOL WINAPI Stop( DWORD dwCtrlType )
{
	// LoginServer::Instance().Stop();
	for(;;);
	return TRUE;
}

void dotest();
//--------------------------------------------------------//
//	created:	8:12:2009   15:24
//	filename: 	LoginServer
//	author:		Albert.xu
//
//	purpose:	程序入口
//--------------------------------------------------------//
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale( LC_ALL, "chs" );
	SetConsoleCtrlHandler( NULL, FALSE );
	SetConsoleCtrlHandler( Stop, TRUE );

	// dotest();

	CreateNetwork( _T("asio") );
	LoginServer::Instance().Start();
	DestroyNetwork();
}

void test_allocator()
{
	std::vector< int, XGC::common::allocator<int> >	_array_xgc;
	std::vector< int > _array_std;
	srand((unsigned int)time(NULL));
	for( int n = 0; n < 10; ++n )
	{
		clock_t t1 = clock();
		for( int i = 0; i < 1000000; ++i )
		{
			_array_xgc.push_back( rand() );
		}
		// _array_xgc.clear();
		clock_t t2 = clock();

		for( int i = 0; i < 1000000; ++i )
		{
			_array_std.push_back( rand() );
		}
		// _array_std.clear();
		clock_t t3 = clock();

		printf( "%d, %d\n", t2 - t1, t3 - t2 );
	}

	std::vector< char* >	vec_point1, vec_point2;
	for( int n = 0; n < 10; ++n )
	{
		clock_t t1 = clock();
		for( int i = 0; i < 1000*1000; ++i )
		{
			vec_point1.push_back( (char*)AllocatorSingleton<>::Instance().Allocate( rand()%255, false ) );
			if( i % (1000*100) == 0 )
			{
				for( std::vector< char* >::iterator it = vec_point1.begin(); it != vec_point1.end(); ++it )
				{
					AllocatorSingleton<>::Instance().Deallocate( *it );
				}
				vec_point1.clear();
				printf( "." );
			}
		}
		clock_t t2 = clock();
		printf("\t%d\n", t2-t1);

		for( int i = 0; i < 1000*1000; ++i )
		{
			vec_point2.push_back( new char[rand()%255] );
			if( i % (1000*100) == 0 )
			{
				for( std::vector< char* >::iterator it = vec_point2.begin(); it != vec_point2.end(); ++it )
				{
					delete[] *it;
				}
				vec_point2.clear();
				printf( "." );
			}
		}
		clock_t t3 = clock();

		printf( "\t%d\n", t3 - t2 );
	}
	getch();
}

void testdb()
{
	db::initial( db::mssql );
	db::connection conn = db::connect( "Driver=SQL Server;Server=Albert\\SQLEXPRESS;UID=Albert;PWD=winner;Database=LogonDB" );
	if( conn )
	{
		if( db::execute_sql( conn, "delete from test" ) == false )
		{
			puts( db::get_error_info() );
		}

		db::command cmd = db::create_command( db::dbCmdStoredProc, "InsertData" );
		if( cmd )
		{
			// 数值型参数测试
			db::insert_param_tinyint( cmd, "ti", db::dbParamInput, 123 );
			db::insert_param_smallint( cmd, "si", db::dbParamInput, 5432 );
			db::insert_param_integer( cmd, "ii", db::dbParamInput, 12345678 );
			db::insert_param_bigint( cmd, "bi", db::dbParamInput, 123456789 );
			db::insert_param_bigint( cmd, "ret", db::dbParamReturnValue, 0 );
			if( db::execute_cmd( conn, cmd ) == false )
			{
				puts( db::get_error_info() );
			}
			printf( "ret value = %d", db::get_param_integer(cmd, "ret" ) );

			// mbsc字符串测试
			db::reset_command( cmd, db::dbCmdStoredProc, "InsertString" );

			db::insert_param_string( cmd, "c", db::dbParamInput, "01234567890123456789012345678901", 32 );
			db::insert_param_vstring( cmd, "vc", db::dbParamInput, "01234567890123456789012345678901", 32 );
			db::insert_param_lvstring( cmd, "lvc", db::dbParamInput, "01234567890123456789012345678901", 32 );
			if( db::execute_cmd( conn, cmd ) == false )
			{
				puts( db::get_error_info() );
			}

			// unicode字符串测试
			db::reset_command( cmd, db::dbCmdStoredProc, "InsertWString" );

			db::insert_param_wstring( cmd, "c", db::dbParamInput, L"01234567890123456789012345678901", 32 );
			db::insert_param_vwstring( cmd, "vc", db::dbParamInput, L"01234567890123456789012345678901", 32 );
			db::insert_param_lvwstring( cmd, "lvc", db::dbParamInput, L"01234567890123456789012345678901", 32 );
			if( db::execute_cmd( conn, cmd ) == false )
			{
				puts( db::get_error_info() );
			}

			db::destroy_command( cmd );
		}
		db::disconnect( conn );
	}
	db::final();
	_getch();
}

void dotest()
{
	test_allocator();
}