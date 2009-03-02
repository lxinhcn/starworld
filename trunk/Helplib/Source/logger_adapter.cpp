#include "StdAfx.h"
#include "logger_adapter.h"
#include <WinSock2.h>
#include <time.h>
#include <atlconv.h>

#include "NetStream.h"
#include "utility.h"
//--------------------------------------------------------------------------------------------------------//
// 
CSocketAdapter::~CSocketAdapter()
{
	if( m_socket != INVALID_SOCKET )
	{
		shutdown( m_socket, SD_BOTH );
		closesocket( m_socket );
	}
}

//--------------------------------------------------------------------------------------------------------//
// 连接日志服务器
// szHost		:	日志服务器ip
// nPort		:	日志服务器端口
// return		:	连接是否成功
//--------------------------------------------------------------------------------------------------------//
bool CSocketAdapter::Connect( LPCTSTR szHost, int nPort )
{
	SOCKADDR_IN		ServerAddr;

	USES_CONVERSION;
	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= inet_addr( W2A(szHost) );
	ServerAddr.sin_port			= htons( nPort );

	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	int hResult = connect( m_socket, ( SOCKADDR *)&ServerAddr, sizeof( ServerAddr ) );
	if( hResult == SOCKET_ERROR )
	{
		int nErrorCode = WSAGetLastError();
		closesocket( m_socket );
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------//
// 关联一个Socket
// socket		:	指定的套接字
// return		:	是否成功
//--------------------------------------------------------------------------------------------------------//
bool CSocketAdapter::AttachSocket( DWORD s )
{
	if( s == INVALID_SOCKET )	return false;
	m_socket = (SOCKET)s;
	return true;
}

//--------------------------------------------------------------------------------------------------------//
// 写日志
// szBuffer		:	日志缓冲区，由用户提供
// size			:	缓冲区大小
//--------------------------------------------------------------------------------------------------------//
int CSocketAdapter::WriteLog( _string strLoggerName, LPCTSTR szBuffer, size_t size )
{
	TCHAR szLog[4096];
	CNetInStream in( (char*)szLog, sizeof( szLog ) );
	try
	{
		in	<< DWORD( 0 )
			<< BYTE( 0x53 )
			<< strLoggerName.c_str() << GetName()
			<< size;
		in.write( (const char*)szBuffer, size*sizeof(TCHAR) );
		in << end;
		in.set( 0 );
		in << in.length() - sizeof(DWORD);
	}
	catch( std::runtime_error& e )
	{
		e.what();
		return 0;
	}
	
	return send( m_socket, in.get(), (int)in.length(), 0 );
}

//--------------------------------------------------------------------------------------------------------//
// class CFileAdapter
//--------------------------------------------------------------------------------------------------------//
CFileAdapter::~CFileAdapter()
{
	Close();
}

int CFileAdapter::CombinedFileName( LPTSTR lptszBuf, size_t nCount )
{
	int nRet = 0;
	switch( m_enSplitMode )
	{
	case split_none:
		{
			nRet = _sntprintf( lptszBuf, nCount, _T("%s.%s"), m_strFilename.c_str(), m_strExt.c_str() );
		}
		break;
	case split_date:
		{
			time_t ltm = time( &ltm );
			tm* ptm = localtime( &ltm );
			nRet = _sntprintf( lptszBuf, nCount, _T("%s_%02d%02d.%s"), m_strFilename.c_str(), ptm->tm_mon, ptm->tm_mday, m_strExt.c_str() );
			m_lpSplitParam = ptm->tm_mday;
		}
		break;
	case split_line:
		{
			nRet = _sntprintf( lptszBuf, nCount, _T("%s_%d.%s"), m_strFilename.c_str(), m_nCurLine%m_lpSplitParam, m_strExt.c_str() );
		}
		break;
	}
	return nRet;
}

//--------------------------------------------------------------------------------------------------------//
// 打开日志文件
// szFilename	:	日志文件名
// return		:	文件是否打开
//--------------------------------------------------------------------------------------------------------//
bool CFileAdapter::Open( LPCTSTR szFilename, LPCTSTR szExt, CFileAdapter::EN_FILEMODE enSplitMode, LPARAM lpParam )
{
	if( m_file )
	{
		return false;
	}
	m_strFilename	= szFilename;
	m_strExt		= szExt;
	m_enSplitMode	= enSplitMode;
	m_lpSplitParam	= lpParam&0xffff;

	TCHAR tszFile[_MAX_PATH];
	CombinedFileName( tszFile, _countof( tszFile ) );

	if( ( m_file = _tfopen( tszFile, _T("a") ) ) == NULL )
	{
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------//
// 关闭日志文件
//--------------------------------------------------------------------------------------------------------//
void CFileAdapter::Close()
{
	if( m_file )
	{
		fclose(m_file);
		m_file = NULL;
	}
}

//--------------------------------------------------------------------------------------------------------//
// 写日志
// szBuffer		:	日志缓冲区，由用户提供
// size			:	缓冲区大小
//--------------------------------------------------------------------------------------------------------//
int CFileAdapter::WriteLog( _string strLoggerName, LPCTSTR szBuffer, size_t size )
{
	if( !m_file ) return 0;

#ifdef _DEBUG
	bool	bDebug = true;
#else
	bool	bDebug = false;
#endif

	if( GetLevel() == CLoggerAdapter::ADAPTER_DEBUG && bDebug == false )	return 0;

	TCHAR szLog[4096];
	TCHAR szDate[128];
	TCHAR szTime[128];
	_tstrdate( szDate );
	_tstrtime( szTime );
	ASSERT( debug_helper::IsValidAddress( szBuffer, size, FALSE ) );
	int len = _sntprintf( szLog, sizeof( szLog ), _T("- %s %s [%s].[%s]:%s\r\n"), szDate, szTime, strLoggerName.c_str(), GetName(), szBuffer );

	if( bDebug || GetLevel() == ADAPTER_ERROR )
	{
		switch( m_enSplitMode )
		{
		case split_line:
			if( m_lpSplitParam && !(m_nCurLine%m_lpSplitParam) )
			{
				TCHAR szFilename[_MAX_PATH];
				if( CombinedFileName( szFilename, _countof( szFilename ) ) > 0 )
				{
					FILE* fp = NULL;
					if( ( fp = _tfopen( szFilename, _TEXT("a") ) ) != NULL )
					{
						Close();
						m_file = fp;
					}
				}
				else
				{
					return 0;
				}
			}
			break;
		case split_date:
			{
				time_t ltm = time( &ltm );
				tm* ptm = localtime( &ltm );
				if( ptm->tm_mday != m_lpSplitParam )
				{
					TCHAR szFilename[_MAX_PATH];
					if( CombinedFileName( szFilename, _countof( szFilename ) ) > 0 )
					{
						FILE* fp = NULL;
						if( ( fp = _tfopen( szFilename, _TEXT("a") ) ) != NULL )
						{
							Close();
							m_file = fp;
							m_lpSplitParam = ptm->tm_mday;
						}
					}
					else
					{
						return 0;
					}
				}
			}
			break;
		}
		_fputts( szLog, m_file );
		if( m_nWriteVelocity && !(++m_nCurLine%m_nWriteVelocity) )
		{
			fflush( m_file );
		}
	}
	else if( bDebug || GetLevel() == ADAPTER_WRONG )
	{
		OutputDebugString( szLog );
	}
	return len;
}

//--------------------------------------------------------------------------------------------------------//
// WindowsAdapter [4/4/2007]
// CWindowAdapter
//--------------------------------------------------------------------------------------------------------//

CWindowAdapter::CWindowAdapter( HWND hEditHwnd, LPCTSTR lpszName )
: CLoggerAdapter( lpszName, CLoggerAdapter::ADAPTER_ERROR )
, m_hEditHwnd( hEditHwnd )
, m_nTotalLen( 0 )
{
}

CWindowAdapter::~CWindowAdapter(void)
{
}

//--------------------------------------------------------------------------------------------------------//
// 写日志
// szBuffer		:	日志缓冲区，由用户提供
// size			:	缓冲区大小
//--------------------------------------------------------------------------------------------------------//
int CWindowAdapter::WriteLog( _string strLoggerName, LPCTSTR szBuffer, size_t size )
{
	TCHAR szLog[4096];
	TCHAR szDate[128];
	TCHAR szTime[128];
	_tstrdate( szDate );
	_tstrtime( szTime );
	ASSERT( debug_helper::IsValidAddress( szBuffer, size, FALSE ) );
	int len = _sntprintf( szLog, sizeof( szLog ), _T("-%s %s :%s\r\n"), szDate, szTime, szBuffer );

	DWORD dwResult;
	if( m_hEditHwnd )
	{
		if( m_nTotalLen > 1024 * 4 )
		{
			::SendMessageTimeout(m_hEditHwnd,   EM_SETSEL,   0,   m_nTotalLen, SMTO_BLOCK, 10, &dwResult );			
			::SendMessageTimeout(m_hEditHwnd,   EM_REPLACESEL,   0,  (LPARAM)"", SMTO_BLOCK, 10, &dwResult);
			m_nTotalLen = 0;
		}
		::SendMessageTimeout(m_hEditHwnd,   EM_SETSEL,   -2,   -1, SMTO_BLOCK, 10, &dwResult);				//将当前位置设在Edit控件的最后并清除当前选择   
		::SendMessageTimeout(m_hEditHwnd,   EM_REPLACESEL,   0,  (LPARAM)szLog, SMTO_BLOCK, 10, &dwResult);	//添加一行字符
		m_nTotalLen += len;
	}
	//::SendMessage( m_hDlgHwnd, WM_USER + 0x4031, 0, (LPARAM)szLog );
	return 0;
}

//--------------------------------------------------------------------------------------------------------//
// CConsoleAdapter [10/29/2007]
// CConsoleAdapter
//--------------------------------------------------------------------------------------------------------//
CConsoleAdapter::CConsoleAdapter( LPCTSTR lpszName/* = _T("WindowLog")*/, CLoggerAdapter::ENLEVEL enLevel/* = CLoggerAdapter::ADAPTER_ERROR*/ )
:CLoggerAdapter( lpszName, enLevel )
{
}

CConsoleAdapter::~CConsoleAdapter(void)
{
}
//--------------------------------------------------------------------------------------------------------//
// 写日志
// szBuffer		:	日志缓冲区，由用户提供
// size			:	缓冲区大小
//--------------------------------------------------------------------------------------------------------//
int CConsoleAdapter::WriteLog( _string strLoggerName, LPCTSTR szBuffer, size_t size )
{
	_tprintf( _T("-%s :%s\r\n"), strLoggerName.c_str(), szBuffer );
	return 0;
}
