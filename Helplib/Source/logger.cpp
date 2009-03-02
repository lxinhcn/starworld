#include "StdAfx.h"
#include "utility.h"
#include "logger.h"
#include "logger_adapter.h"
//--------------------------------------------------------------------------------------------------------//
// class CLogger 
// 日志管理者，日至记录入口
//--------------------------------------------------------------------------------------------------------//
struct CLogger::IMPL
{
	typedef std::list< CLoggerAdapter* >	CLoggerAdapterList;
	typedef std::map< _string, logobj >		CLoggerInstanceMap;
	typedef std::list< _string >			CLoggerBufferList;

	CLoggerAdapterList			m_AdapterList;
	CLoggerBufferList			m_LoggerList;
	CRITICAL_SECTION			m_AdapterListLock;

	static CLoggerInstanceMap	m_LoggerMap;
	static CCritical			m_LoggerMapLock;

	logger_mode					m_mode;
};

CLogger::IMPL::CLoggerInstanceMap	CLogger::IMPL::m_LoggerMap;
CCritical							CLogger::IMPL::m_LoggerMapLock;

CLogger::CLogger( LPCTSTR szLoggerName )
: m_pImpl( new IMPL )
, m_strLoggerName( szLoggerName )
{
	InitializeCriticalSection( &m_pImpl->m_AdapterListLock );
	m_pImpl->m_mode = CLogger::normal;
}

CLogger::~CLogger(void)
{
	ASSERT( m_pImpl->m_AdapterList.empty() );
	DeleteCriticalSection( &m_pImpl->m_AdapterListLock );
	delete m_pImpl;
	m_pImpl = NULL;
}

//--------------------------------------------------------------------------------------------------------//
// 取得日志实例
// szLogger		:	实例名
//--------------------------------------------------------------------------------------------------------//
CLogger* CLogger::GetInstance( LPCTSTR szLogger )
{
	CCriticalLock lock( IMPL::m_LoggerMapLock );
	IMPL::CLoggerInstanceMap::const_iterator citer = IMPL::m_LoggerMap.find( szLogger );
	CLogger* pLogger = NULL;
	if( citer == IMPL::m_LoggerMap.end() )
	{
		pLogger = new CLogger( szLogger );
		IMPL::m_LoggerMap[szLogger] = pLogger;
	}
	else
	{
		pLogger = citer->second;
	}
	return pLogger;
}

//--------------------------------------------------------------------------------------------------------//
// 设置日志输出模式
//--------------------------------------------------------------------------------------------------------//
VOID CLogger::SetLoggerMode( logger_mode en_mode )
{
	CCriticalLock lock( IMPL::m_LoggerMapLock );

	IMPL::CLoggerInstanceMap::const_iterator citer = IMPL::m_LoggerMap.begin();
	while( citer != IMPL::m_LoggerMap.end() )
	{
		citer->second->SetMode( en_mode );
		++citer;
	}
}

//--------------------------------------------------------------------------------------------------------//
// 日志实例是否存在
// szLogger		:	实例名
//--------------------------------------------------------------------------------------------------------//
bool CLogger::Have( LPCTSTR szLogger )
{
	CCriticalLock lock( IMPL::m_LoggerMapLock );

	IMPL::CLoggerInstanceMap::const_iterator citer = IMPL::m_LoggerMap.find( szLogger );
	return citer != IMPL::m_LoggerMap.end();
}

//--------------------------------------------------------------------------------------------------------//
// 清除日至
// szLogger		:	实例名
//--------------------------------------------------------------------------------------------------------//
void CLogger::Clear( LPCTSTR szLogger )
{
	CCriticalLock lock( IMPL::m_LoggerMapLock );

	IMPL::CLoggerInstanceMap::iterator iter = IMPL::m_LoggerMap.find( szLogger );
	if( iter != IMPL::m_LoggerMap.end() )
	{
		IMPL::m_LoggerMap.erase( iter );
	}
}

//--------------------------------------------------------------------------------------------------------//
// 清除所有日至
//--------------------------------------------------------------------------------------------------------//
void CLogger::ClearAll()
{
	CCriticalLock lock( IMPL::m_LoggerMapLock );

	IMPL::m_LoggerMap.clear();
}

//--------------------------------------------------------------------------------------------------------//
// 添加一个日至适配器
// pAdapter		:	适配器指针
//--------------------------------------------------------------------------------------------------------//
bool CLogger::AddAdapter( CLoggerAdapter* pAdapter )
{
	CCriticalLock lock( m_pImpl->m_AdapterListLock );

	IMPL::CLoggerAdapterList::const_iterator citer = std::find( m_pImpl->m_AdapterList.begin(), m_pImpl->m_AdapterList.end(), pAdapter );
	if( citer == m_pImpl->m_AdapterList.end() )
	{
		m_pImpl->m_AdapterList.push_back( pAdapter );
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------//
// 删除一个适配器
// szAdapterName	:	适配器名
//--------------------------------------------------------------------------------------------------------//
CLoggerAdapter* CLogger::RemoveAdapter( LPCTSTR szAdapterName )
{
	CCriticalLock lock( m_pImpl->m_AdapterListLock );

	IMPL::CLoggerAdapterList::iterator iter		= m_pImpl->m_AdapterList.begin();
	IMPL::CLoggerAdapterList::iterator iter_end	= m_pImpl->m_AdapterList.end();
	CLoggerAdapter* pAdapter = NULL;
	while( iter != iter_end )
	{
		if( _tcscmp( szAdapterName, (*iter)->GetName() ) == 0 )
		{
			pAdapter = *iter;
			m_pImpl->m_AdapterList.erase( iter );
			break;
		}
		++iter;
	}
	return pAdapter;
}

//--------------------------------------------------------------------------------------------------------//
// 写日至
// szBuffer		:	日至缓冲区，由用户提供
// size			:	缓冲区大小
// fmt			:	格式串
// ...			:	参数列表
//--------------------------------------------------------------------------------------------------------//
int CLogger::WriteLog( LPTSTR szBuffer, size_t nSize, LPCTSTR szFmt, ... )
{
	ASSERT( this );
	ASSERT( szBuffer );
	ASSERT( szFmt );
	if( m_pImpl->m_AdapterList.empty() ) return 0;

	va_list args;
	va_start(args, szFmt);

	int size = _vsntprintf( szBuffer, nSize, szFmt, args );
	szBuffer[nSize-1] = 0;
	if( m_pImpl->m_mode == normal )
		DispatchLog( szBuffer, size );
	else if( m_pImpl->m_mode == pause )
		m_pImpl->m_LoggerList.push_back( szBuffer );
	else
		return 0;
	return size;
}

//--------------------------------------------------------------------------------------------------------//
// 写日至
// szBuffer		:	日至缓冲区，由用户提供
// ...			:	参数列表
// return		:	写入的字符数
// remark		:	小心，不能写如果长的字符，255个字符刚好。
//--------------------------------------------------------------------------------------------------------//
int CLogger::WriteLog( LPCTSTR szFmt, ... )
{
	TCHAR tszLog[255];
	ASSERT( this );
	ASSERT( szFmt );
	if( m_pImpl->m_AdapterList.empty() ) return 0;

	va_list args;
	va_start(args, szFmt);

	size_t nSize = _countof( tszLog );
	int size = _vsntprintf( tszLog, nSize, szFmt, args );
	if( size < 0 )	return 0;
	tszLog[nSize-1] = 0;
	if( m_pImpl->m_mode == normal )
		DispatchLog( tszLog, size );
	else if( m_pImpl->m_mode == pause )
		m_pImpl->m_LoggerList.push_back( tszLog );
	else
		return 0;
	return size;
}

//--------------------------------------------------------------------------------------------------------//
// 写日至
// szLog		:	日志串
// nSize		:	串长度
//--------------------------------------------------------------------------------------------------------//
int CLogger::WriteLog( size_t nSize, LPCTSTR szLog )
{
	ASSERT( this );
	ASSERT( szLog );
	if( m_pImpl->m_mode == normal )
		DispatchLog( szLog, nSize );
	else if( m_pImpl->m_mode == pause )
		m_pImpl->m_LoggerList.push_back( szLog );
	else 
		return 0;
	return (int)_tcslen( szLog );
}

//--------------------------------------------------------------------------------------------------------//
// 设置日志输出模式
//--------------------------------------------------------------------------------------------------------//
VOID CLogger::SetMode( logger_mode en_mode )
{
	m_pImpl->m_mode = en_mode;
	if( en_mode == CLogger::normal )
	{
		IMPL::CLoggerBufferList::const_iterator citer = m_pImpl->m_LoggerList.begin();
		while( citer != m_pImpl->m_LoggerList.end() )
		{
			DispatchLog( citer->c_str(), citer->length() );
			++citer;
		}
		m_pImpl->m_LoggerList.clear();
	}
}

//--------------------------------------------------------------------------------------------------------//
// 分发日至到适配器
// szLog		:	日志串
// nSize		:	串长度
//--------------------------------------------------------------------------------------------------------//
void CLogger::DispatchLog( LPCTSTR szLog, size_t nSize )
{
	CCriticalLock lock( m_pImpl->m_AdapterListLock );

	IMPL::CLoggerAdapterList::const_iterator citer		= m_pImpl->m_AdapterList.begin();
	IMPL::CLoggerAdapterList::const_iterator citer_end	= m_pImpl->m_AdapterList.end();
	while( citer != citer_end )
	{
		(*citer)->WriteLog( m_strLoggerName, szLog, nSize );
		++citer;
	}
}

CLogger::logobj::~logobj()
{
	if( m_pLog )
	{ 
		delete m_pLog;
		m_pLog = NULL;
	}
}
