#pragma once
#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

class TOOLLIB_API IRecordset;
class TOOLLIB_API IDBRequest;

class TOOLLIB_API IConnection
{
public:
	virtual ~IConnection(){};
	virtual bool		open( LPCSTR szConn ) = 0;
	virtual bool		request( IDBRequest* pRequest ) = 0;

	virtual IRecordset* execute( const char* szSql ) = 0;
	virtual bool		execute_request() = 0;
};

class TOOLLIB_API IRecordset
{
public:
	~IRecordset(void){};

	virtual bool		open( const IConnection* pConn, const char* sql ) = 0;
	virtual bool		add_new() = 0;
	virtual bool		edit() = 0;
	virtual bool		update() = 0;
	virtual long		get_record_count() = 0;
	virtual bool		next_row() = 0;
	virtual bool		prve_row() = 0;

	virtual long		get_field_count() = 0;
	virtual const char* get_field_name( int nIndex ) = 0;
	
	virtual bool		set_field_value( const char* field_name, const char* field_value ) = 0;
	virtual bool		set_field_value( int nIndex, const char* field_value ) = 0;

	virtual const char*	get_field_value( const char* field_name ) = 0;
	virtual const char* get_field_value( int nIndex ) = 0;
	virtual void		release() = 0;
};

class TOOLLIB_API IDBRequest
{
public:
	virtual ~IDBRequest(){}

	//////////////////////////////////////////////////////////////////////////
	// 设置请求内容
	// nClientID	：	发送请求的客户端ID
	// szSQL		：	请求的SQL语句，以0结尾
	//////////////////////////////////////////////////////////////////////////
	virtual void SetRequest( int nUserID, int nTransmitID, const char* szSQL ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// 得到请求结果
	// return		：	结果集
	//////////////////////////////////////////////////////////////////////////
	virtual IRecordset* GetRecordset() = 0;

	//////////////////////////////////////////////////////////////////////////
	// 请求执行时调用
	// pConnection	：	数据库连接对象，通过该对象调用execute
	// return		：	是否正确执行
	// remark
	//		返回值为true时，程序继续执行，直到将结果集传递给调用方。
	//		返回值为false时，程序将抛弃这个请求。
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnExecute( IConnection* pConnection )	= 0;

	//////////////////////////////////////////////////////////////////////////
	// 选择哪个数据库连接进行操作
	// nConnectionCount	:	数据库个数
	// return			:	使用的数据库索引,必须为有效值
	//////////////////////////////////////////////////////////////////////////
	virtual int HashConnection( int nConnectionCount ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// 有结果返回时调用
	// pRecordset	：	结果集对象
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnResult()		= 0;

	//////////////////////////////////////////////////////////////////////////
	// 释放内存
	//////////////////////////////////////////////////////////////////////////
	virtual void Release() = 0;
};