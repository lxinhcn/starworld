#include "StdAfx.h"
//#include "StringFunc.h"
//#include "critical_lock.h"
//#include "db.h"
//#include "utility.h"
//
//	//##ModelId=46722CE2010A
//struct CConnection::Impl
//{
//		//##ModelId=46722CE2010D
//	MYSQL*		m_pMySQL;
//		//##ModelId=46722CE20119
//	MYSQL_RES*	inner_execute( const char* szSql )const;
//};
//
//	//##ModelId=46722CE20178
//struct CRecordset::Impl
//{
//		//##ModelId=46722CE2017B
//	MYSQL_RES	*	m_res;
//		//##ModelId=46722CE20187
//	MYSQL_FIELD	*	m_fd;
//		//##ModelId=46722CE20197
//	MYSQL_ROW		m_row;
//};
//
////////////////////////////////////////////////////////////////////////////
//// class CConnMng
////////////////////////////////////////////////////////////////////////////
//// Bug Report [2/2/2007]
//// 初始化过程中， 如果很快就退出，则会导致param被过早删除，以至于DBThreadLoop线程中初始化时引用了无效的指针。
//
////##ModelId=46722CE2004E
//CConnMng::CConnMng()
//: m_bStart( false )
//{
//	InitializeCriticalSection( &m_lock );
//}
//
////##ModelId=46722CE2004F
//CConnMng::~CConnMng()
//{
//	{ 
//		CCriticalLock lock( m_lock );
//		ASSERT( !m_bStart );
//
//		CThreadParamVec::const_iterator citer = m_ParamVec.begin();
//		CThreadParamVec::const_iterator citerend = m_ParamVec.end();
//		while( citer != citerend )
//		{
//			PTHREADPARAM param	= *citer;
//			if( param )
//			{
//				if( param->bWork )
//				{
//					::InterlockedExchange( &param->bWork, 0 );
//					WaitForSingleObject( param->hHandle, max(param->nHartTime,param->nExecuteTime) + 100 );
//					CloseHandle( param->hHandle );
//				}
//				delete param;
//			}
//			++citer;
//		}
//		m_ParamVec.clear();
//	}
//	DeleteCriticalSection( &m_lock );
//}
//
////##ModelId=46722CE2005D
//bool CConnMng::RegistConnection( LPCTSTR lpszConnString, int nHartTime, int nExecuteTime )
//{
//	CCriticalLock lock( m_lock );
//	debug_helper::IsValidString( lpszConnString );
//	if( m_bStart )	return false;
//
//	PTHREADPARAM param		= new THREADPARAM;
//	if( !param )
//	{
//		return false;
//	}
//
//	param->strConnString	= lpszConnString;
//	param->bWork			= false;
//	param->pConn			= NULL;
//	param->nHartTime		= nHartTime;
//	param->nExecuteTime		= nExecuteTime;
//
//	m_ParamVec.push_back( param );
//
//	return true;
//}
//
////##ModelId=46722CE20061
//int CConnMng::UnRegistConnection( LPCTSTR lpszConnString )
//{
//	CCriticalLock lock( m_lock );
//	if( m_bStart ) return 0;
//	CThreadParamVec::iterator iter		= m_ParamVec.begin();
//	CThreadParamVec::iterator iterend	= m_ParamVec.end();
//	while( iter != iterend )
//	{
//		PTHREADPARAM param		= *iter;
//		if( param->strConnString == lpszConnString )
//		{
//			CThreadParamVec::iterator iterold =  iter++;
//
//			m_ParamVec.erase( iterold );
//			continue;
//		}
//		++iter;
//	}
//	return true;
//}
//
////////////////////////////////////////////////////////////////////////////
//// 开始数据库服务
//// return 服务是否成功开启
////////////////////////////////////////////////////////////////////////////
////##ModelId=46722CE20050
//bool CConnMng::DBStart()
//{
//	if( m_ParamVec.empty() )	return false;
//	if( m_bStart )				return false;
//
//	CCriticalLock lock( m_lock );
//	m_bStart = true;
//
//	CThreadParamVec::const_iterator citer = m_ParamVec.begin();
//	CThreadParamVec::const_iterator citerend = m_ParamVec.end();
//	while( citer != citerend )
//	{
//		PTHREADPARAM param		= *citer;
//		ASSERT( param->bWork == FALSE );
//		param->hHandle = (HANDLE)_beginthreadex( NULL, 0, CConnMng::DBThreadLoop, param, 0, NULL );
//		++citer;
//	}
//	return true;
//}
//
////////////////////////////////////////////////////////////////////////////
//// 停止数据库服务
////////////////////////////////////////////////////////////////////////////
////##ModelId=46722CE20051
//void CConnMng::DBStop()
//{
//	CCriticalLock lock( m_lock );
//	if( !m_bStart ) return;
//
//	CThreadParamVec::const_iterator citer		= m_ParamVec.begin();
//	CThreadParamVec::const_iterator citerend	= m_ParamVec.end();
//	while( citer != citerend )
//	{
//		PTHREADPARAM param	= *citer;
//		if( param )
//		{
//			::InterlockedExchange( &param->bWork, 0 );
//			WaitForSingleObject( param->hHandle, INFINITE /*max(param->nHartTime,param->nExecuteTime) + 100*/ );
//			CloseHandle( param->hHandle );
//			param->hHandle = INVALID_HANDLE_VALUE;
//		}
//		++citer;
//	}
//	m_bStart = false;
//}
//
////##ModelId=46722CE20063
//void CConnMng::PostRequest( IDBRequest* pRequest )
//{
//	if( !pRequest ) return;
//
//	ASSERT( m_ParamVec.size() );
//	int nSelect = pRequest->HashConnection( (int)m_ParamVec.size() );
//	ASSERT( nSelect >= 0 && nSelect < (int)m_ParamVec.size() );
//
//	if( nSelect >= 0 && nSelect < (int)m_ParamVec.size() )
//	{
//		// 这里由于不会在运行时更改m_ParamVec数组，所以没有进行同步。
//		PTHREADPARAM param = m_ParamVec[nSelect];
//		if( param && param->pConn )
//		{
//			param->pConn->request( pRequest );
//		}
//	}
//}
//
////##ModelId=46722CE2006D
//unsigned int __stdcall CConnMng::DBThreadLoop( void* lpParam )
//{
//	ASSERT( lpParam );
//	if( lpParam == NULL )	return 0;
//
//	PTHREADPARAM pParam = reinterpret_cast< PTHREADPARAM >( lpParam );
//	pParam->bWork = true;
//
//	// 必须在线程里创建，因为要根据线程创建一些必要的数据。使用到了线程本地存储
//	// 否则例如GetLastInsertID这样的调用将会有不可预期的返回
//	// MySql用户手册上这么说的。
//	pParam->pConn = new CConnection();
//	DWORD	dwTime = timeGetTime();
//	if( pParam->pConn && pParam->pConn->open( pParam->strConnString.c_str() ) )
//	{
//		while( pParam->bWork )
//		{
//			if( !pParam->pConn->execute_request() )
//			{
//				Sleep( pParam->nExecuteTime );
//				DWORD dwNowTime = timeGetTime();
//				if( dwNowTime - dwTime >= (DWORD)pParam->nHartTime )
//				{
//					IRecordset* pRs = pParam->pConn->execute( _TEXT( "select 1" ) );
//					if( pRs ) pRs->release();
//					dwTime = dwNowTime;
//				}
//			}
//		}
//	}
//	IConnection* pTmpConn = pParam->pConn;
//	pParam->pConn = NULL;
//	delete pTmpConn;
//	return 0;
//}
//
////////////////////////////////////////////////////////////////////////////
//// class CConnection
////////////////////////////////////////////////////////////////////////////
//
////##ModelId=46722CE200BD
//CConnection::CConnection(void)
//: m_Impl( new Impl )
//{
//	m_Impl->m_pMySQL = mysql_init( NULL );
//}
//
////##ModelId=46722CE200BF
//CConnection::~CConnection(void)
//{
//	mysql_close( m_Impl->m_pMySQL );
//	m_Impl->m_pMySQL = NULL;
//	delete m_Impl;
//}
//
////##ModelId=46722CE200CB
//bool CConnection::open( LPCSTR szConn )
//{
//	// -ip 127.0.0.1 -port 3369 -user user -pwd password -database files
//	CCriticalLock lock( m_ExecuteLock );
//	char szAttrib[256];
//	char szValue[256];
//	size_t i = 0;
//	size_t nLen = _tcslen( szConn );
//	while( i < nLen )
//	{
//		i += StringFunc::FirstSegment( szConn + i, szAttrib, sizeof( szAttrib ), ' ' );
//		if( i > nLen )	return false;
//		i += StringFunc::FirstSegment( szConn + i, szValue, sizeof( szValue ), ' ' );
//		if( i > nLen )	return false;
//
//		if( _tcscmp( "-ip", szAttrib ) == 0 )
//		{
//			m_strIP = szValue;
//		}
//		else if( _tcscmp( "-port", szAttrib ) == 0 )
//		{
//			m_nPort = atoi( szValue );
//		}
//		else if( _tcscmp( "-database", szAttrib ) == 0 )
//		{
//			m_strDatabase = szValue;
//		}
//		else if( _tcscmp( "-user", szAttrib ) == 0 )
//		{
//			m_strUser = szValue;
//		}
//		else if( _tcscmp( "-pwd", szAttrib ) == 0 )
//		{
//			m_strPwd = szValue;
//		}
//	}
//
//	if( !mysql_real_connect( m_Impl->m_pMySQL, m_strIP.c_str(), m_strUser.c_str(), m_strPwd.c_str(), m_strDatabase.c_str(), m_nPort, NULL, 0 ) )
//	{
//		const char* szError = mysql_error( m_Impl->m_pMySQL );
//		CLogger::GetInstance( DATABASE_LOG )->WriteLog( szError );
//		return false;
//	}
//
//	return true;
//}
//
////##ModelId=46722CE200CE
//bool CConnection::request( IDBRequest* pRequest )
//{
//	if( pRequest )
//	{
//		CCriticalLock lock( m_Lock );
//		m_RequestQueue.push( pRequest );
//		return true;
//	}
//	return false;
//}
//
////##ModelId=46722CE20119
//MYSQL_RES* CConnection::Impl::inner_execute( const char* szSql )const
//{
//	if( mysql_query( m_pMySQL, szSql ) )
//	{
//		const char* pszError = mysql_error( m_pMySQL );
//		OutputDebugString( pszError );
//		CLogger::GetInstance( DATABASE_LOG )->WriteLog( pszError );
//		return NULL;
//	}
//	return mysql_store_result( m_pMySQL );
//}
//
////##ModelId=46722CE200D1
//IRecordset* CConnection::execute( const char* szSql )
//{
//	CCriticalLock lock( m_ExecuteLock );
//	MYSQL_RES* pRes = m_Impl->inner_execute( szSql );
//
//	if( pRes )
//	{
//		CRecordset* pRS = new CRecordset;
//		pRS->m_Impl->m_res = pRes;
//		pRS->m_Impl->m_fd = mysql_fetch_fields( pRes );
//		return static_cast< IRecordset* >( pRS );
//	}
//	else
//	{
//		unsigned int errcode = mysql_errno( m_Impl->m_pMySQL );
//		if( errcode == 2006 )
//		{
//			if( !mysql_real_connect( m_Impl->m_pMySQL, m_strIP.c_str(), m_strUser.c_str(), m_strPwd.c_str(), m_strDatabase.c_str(), m_nPort, NULL, 0 ) )
//			{
//				const char* szError = mysql_error( m_Impl->m_pMySQL );
//				CLogger::GetInstance( DATABASE_LOG )->WriteLog( szError );
//			}
//		}
//	}
//	return NULL;
//}
//
////////////////////////////////////////////////////////////////////////////
//// return	：	返回是否仍有请求
////##ModelId=46722CE200D4
//bool CConnection::execute_request()
//{
//	IDBRequest* pRequest = NULL;
//	{
//		CCriticalLock lock( m_Lock );
//		if( m_RequestQueue.empty() )	return false;
//
//		pRequest = m_RequestQueue.front();
//		m_RequestQueue.pop();
//	}
//	if( pRequest->OnExecute( this ) )
//	{
//		if( pRequest->OnResult() )
//		{
//			pRequest->Release();
//		}
//		else
//		{
//			CCriticalLock lock( m_Lock );
//			m_RequestQueue.push( pRequest );
//		}
//	}
//	return true;
//}
//
////////////////////////////////////////////////////////////////////////////
//// class CRecordset
////////////////////////////////////////////////////////////////////////////
//
////##ModelId=46722CE20149
//CRecordset::CRecordset(void)
//: m_Impl( new Impl )
//{
//	m_Impl->m_res = NULL;
//	m_Impl->m_fd = NULL;
//}
//
////##ModelId=46722CE2014B
//CRecordset::~CRecordset(void)
//{
//	if( m_Impl->m_res )
//	{
//		mysql_free_result( m_Impl->m_res );
//		m_Impl->m_res = NULL;
//	}
//
//	if( m_Impl->m_fd )
//	{
//		m_Impl->m_fd = NULL;
//	}
//	delete m_Impl;
//}
//
////##ModelId=46722CE2014D
//bool CRecordset::open( const IConnection* pConn, const char* sql )
//{
//	if( !pConn )	return false;
//
//	const CConnection* pConnection = static_cast< const CConnection* >( pConn );
//	m_Impl->m_res = pConnection->m_Impl->inner_execute( sql );
//
//	m_Impl->m_fd = mysql_fetch_fields( m_Impl->m_res );
//	return m_Impl->m_res != NULL;
//}
//
////##ModelId=46722CE20151
//bool CRecordset::add_new()
//{
//	return true;
//}
//
////##ModelId=46722CE20153
//bool CRecordset::edit()
//{
//	return true;
//}
//
////##ModelId=46722CE20155
//bool CRecordset::update()
//{
//	return true;
//}
//
////##ModelId=46722CE20158
//long CRecordset::get_record_count()
//{
//	return (long)mysql_num_rows( m_Impl->m_res );
//}
//
////##ModelId=46722CE2015A
//bool CRecordset::next_row()
//{
//	m_Impl->m_row = mysql_fetch_row( m_Impl->m_res );
//	if( m_Impl->m_row == NULL )	return false;
//	return true;
//}
//
////##ModelId=46722CE2015C
//bool CRecordset::prve_row()
//{
//	return false;
//}
//
////##ModelId=46722CE2015E
//long CRecordset::get_field_count()
//{
//	return mysql_num_fields( m_Impl->m_res );
//}
//
////##ModelId=46722CE20160
//const char* CRecordset::get_field_name( int nIndex )
//{
//	if( nIndex >= 0 && nIndex < get_field_count() )
//	{
//		return m_Impl->m_fd[nIndex].name;
//	}
//	return "";
//}
//
////##ModelId=46722CE20163
//bool CRecordset::set_field_value( const char* field_name, const char* field_value )
//{
//	return true;
//}
//
////##ModelId=46722CE20167
//bool CRecordset::set_field_value( int nIndex, const char* field_value )
//{
//	return true;
//}
//
////##ModelId=46722CE2016B
//const char*	CRecordset::get_field_value( const char* field_name )
//{
//	for( int i = 0; i < get_field_count(); ++i )
//	{
//		if( _tcscmp( m_Impl->m_fd[i].name, field_name ) == 0 )
//		{
//			return m_Impl->m_row[i];
//		}
//	}
//	return "";
//}
//
////##ModelId=46722CE2016E
//const char* CRecordset::get_field_value( int nIndex )
//{
//	if( nIndex >= 0 && nIndex < get_field_count() )
//	{
//		return m_Impl->m_row[nIndex];
//	}
//	return "";
//}
//
////##ModelId=46722CE20171
//void CRecordset::release()
//{
//	delete this;
//}
//
////////////////////////////////////////////////////////////////////////////
//// class CDBRequest
////////////////////////////////////////////////////////////////////////////
////##ModelId=46722CE201B5
//CDBRequest::CDBRequest()
//: m_pRS( NULL )
//, m_nUserID( 0 )
//, m_nTransmitID( 0 )
//, m_bFinish( false )
//{
//}
//
////##ModelId=46722CE201B6
//CDBRequest::~CDBRequest()
//{
//	if( m_pRS )
//	{
//		m_pRS->release();
//		m_pRS = NULL;
//	}
//}
////////////////////////////////////////////////////////////////////////////
//// 设置请求内容
//// nClientID	：	发送请求的客户端ID
//// szSQL		：	请求的SQL语句，以0结尾
////////////////////////////////////////////////////////////////////////////
////##ModelId=46722CE201B8
//void CDBRequest::SetRequest( int nUserID, int nTransmitID, const char* szSQL )
//{
//	m_nUserID = nUserID;
//	m_nTransmitID = nTransmitID;
//	m_strSql = szSQL;
//}
//
////////////////////////////////////////////////////////////////////////////
//// 得到请求结果
//// return		：	结果集
////////////////////////////////////////////////////////////////////////////
////##ModelId=46722CE201BD
//IRecordset* CDBRequest::GetRecordset()
//{
//	return m_pRS;
//}
//
////////////////////////////////////////////////////////////////////////////
//// 请求执行时调用
//// pConnection	：	数据库连接对象，通过该对象调用execute
//// return		：	是否正确执行
//// remark
////		返回值为true时，程序继续执行，直到将结果集传递给调用方。
////		返回值为false时，程序将抛弃这个请求。
////////////////////////////////////////////////////////////////////////////
////##ModelId=46722CE201BF
//bool CDBRequest::OnExecute( IConnection* pConnection )
//{
//	m_pRS = pConnection->execute( m_strSql.c_str() );
//	return true;
//}
//
////##ModelId=46722CE201C2
//int CDBRequest::HashConnection( int nConnectionCount )
//{
//	return m_nUserID%nConnectionCount;
//}
