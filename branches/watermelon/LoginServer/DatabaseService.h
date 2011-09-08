#pragma once
#include "TransQueue.h"

class CDatabaseService;
typedef message_dispatcher
< 
	CDatabaseService, 
	size_t (CDatabaseService::*)( db::connection conn, transaction *header ), 
	short 
> 
DbDispatcherBase;

class CDatabaseService	:	public DbDispatcherBase
{
	IMPLEMENT_DISPATCHER_TABLE( database );
private:
	uintptr_t		m_thread_h;
	bool			m_work_b;
	_astring		m_connstring;

	// 事务队列
	CTransQueue		m_transqueue;

public:
	CDatabaseService(void);
	~CDatabaseService(void);

	//--------------------------------------------------------//
	//	created:	29:12:2009   16:48
	//	filename: 	DatabaseService
	//	author:		Albert.xu
	//
	//	purpose:	创建数据库服务
	//--------------------------------------------------------//
	bool start( _lpcstr connstring );
	void stop();

protected:
	// 数据库服务线程
	static unsigned int __stdcall svc( _lpvoid pParam );

	//--------------------------------------------------------//
	//	created:	29:12:2009   16:49
	//	filename: 	DatabaseService
	//	author:		Albert.xu
	//
	//	purpose:	处理数据库事务
	//--------------------------------------------------------//
	size_t process( db::connection conn, transaction *header );

	//--------------------------------------------------------//
	//	created:	3:12:2009   13:11
	//	filename: 	DispatchDatabase
	//	author:		Albert.xu
	//
	//	purpose:	注册一个账号
	//--------------------------------------------------------//
	size_t userRegist( db::connection conn, transaction *header );

	//--------------------------------------------------------//
	//	created:	3:12:2009   13:11
	//	filename: 	DispatchDatabase
	//	author:		Albert.xu
	//
	//	purpose:	用户登陆
	//--------------------------------------------------------//
	size_t userLogon( db::connection conn, transaction *header );
};
