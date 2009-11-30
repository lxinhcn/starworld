// LoginServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <conio.h>
#include "Database.h"
using namespace XGC;
int _tmain(int argc, _TCHAR* argv[])
{
	db::initial( db::mssql );
	db::connection conn = db::connect( "Driver=SQL Server;Server=Albert\\SQLEXPRESS;UID=Albert;PWD=winner;Database=LogonDB" );
	if( conn )
	{
		db::command cmd = db::create_command( db::dbCmdStoredProc, "InsertData" );
		if( cmd )
		{
			db::insert_param_bigint( cmd, "@bigint", db::dbParamInput, 123456789 );
			db::insert_param_integer( cmd, "@integer", db::dbParamInput, 12345678 );
			db::insert_param_smallint( cmd, "@smallint", db::dbParamInput, 54321 );
			db::insert_param_tinyint( cmd, "@tinyint", db::dbParamInput, 123 );
			db::insert_param_bigint( cmd, "@ret", db::dbParamReturnValue, 0 );
			if( db::execute_cmd( conn, cmd ) == false )
			{
				puts( db::get_error_info(conn) );
			}
			printf( "ret value = %d", db::get_param_integer(cmd, "@ret" ) );
			db::destroy_command( cmd );
		}
		db::disconnect( conn );
	}
	db::final();
	_getch();
	return 0;
}

