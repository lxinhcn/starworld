#include "stdafx.h"
#include "utility.h"
#include "logger_server.h"
#include "logger.h"
#include "logger_adapter.h"
#include "netstream.h"

typedef std::map< _string, _string >	CUserMap;
static CUserMap	m_UserMap;

CLoggerClient::CLoggerClient()
: m_pHeadPackage( new CNetPackage( 4 ) )
, m_pPackage( NULL )
{
}

CLoggerClient::~CLoggerClient()
{
	delete m_pHeadPackage;
	if( m_pPackage )
	{
		delete m_pPackage;
		m_pPackage = NULL;
	}
}

void CLoggerClient::OnAccept()
{
}

void CLoggerClient::OnRead()
{
	int nRet = 0;
	char szBuffer[4096];
	nRet = recv( *this, szBuffer, sizeof( szBuffer ), 0 );
	if( nRet == SOCKET_ERROR )
	{
		nRet = WSAGetLastError();
		return;
	}
	{
		char* pDataBuffer = szBuffer;
		size_t size = nRet;
		while( size )
		{
			if( !m_pHeadPackage->is_done() )
			{
				// 读包头
				size_t read_size = m_pHeadPackage->push( pDataBuffer, size );
				size -= read_size;
				pDataBuffer += read_size;
			}
			else
			{
				// 包头读完了
				if( m_pPackage == NULL )
				{
					// 是否是新的数据包
					const int* pHead = ( const int* )m_pHeadPackage->get();
					size_t package_size = *pHead;
					if( package_size > 1024 * 1024 * 8 )
					{
						m_pHeadPackage->reset();
						pDataBuffer -= m_pHeadPackage->size() - 1;
						size += m_pHeadPackage->size() - 1;
						continue;
					}
					m_pPackage = new CNetPackage( package_size );
				}
				else
				{
					// 读数据
					size_t read_size = m_pPackage->push( pDataBuffer, size );
					size -= read_size;
					pDataBuffer += read_size;
				}

				if( m_pPackage->is_done() )
				{
					m_pHeadPackage->reset();
					// 数据读完了，开始业务处理。
					ProcessPackage();
					delete m_pPackage;
					m_pPackage = NULL;
				}
			}
		}
	}
	return;
}

void CLoggerClient::ProcessPackage()
{
	CNetOutStream out( m_pPackage->get(), m_pPackage->length() );
	const char* pUserName;
	const char* pPassWord;
	const char* pLoggerName;
	const char* pAdapterName;
	try
	{
		BYTE btCode;
		out >> btCode;
		switch( btCode )
		{
		case 0x01:	// 注册适配器
			{
				out >> pUserName >> pPassWord >> pLoggerName >> pAdapterName;
				USES_CONVERSION;
				_string strLoggerName = A2W(pLoggerName);
				if( !CLoggerServer::CheckTable( A2W(pUserName), A2W(pPassWord) ) )
				{
					try
					{
						char szMsg[1024];
						CNetInStream in( szMsg, sizeof( szMsg ) );
						TCHAR szLog[] = _TEXT( "用户名或密码错。" );
						in << DWORD( 1 + _tcslen( szLog ) + sizeof( TCHAR ) ) << BYTE( 0x51 ) << szLog << end;	// 加入日志失败
						send( *this, in.get(), (int)in.length(), 0 );
						shutdown( *this, SD_BOTH );
						closesocket( *this );
					}
					catch( std::runtime_error& e )
					{
						e.what();
					}

					closesocket( *this );
					return;
				}

				if( CLogger::Have( strLoggerName.c_str() ) )
				{
					CLogger* pLogger = CLogger::GetInstance( strLoggerName.c_str() );
					if( pLogger )
					{
						m_strLogger		= strLoggerName.c_str();
						m_strWatcher	= A2W(pAdapterName);

						CSocketAdapter* pAdapter = new CSocketAdapter( A2W(pAdapterName) );
						pAdapter->AttachSocket( (DWORD)*this );
						pLogger->AddAdapter( pAdapter );
						try
						{
							char szMsg[1024];
							CNetInStream in( szMsg, sizeof( szMsg ) );
							TCHAR szLog[] = _T( "已分配适配器。" );
							in << DWORD( 1 + _tcslen( szLog ) + sizeof( TCHAR ) ) << BYTE( 0x52 ) << szLog << end;
							send( *this, in.get(), (int)in.length(), 0 );
						}
						catch( std::runtime_error& e )
						{
							e.what();
						}
					}
				}
				else
				{
					try
					{
						char szMsg[1024];
						CNetInStream in( szMsg, sizeof( szMsg ) );
						TCHAR szLog[] = _T("请求的日至不存在。");
						in << DWORD( 1 + _tcslen( szLog ) + sizeof( TCHAR ) ) << BYTE( 0x51 ) << szLog << end;	// 加入日志失败
						send( *this, in.get(), (int)in.length(), 0 );
						shutdown( *this, SD_BOTH );
						closesocket( *this );
					}
					catch( std::runtime_error& e )
					{
						e.what();
					}
				}
			}
			break;
		}
	}
	catch( std::runtime_error& e )
	{
		e.what();
		return;
	}
}

void CLoggerClient::OnWrite()
{
	return;
}

void CLoggerClient::OnClose()
{
	CSocketAdapter* pAdapter = static_cast< CSocketAdapter* >( CLogger::GetInstance( m_strLogger.c_str() )->RemoveAdapter( m_strWatcher.c_str() ) );
	if( pAdapter )
	{
		delete pAdapter;
	}
	m_strWatcher.clear();
	return;
}

void CLoggerClient::OnConnect()
{
}

void CLoggerClient::Release()
{
	delete this;
}
//////////////////////////////////////////////////////////////////////////
// class CLoggerServer
//////////////////////////////////////////////////////////////////////////


ISocketBase* CLoggerServer::AllocClient()
{
	return new CLoggerClient;
}

bool CLoggerServer::InitUserTable( LPCTSTR lpszTableFile )
{
	FILE* fp = _tfopen( lpszTableFile, _TEXT( "r" ) );
	if( fp == NULL )
	{
		return false;
	}

	TCHAR szBuffer[4096];
	TCHAR szUser[256];
	//TCHAR szPass[256];
	while( !feof( fp ) )
	{
		TCHAR* pRet = _fgetts( szBuffer, _countof( szBuffer ), fp );
		if( pRet == NULL )
		{
			break;
		}

		_W64 int n = szBuffer - _tcschr( szBuffer, _T( '=' ) );
		szBuffer[n] = 0;
		_tcsncpy( szUser, szBuffer, _countof( szUser ) );
		_W64 int m = szBuffer - _tcschr( szBuffer + n + 1, _T( '\n' ) );
		szBuffer[m] = 0;
		m_UserMap[szUser] = szBuffer + n + 1;
	}
	return true;
}

bool CLoggerServer::CheckTable( LPCTSTR pUser, LPCTSTR pPass )
{
	CUserMap::const_iterator citer = m_UserMap.find( pUser );
	if( citer != m_UserMap.end() )
	{
		if( citer->second == pPass )
		{
			return true;
		}
	}
	return false;
}
