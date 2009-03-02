#include "StdAfx.h"
#include "netevent.h"

CNetEvent::CNetEvent( LONG nMaxSocketCount )
: m_nTotalNet( 0 )
, m_bWork( true )
, m_hThread( INVALID_HANDLE_VALUE )
, m_nMaxSocketCount( nMaxSocketCount )
, m_nListenIndex( -1 )
, m_bConnect( false )
{
	m_pSocketArray	= new SOCKET[m_nMaxSocketCount];
	m_pEventArray	= new WSAEVENT[m_nMaxSocketCount];
	m_pClientArray	= new ISocketBase*[m_nMaxSocketCount];

	for( int i = 0; i < m_nMaxSocketCount; ++ i )
	{
		m_pSocketArray[i]	= INVALID_SOCKET;
		m_pEventArray[i]	= INVALID_HANDLE_VALUE;
		m_pClientArray[i]	= NULL;
	}

	m_hThread = (HANDLE)_beginthreadex( NULL, 0, EventThreadLoop, this, 0, NULL );
	if( m_hThread == INVALID_HANDLE_VALUE )
	{
		m_bWork = false;
	}
}

CNetEvent::~CNetEvent(void)
{
	if( m_bWork )
	{
		Stop();
		m_bWork = false;
		if( m_hThread != INVALID_HANDLE_VALUE )
		{
			::InterlockedExchange( &m_bWork, 0 );
			::WaitForSingleObject( m_hThread, 500 );
			CloseHandle( m_hThread );
			m_hThread = INVALID_HANDLE_VALUE;
		}
	}

	// 清理数组
	delete[] m_pSocketArray;
	delete[] m_pEventArray;
	delete[] m_pClientArray;
	m_pSocketArray = NULL;
	m_pEventArray = NULL;
	m_pClientArray = NULL;
}

LONG CNetEvent::CreateSocketHandle( SOCKET s )
{
	CCriticalLock lock( m_Lock );
	if( m_nTotalNet > m_nMaxSocketCount )
	{
		return -1;
	}

	for( int i = 0; i < m_nMaxSocketCount; ++i )
	{
		if( m_pSocketArray[i] == INVALID_SOCKET )
		{
			// 创建需要的对象
			try
			{
				if( s != INVALID_SOCKET )
				{
					m_pSocketArray[i] = s;
				}
				else
				{
					SOCKET s = socket( AF_INET, SOCK_STREAM, 0 );
					if( s == INVALID_SOCKET ) throw( std::runtime_error( "create socket error" ) );
					m_pSocketArray[i] = s;
				}

				WSAEVENT we	= WSACreateEvent();
				if( we == WSA_INVALID_EVENT ) throw( std::runtime_error( "create event error" ) );
				m_pEventArray[i] = we;

				ISocketBase* sc = AllocClient();
				if( sc == NULL ) throw( std::runtime_error( "create client error" ) );
				m_pClientArray[i]	= sc;
				*sc = m_pSocketArray[i];
			}
			catch( std::runtime_error& e )
			{
				(void)e;
				//CLogger::GetInstance( TOOLLIB_ERROR )->WriteLog( A2W(e.what()) );
				CloseSocketHandle( i );
				return -1;
			}

			// 增加当前连接计数
			++m_nTotalNet;
			return i;
		}
	}
	return -1;
}

BOOL CNetEvent::CloseSocketHandle( LONG hHandle )
{
	if( !IsValidSocketHandle( hHandle ) ) return FALSE;
	CCriticalLock lock( m_Lock );
	if( m_pSocketArray[hHandle] != INVALID_SOCKET )
	{
		WSAEventSelect( m_pSocketArray[hHandle], m_pEventArray[hHandle], 0 );
		if( closesocket( m_pSocketArray[hHandle] ) == SOCKET_ERROR )
		{
			int err = WSAGetLastError();
			err;
		}
		m_pSocketArray[hHandle] = INVALID_SOCKET;
	}
	else
	{
		return FALSE;
	}

	if( m_pEventArray[hHandle] != INVALID_HANDLE_VALUE && WSACloseEvent( m_pEventArray[hHandle] ) )
	{
		m_pEventArray[hHandle] = INVALID_HANDLE_VALUE;
	}

	if( m_pClientArray[hHandle] != NULL )
	{
		m_pClientArray[hHandle]->Release();
		m_pClientArray[hHandle] = NULL;
	}

	--m_nTotalNet;
	ASSERT( m_nTotalNet >= 0 );
	m_pEventArray[hHandle]	= m_pEventArray[m_nTotalNet];
	m_pSocketArray[hHandle]	= m_pSocketArray[m_nTotalNet];
	m_pClientArray[hHandle]	= m_pClientArray[m_nTotalNet];

	return TRUE;
}

BOOL CNetEvent::Connect( LPCSTR lpszHost, int nPort, DWORD dwTimeOut )
{
	if( m_nListenIndex != -1 ) return TRUE;

	CCriticalLock lock( m_Lock );
	LONG nIndex = m_nListenIndex = CreateSocketHandle();
	if( nIndex == -1 )
	{
		return -1;
	}

	SOCKADDR_IN		ServerAddr;

	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= inet_addr( lpszHost );
	ServerAddr.sin_port			= htons( nPort );

	WSAEventSelect( m_pSocketArray[nIndex], m_pEventArray[nIndex], FD_CONNECT|FD_READ|FD_WRITE|FD_CLOSE );
	int nRet = connect( m_pSocketArray[nIndex], (PSOCKADDR)&ServerAddr, sizeof( ServerAddr ) );
	int result = WSAGetLastError();
	if( nRet == -1 && WSAGetLastError() != WSAEWOULDBLOCK )
	{
		CloseSocketHandle( nIndex );
		return -1;
	}

	// 等待一个事件返回
	DWORD dwRet = WSAWaitForMultipleEvents( 1, &m_pEventArray[nIndex], FALSE, dwTimeOut, FALSE );
	if( dwRet == WSA_WAIT_FAILED || dwRet == WSA_WAIT_TIMEOUT )
	{
		Disconnect();
		return FALSE;
	}
	if( m_pClientArray[nIndex] )
	{
		m_pClientArray[nIndex]->hHandle = nIndex;
	}
	m_bConnect = true;
	return TRUE;
}

VOID CNetEvent::Disconnect()
{
	LONG nIndex = m_nListenIndex;
	m_nListenIndex = -1;
	m_bConnect = false;
	CloseSocketHandle( nIndex );
}

INT	CNetEvent::Send( const char* data, int len )
{
	if( m_nListenIndex >= 0 && m_nListenIndex < m_nMaxSocketCount )
	{
		SOCKET s = m_pSocketArray[m_nListenIndex];
		if( s != INVALID_SOCKET )
		{
			return send( s, data, len, 0 );
		}
	}
	return SOCKET_ERROR;
}

bool CNetEvent::Start( int nPort )
{
	if( m_nListenIndex != -1 ) return false;
	m_nListenIndex = CreateSocketHandle();

	if( m_nListenIndex == -1 ) return false;

	SOCKADDR_IN		ServerAddr;

	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_addr.s_addr	= INADDR_ANY;
	ServerAddr.sin_port			= htons( nPort );

	int nResult = 0;
	nResult = bind( m_pSocketArray[m_nListenIndex], (PSOCKADDR)&ServerAddr, sizeof( ServerAddr ) );
	if( nResult == SOCKET_ERROR )
	{
		CloseSocketHandle( m_nListenIndex );
		return false;
	}

	nResult = WSAEventSelect( m_pSocketArray[m_nListenIndex], m_pEventArray[m_nListenIndex], FD_ACCEPT|FD_CLOSE|FD_CONNECT );
	if( nResult == SOCKET_ERROR )
	{
		CloseSocketHandle( m_nListenIndex );
		return false;
	}

	nResult = listen( m_pSocketArray[m_nListenIndex], 20 );
	if( nResult == SOCKET_ERROR )
	{
		CloseSocketHandle( m_nListenIndex );
		return false;
	}
	return true;
}

void CNetEvent::Stop()
{
	for( LONG i = 0; i < m_nMaxSocketCount; ++i )
	{
		CloseSocketHandle( i );
	}
}

unsigned int __stdcall CNetEvent::EventThreadLoop( LPVOID lParam )
{
	CNetEvent* pThis = reinterpret_cast< CNetEvent* >( lParam );
	if( !pThis )	return 1;

	WSANETWORKEVENTS NetWorkEvents;
	while( pThis->IsWork() )
	{
		if( pThis->m_nTotalNet == 0 )
		{
			Sleep( 1000 );
			continue;
		}

		DWORD nIndex = WSAWaitForMultipleEvents( pThis->m_nTotalNet, pThis->m_pEventArray, FALSE, 200, FALSE );
		if( WSA_WAIT_FAILED == nIndex )
		{
			int nRet = WSAGetLastError();
			continue;
		}
		else if( WSA_WAIT_TIMEOUT == nIndex )
		{
			continue;
		}

		nIndex -= WSA_WAIT_EVENT_0;
		int nRet = WSAEnumNetworkEvents( pThis->m_pSocketArray[nIndex], pThis->m_pEventArray[nIndex], &NetWorkEvents );
		if( nRet != SOCKET_ERROR )
		{
			if( NetWorkEvents.lNetworkEvents & FD_ACCEPT )
			{
				if( NetWorkEvents.iErrorCode[FD_ACCEPT_BIT] != 0 )
				{
					// SendErrorCode( CODE_LOG, "接收socket连接请求时失败。" );
				}
				else
				{
					SOCKET sAccept = accept( pThis->m_pSocketArray[pThis->m_nListenIndex], NULL, NULL );
					LONG nIndex = pThis->CreateSocketHandle( sAccept );
					WSAEventSelect( sAccept, pThis->m_pEventArray[nIndex], FD_READ|FD_WRITE|FD_CLOSE );
					pThis->m_pClientArray[nIndex]->OnAccept();
				}
			}
			else if( NetWorkEvents.lNetworkEvents & FD_CONNECT )
			{
				pThis->m_pClientArray[nIndex]->OnConnect();
			}
			else if( NetWorkEvents.lNetworkEvents & FD_READ )
			{
				//u_long nLen = 0;
				//do 
				//{
				pThis->m_pClientArray[nIndex]->OnRead();
				//	ioctlsocket( pThis->m_pSocketArray[nIndex], FIONREAD, &nLen );
				//}while( nLen > 0 );
			}
			else if( NetWorkEvents.lNetworkEvents & FD_WRITE )
			{
				pThis->m_pClientArray[nIndex]->OnWrite();
			}
			else if( NetWorkEvents.lNetworkEvents & FD_CLOSE )
			{
				pThis->m_pClientArray[nIndex]->OnClose();
				if( pThis->m_nListenIndex == nIndex )
				{
					pThis->Disconnect();
				}
				else
				{
					pThis->CloseSocketHandle( nIndex );
				}
				continue;
			}
		}
		WSAResetEvent( pThis->m_pEventArray[nIndex] );
	}
	return 0;
}
