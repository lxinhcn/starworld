#pragma once
#ifndef _DB_H
#define _DB_H
#include <queue>
#include "IDB.h"
#include "lookaside.h"
#include "critical_lock.h"
#include "macro_define.h"

#ifdef USE_MYSQL
	#pragma comment( lib, "libmySQL.lib" )
#endif // USE_MYSQL

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS
#pragma warning( disable :	4251 ) 

class TOOLLIB_API CDBRequest;
class TOOLLIB_API CConnection;

//##ModelId=46722CE2003E
class TOOLLIB_API CConnMng
{
public:
	//##ModelId=46722CE2004E
	CConnMng();
	//##ModelId=46722CE2004F
	~CConnMng();

	//////////////////////////////////////////////////////////////////////////
	// 开始数据库服务
	// return 服务是否成功开启
	//////////////////////////////////////////////////////////////////////////
	//##ModelId=46722CE20050
	bool DBStart();

	//////////////////////////////////////////////////////////////////////////
	// 停止数据库服务
	//////////////////////////////////////////////////////////////////////////
	//##ModelId=46722CE20051
	void DBStop();

	//////////////////////////////////////////////////////////////////////////
	// 注册一个连接
	// lpszConnString	:	连接字符串
	// nHartTime		:	心跳时间
	// nExecuteTime		:	无数据库请求时的等待时间
	// return			:	是否成功加入数据库连接表
	//////////////////////////////////////////////////////////////////////////
	//##ModelId=46722CE2005D
	bool RegistConnection( LPCTSTR lpszConnString, int nHartTime = 1000, int nExecuteTime = 100 );

	//////////////////////////////////////////////////////////////////////////
	// 删除一个连接
	// 这里认为连接都使用相同的表
	// return	:	有多少个连接被清除
	//////////////////////////////////////////////////////////////////////////
	//##ModelId=46722CE20061
	int UnRegistConnection( LPCTSTR lpszConnString );

	//////////////////////////////////////////////////////////////////////////
	// 投递一个数据库请求
	//////////////////////////////////////////////////////////////////////////
	//##ModelId=46722CE20063
	void PostRequest( IDBRequest* pRequest );

private:
	//##ModelId=46722CE2006D
	static unsigned int __stdcall DBThreadLoop( void* lpParam );

	//##ModelId=46722CE2009C
	typedef struct _THREADPARAM_
	{
		_string			strConnString;
		volatile LONG	bWork;
		IConnection*	pConn;
		int				nHartTime;
		int				nExecuteTime;
		HANDLE			hHandle;
	}*PTHREADPARAM, THREADPARAM;

	typedef std::vector< PTHREADPARAM >	CThreadParamVec;
	//##ModelId=46722CE20070
	CRITICAL_SECTION	m_lock;
	//##ModelId=46722CE2008D
	CThreadParamVec		m_ParamVec;
	//##ModelId=46722CE20091
	bool				m_bStart;
};

//##ModelId=46722CE200BB
class TOOLLIB_API CConnection :	public IConnection
{
friend class CRecordset;
public:
	//##ModelId=46722CE200BD
	CConnection(void);
	//##ModelId=46722CE200BF
	~CConnection(void);

	// 打开数据库
	// szConn	:	连接串
	// return	:	true 连接成功	false 连接失败
	// remark	:	一个完整的连接串示例
	// -ip 127.0.0.1 -port 3369 -user user -pwd password -database files
	//##ModelId=46722CE200CB
	virtual bool open( LPCSTR szConn );

	// 投递数据库操作。
	//##ModelId=46722CE200CE
	virtual bool request( IDBRequest* pRequest );

	//##ModelId=46722CE200D1
	virtual IRecordset* execute( const char* szSql );
	//##ModelId=46722CE200D4
	virtual bool execute_request();

private:
	struct Impl;
	//##ModelId=46722CE200DB
	Impl* m_Impl;
	//##ModelId=46722CE20104
	typedef std::queue< IDBRequest* >	CRequestQueue;

	//##ModelId=46722CE200E0
	_string		m_strIP;
	//##ModelId=46722CE200E5
	_string		m_strDatabase;
	//##ModelId=46722CE200EA
	_string		m_strUser;
	//##ModelId=46722CE200EF
	_string		m_strPwd;
	//##ModelId=46722CE200F3
	int				m_nPort;
	//##ModelId=46722CE200F5
	CRequestQueue	m_RequestQueue;

	//##ModelId=46722CE200FB
	CCritical		m_Lock;
	//##ModelId=46722CE20100
	CCritical		m_ExecuteLock;
};

//##ModelId=46722CE20138
class TOOLLIB_API CRecordset	:	public IRecordset
{
friend class CConnection;
public:
	//##ModelId=46722CE20149
	CRecordset(void);
	//##ModelId=46722CE2014B
	~CRecordset(void);

	//##ModelId=46722CE2014D
	virtual bool	open( const IConnection* pConn, const char* sql );
	//##ModelId=46722CE20151
	virtual bool	add_new();
	//##ModelId=46722CE20153
	virtual bool	edit();
	//##ModelId=46722CE20155
	virtual bool	update();
	//##ModelId=46722CE20158
	virtual long	get_record_count();

	//##ModelId=46722CE2015A
	virtual bool	next_row();
	//##ModelId=46722CE2015C
	virtual bool	prve_row();

	//##ModelId=46722CE2015E
	virtual long		get_field_count();
	//##ModelId=46722CE20160
	virtual const char* get_field_name( int nIndex );

	//##ModelId=46722CE20163
	virtual bool		set_field_value( const char* field_name, const char* field_value );
	//##ModelId=46722CE20167
	virtual bool		set_field_value( int nIndex, const char* field_value );

	//##ModelId=46722CE2016B
	virtual const char*	get_field_value( const char* field_name );
	//##ModelId=46722CE2016E
	virtual const char* get_field_value( int nIndex );
	//##ModelId=46722CE20171
	virtual void		release();

private:
	struct Impl;
	//##ModelId=46722CE20174
	Impl* m_Impl;
};

//##ModelId=46722CE201A5
class TOOLLIB_API CDBRequest	:	public IDBRequest
{
public:
	//##ModelId=46722CE201B5
	CDBRequest();
	//##ModelId=46722CE201B6
	virtual ~CDBRequest();

	//////////////////////////////////////////////////////////////////////////
	// 设置请求内容
	// nClientID	：	发送请求的客户端ID
	// szSQL		：	请求的SQL语句，以0结尾
	//////////////////////////////////////////////////////////////////////////
	//##ModelId=46722CE201B8
	virtual void SetRequest( int nUserID, int nTransmitID, const char* szSQL );

	//////////////////////////////////////////////////////////////////////////
	// 得到请求结果
	// return		：	结果集
	//////////////////////////////////////////////////////////////////////////
	//##ModelId=46722CE201BD
	virtual IRecordset* GetRecordset();

	//////////////////////////////////////////////////////////////////////////
	// 请求执行时调用
	// pConnection	：	数据库连接对象，通过该对象调用execute
	// return		：	是否正确执行
	// remark
	//		返回值为true时，程序继续执行，直到将结果集传递给调用方。
	//		返回值为false时，程序将抛弃这个请求。
	//////////////////////////////////////////////////////////////////////////
	//##ModelId=46722CE201BF
	virtual bool OnExecute( IConnection* pConnection );

	//////////////////////////////////////////////////////////////////////////
	// 选择哪个数据库连接进行操作
	// nConnectionCount	:	数据库个数
	// return			:	使用的数据库索引,必须为有效值
	//////////////////////////////////////////////////////////////////////////
	//##ModelId=46722CE201C2
	virtual int HashConnection( int nConnectionCount );

protected:
	//##ModelId=46722CE201C5
	int			m_nUserID;
	//##ModelId=46722CE201C6
	int			m_nTransmitID;
	//##ModelId=46722CE201C7
	bool		m_bFinish;
	//##ModelId=46722CE201C9
	_string		 m_strSql;

private:
	//##ModelId=46722CE201CE
	IRecordset* m_pRS;
};
#endif // _DB_H
