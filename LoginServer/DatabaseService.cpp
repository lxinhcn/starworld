#include "StdAfx.h"
#include "DatabaseService.h"
#include <process.h>

BEGIN_DISPATCHER_TABLE( CDatabaseService, database )
	DECLARE_DISPATCH( db_user_regist,	userRegist )
	DECLARE_DISPATCH( db_user_logon,	userLogon )
END_DISPATCHER_TABLE( CDatabaseService, database )
CDatabaseService::CDatabaseService(void)
: m_transqueue( 1024 )
{
}

CDatabaseService::~CDatabaseService(void)
{
}

bool CDatabaseService::start( _lpcstr connstring )
{
	m_connstring = connstring;
	m_thread_h = _beginthreadex( NULL, 0, CDatabaseService::svc, NULL, 0, NULL );
	if( m_thread_h == -1 )
		return false;
	return true;
}

void CDatabaseService::stop()
{
	m_work_b = false;
	WaitForSingleObject( (HANDLE)m_work_b, INFINITE );
}

size_t CDatabaseService::process( db::connection conn, transaction &header )
{
	func pFunc = DISPATCHER_GET( database, header.mid );
	if( pFunc )
		return (this->*pFunc)( conn, header );

	return -1;
}

unsigned int __stdcall CDatabaseService::svc( _lpvoid pParam )
{
	CDatabaseService *pService = (CDatabaseService*)pParam;

	pService->m_work_b = true;
	INetPacket *pPacket = NULL;
	db::connection conn = db::connect( pService->m_connstring.c_str() );
	if( !conn )
		return -1;

	while( pService->m_work_b )
	{
		transaction t = pService->m_transqueue.get();
		// 处理数据
		pService->process( conn, t );
	}

	return 0;
}

//--------------------------------------------------------//
//	created:	3:12:2009   13:11
//	filename: 	DispatchDatabase
//	author:		Albert.xu
//
//	purpose:	注册一个账号
//--------------------------------------------------------//
size_t CDatabaseService::userRegist( db::connection conn, transaction &header )
{
	dbRegistUser* msg = (dbRegistUser*)header.msg;

	db::command cmd = db::create_command( db::dbCmdStoredProc, "RegistUser" );
	if( cmd )
	{
		db::insert_param_string( cmd, "@username", db::dbParamInput, msg->username, strlen( msg->username ) );
		db::insert_param_string( cmd, "@password", db::dbParamInput, msg->password, strlen( msg->password ) );
		db::insert_param_integer( cmd, "@Return", db::dbParamReturnValue, 0 );

		if( db::execute_cmd( conn, cmd ) == false )
		{
			XGC_WRITELOGA( LOG, db::get_error_info(conn) );
		}

		int ret = db::get_param_integer( cmd, "@Return" );

		MMO_RegistReplay nmsg;
		nmsg.failed_reason = ret;

		char buf[1024];
		bufstream sbuf( buf, sizeof(buf) );
		sbuf << nmsg;

		SendPacket( header.uid, sbuf.base(), sbuf.wd() );

		db::destroy_command( cmd );
		return 1;
	}
	return -1;
}

//--------------------------------------------------------//
//	created:	3:12:2009   13:11
//	filename: 	DispatchDatabase
//	author:		Albert.xu
//
//	purpose:	用户登陆
//--------------------------------------------------------//
size_t CDatabaseService::userLogon( db::connection conn, transaction &header )
{
	dbUserLogon* msg = (dbUserLogon*)header.msg;

	db::command cmd = db::create_command( db::dbCmdStoredProc, "UserLogon" );
	if( cmd )
	{
		db::insert_param_string( cmd, "@Username", db::dbParamInput, msg->username, strlen( msg->username ) );
		db::insert_param_string( cmd, "@Password", db::dbParamInput, msg->password, strlen( msg->password ) );
		db::insert_param_integer( cmd, "@Return", db::dbParamReturnValue, 0 );

		if( db::execute_cmd( conn, cmd ) == false )
		{
			XGC_WRITELOGA( LOG, db::get_error_info(conn) );
		}

		int ret = db::get_param_integer( cmd, "@Return" );

		db::destroy_command( cmd );
		return 1;
	}
	return -1;
}
