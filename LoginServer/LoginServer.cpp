// LoginServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <conio.h>
#include <stdlib.h>
#include <process.h>
#include "Database.h"
using namespace XGC;
#include <streambuf>
unsigned int __stdcall DatabaseThread( void* param );
//--------------------------------------------------------//
//	created:	3:12:2009   13:36
//	filename: 	LoginServer
//	author:		Albert.xu
//
//	purpose:	开始应用程序
//--------------------------------------------------------//
VOID Start();

//--------------------------------------------------------//
//	created:	3:12:2009   13:35
//	filename: 	LoginServer
//	author:		Albert.xu
//
//	purpose:	退出应用程序
//--------------------------------------------------------//
BOOL WINAPI Stop( DWORD dwCtrlType );

HANDLE g_hExit = INVALID_HANDLE_VALUE;
BOOL g_bWork = TRUE;

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale( LC_ALL, "chs" );
	SetConsoleCtrlHandler( Stop, TRUE );

	COORD c = { 120, 160 };
	SMALL_RECT rc = { 0, 0, 119, 39 };

	SetConsoleScreenBufferSize( GetStdHandle( STD_OUTPUT_HANDLE ), c );
	SetConsoleWindowInfo( GetStdHandle( STD_OUTPUT_HANDLE ), TRUE, & rc );

	HANDLE hCommand = CreateConsoleScreenBuffer( GENERIC_READ|GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );

	SetConsoleScreenBufferSize( hCommand, c );
	SetConsoleWindowInfo( hCommand, TRUE, &rc );

	SetConsoleActiveScreenBuffer( hCommand );

	CConsoleAdapter *pAdapter = new CConsoleAdapter( hCommand, _T("ConsoleLog") );
	CLogger::GetInstance( LOG )->AddAdapter( pAdapter, true );

	g_hExit = CreateEvent( NULL, TRUE, FALSE, NULL );
	
	Start();
	_tchar szCommand[1024];
	DWORD dwRead = 0;
	while( g_bWork )
	{
		if( _kbhit() )
		{
			int ch = _gettch();
			switch( ch )
			{
			case 0x1b:
				Stop(0);
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
	WaitForSingleObject( g_hExit, INFINITE );
}

//--------------------------------------------------------//
//	created:	3:12:2009   13:36
//	filename: 	LoginServer
//	author:		Albert.xu
//
//	purpose:	开始应用程序
//--------------------------------------------------------//
IMessageQueue *g_msg_queue_ptr = NULL;
long_ptr g_server_h;
VOID Start()
{
	InitNetwork();
	//--------------------------------------------------------//
	//	初始化数据库
	//--------------------------------------------------------//
	db::initial( db::mssql );

	// 初始化网络
	g_server_h = StartServer( "0.0.0.0", 19944, &g_msg_queue_ptr, -1 );

}

BOOL WINAPI Stop( DWORD dwCtrlType )
{
	g_bWork = FALSE;

	CLoggerAdapter *pAdapter = CLogger::GetInstance( LOG )->RemoveAdapter( _T("ConsoleLog") );
	delete pAdapter;

	CloseServer( g_server_h );
	g_msg_queue_ptr->Release();
	FiniNetwork();

	db::final();
	SetEvent( g_hExit );
	return TRUE;
}


void testdb()
{
	db::initial( db::mssql );
	db::connection conn = db::connect( "Driver=SQL Server;Server=Albert\\SQLEXPRESS;UID=Albert;PWD=winner;Database=LogonDB" );
	if( conn )
	{
		if( db::execute_sql( conn, "delete from test" ) == false )
		{
			puts( db::get_error_info(conn) );
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
				puts( db::get_error_info(conn) );
			}
			printf( "ret value = %d", db::get_param_integer(cmd, "ret" ) );

			// mbsc字符串测试
			db::reset_command( cmd, db::dbCmdStoredProc, "InsertString" );

			db::insert_param_string( cmd, "c", db::dbParamInput, "01234567890123456789012345678901", 32 );
			db::insert_param_vstring( cmd, "vc", db::dbParamInput, "01234567890123456789012345678901", 32 );
			db::insert_param_lvstring( cmd, "lvc", db::dbParamInput, "01234567890123456789012345678901", 32 );
			if( db::execute_cmd( conn, cmd ) == false )
			{
				puts( db::get_error_info(conn) );
			}

			// unicode字符串测试
			db::reset_command( cmd, db::dbCmdStoredProc, "InsertWString" );

			db::insert_param_wstring( cmd, "c", db::dbParamInput, L"01234567890123456789012345678901", 32 );
			db::insert_param_vwstring( cmd, "vc", db::dbParamInput, L"01234567890123456789012345678901", 32 );
			db::insert_param_lvwstring( cmd, "lvc", db::dbParamInput, L"01234567890123456789012345678901", 32 );
			if( db::execute_cmd( conn, cmd ) == false )
			{
				puts( db::get_error_info(conn) );
			}

			db::destroy_command( cmd );
		}
		db::disconnect( conn );
	}
	db::final();
	_getch();
}

