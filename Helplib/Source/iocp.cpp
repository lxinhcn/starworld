#include "StdAfx.h"
#include ".\iocp.h"
#include <process.h>
#include "utility.h"

iocp_event::iocp_event(ICompletionKey* pKey, bool bEvent /* = true  */)
: m_pKey( pKey )
{
	if( pKey )
	{
		pKey->AddRef();
	}

	ZeroMemory( static_cast< OVERLAPPED* >( this ), sizeof( OVERLAPPED ) );
	if( bEvent )
	{
		hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	}
}

iocp_event::~iocp_event()
{
	if( hEvent )
	{
		CloseHandle( hEvent );
	}

	if( m_pKey )
	{
		m_pKey->Release();
		m_pKey = NULL;
	}
	hEvent = NULL;
}

struct iocp::impl
{
	//static LPFN_ACCEPTEX					lpAcceptEx;
	//static LPFN_TRANSMITFILE				lpTransmitFile;
	//static LPFN_GETACCEPTEXSOCKADDRS		lpGetAcceptExSockaddrs;
	volatile HANDLE	hIOCP;
	CWorkThreadVec	WorkThreadVec;
	CKeyMap			KeyMap;
	CCritical		lock;
};

//LPFN_ACCEPTEX				iocp::impl::lpAcceptEx				= NULL;
//LPFN_TRANSMITFILE			iocp::impl::lpTransmitFile			= NULL;
//LPFN_GETACCEPTEXSOCKADDRS	iocp::impl::lpGetAcceptExSockaddrs	= NULL;

iocp::iocp(void)
: m_impl( new impl )
{
	m_impl->hIOCP = INVALID_HANDLE_VALUE;
}

iocp::~iocp(void)
{
	stop_iocp();
	uninitialize();
	SAFE_DELETE( m_impl );
}

bool iocp::initialize()
{
	m_impl->hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	return m_impl->hIOCP != NULL;
}

void iocp::uninitialize()
{
	if( m_impl->hIOCP != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_impl->hIOCP );
		m_impl->hIOCP = INVALID_HANDLE_VALUE;
	}
}

//--------------------------------------------------------------------------------------------------------//
// 将handle关联到完成端口
// handle	： 被关联的完成端口
// pKey		： 完成键，每个handle有一个完成键
// NumberOfConcurrentThreads	：	这个完成端口上最多允许同时存在的线程数
//--------------------------------------------------------------------------------------------------------//
bool iocp::associat( HANDLE handle, ICompletionKey* pKey, DWORD NumberOfConcurrentThreads )
{
	CCriticalLock _lock( m_impl->lock );
	// 判断是否完成端口已经销毁（销毁中）或未建立。
	if( m_impl->hIOCP == INVALID_HANDLE_VALUE ) return false;

	HANDLE hret = CreateIoCompletionPort( handle, m_impl->hIOCP, reinterpret_cast< ULONG_PTR >( pKey ), NumberOfConcurrentThreads );
	if( hret != NULL )
	{
		if( pKey ) pKey->AddRef();
		m_impl->KeyMap[handle] = pKey;
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------------------------------------//
// 解除handle的关联
// handle	:	被关联的句柄
// return	:	和handle对应的Key的指针
//--------------------------------------------------------------------------------------------------------//
ICompletionKey* iocp::disassociat( HANDLE handle )
{
	CCriticalLock _lock( m_impl->lock );
	CKeyMap::iterator iter = m_impl->KeyMap.find( handle );
	if( iter != m_impl->KeyMap.end() )
	{
		ICompletionKey* pKey = iter->second;
		m_impl->KeyMap.erase( iter );
		if( pKey )	pKey->Release();
		return pKey;
	}
	return NULL;
}

//--------------------------------------------------------------------------------------------------------//
// 查询handle的关联 会有AddRef 引用操作。
// handle	:	被关联的句柄
// return	:	和handle对应的Key的指针
//--------------------------------------------------------------------------------------------------------//
ICompletionKey* iocp::query( HANDLE handle )
{
	CCriticalLock _lock( m_impl->lock );
	CKeyMap::iterator iter = m_impl->KeyMap.find( handle );
	if( iter != m_impl->KeyMap.end() )
	{
		ICompletionKey* pKey = iter->second;
		LONG ref = pKey->AddRef();
		if( ref > 0 ) return pKey;
	}
	return NULL;
}

//--------------------------------------------------------------------------------------------------------//
// 枚举当前的完成端口句柄和完成键
// FuncCallback	:	回调函数指针
// lpData		:	用户数据
// return		:	是否继续	true - 继续	false - 终止
//--------------------------------------------------------------------------------------------------------//
HANDLE iocp::enum_key( iocp::EnumComlationKey FuncCallback, LPVOID lpData )
{
	CCriticalLock _lock( m_impl->lock );
	CKeyMap::iterator iter = m_impl->KeyMap.begin();
	while( iter != m_impl->KeyMap.end() )
	{
		ICompletionKey* pKey = iter->second;
		if( pKey )
		{
			LONG ref = pKey->AddRef();
			if( ref > 0 ) 
			{
				try
				{
					if( !FuncCallback( iter->first, pKey, lpData ) )
					{
						HANDLE hKey = pKey->Get();
						pKey->Release();
						return hKey;
					}
				}
				catch(...)
				{
					pKey->Release();
					return NULL;
				}
			}
			pKey->Release();
		}
		++iter;
	}
	return NULL;
}

bool iocp::start_iocp( DWORD dwNumberOfThread, work pfn, LPVOID lpParam )
{
	if( dwNumberOfThread == 0 )
	{
		SYSTEM_INFO SysInfo;
		GetSystemInfo(&SysInfo);

		dwNumberOfThread = SysInfo.dwNumberOfProcessors * 2 + 2;
	}

	if( lpParam == NULL )
	{
		lpParam = (LPVOID)this;
	}

	for( DWORD i = 0; i < dwNumberOfThread; ++i )
	{
		HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, pfn, lpParam, 0, NULL );
		if( hThread <= 0 )
		{
			puts( "error on create thread!\n" );
			break;
		}
		m_impl->WorkThreadVec.push_back( hThread );
	}
	return true;
}

void iocp::stop_iocp()
{
	// 将关联的端口相关的操作都取消
	if( m_impl->hIOCP == INVALID_HANDLE_VALUE ) return;
	CCriticalLock _lock( m_impl->lock );
	HANDLE hIOCP = m_impl->hIOCP;
	m_impl->hIOCP = INVALID_HANDLE_VALUE;
	CKeyMap::const_iterator citer = m_impl->KeyMap.begin();
	while( citer != m_impl->KeyMap.end() )
	{
		if( citer->second )
		{
			citer->second->Close();
			}
		++citer;
	}
	m_impl->KeyMap.clear();
	Sleep( 1000 );

	for( size_t i = 0; i < m_impl->WorkThreadVec.size(); ++i )
	{
		PostQueuedCompletionStatus( hIOCP, -1, NULL, NULL );
	}

	WaitForMultipleObjects( (DWORD)m_impl->WorkThreadVec.size(), &m_impl->WorkThreadVec[0], TRUE, 10000 );

	if( hIOCP != INVALID_HANDLE_VALUE )
	{
		CloseHandle( hIOCP );
	}
	hIOCP = INVALID_HANDLE_VALUE;
	for( size_t i = 0; i < m_impl->WorkThreadVec.size(); ++i )
	{
		HANDLE hExit = m_impl->WorkThreadVec[i];
		CloseHandle( hExit );
	}
	m_impl->WorkThreadVec.clear();
}

iocp::operator HANDLE()
{
	return m_impl->hIOCP;
}

unsigned __stdcall iocp::default_work( LPVOID lpParam )
{
	iocp* pTHIS = reinterpret_cast< iocp* >( lpParam );
	if( !pTHIS )
	{
		return -1;
	}

	DWORD			dwNumberBytes = 0;
	LPOVERLAPPED	lpEvent = NULL;
	ICompletionKey*	lpKey	= NULL;
	BOOL			bSuccess = FALSE;
	HANDLE			hIOCP	= *pTHIS;
	while( TRUE )
	{
		dwNumberBytes	= 0;
		lpEvent			= NULL;
		lpKey			= NULL;

		//try
		{
			bSuccess = GetQueuedCompletionStatus(
				hIOCP,
				&dwNumberBytes,
				(PULONG_PTR)&lpKey,
				&lpEvent,
				INFINITE
				);

			// 接收数据错误，或者完成句柄被关闭，则跳出主循环
			if( dwNumberBytes == -1 )	break;

			iocpEvent* pEvent = static_cast< iocpEvent* >( lpEvent );
			if( bSuccess == FALSE )
			{
				DWORD dwErr = GetLastError();
				if( lpKey )
				{ 
					lpKey->AddRef();
				}
				if( pEvent )
				{
					//case ERROR_SUCCESS:
					//case WAIT_TIMEOUT:
					//case ERROR_CONNECTION_ABORTED:
					//case ERROR_NETNAME_DELETED:
					//case ERROR_OPERATION_ABORTED:
					try
					{
						pEvent->OnError( lpKey, dwNumberBytes, dwErr );
						//helper::CErrorDescribe err;
						//CLogger::GetInstance( _T("xservice") )->WriteLog( "%s[%d]", err.GetDescribe(), dwErr );
					}
					catch( iocp_exception* e )
					{
						if( e )
						{
							e->do_exception();
							delete e;
						}
					}
					pEvent->Release();
				}
				if( lpKey && lpKey->Release() == 1 )
				{
					pTHIS->disassociat( lpKey->Get() );
				}
			}
			else
			{
				if( lpKey )
				{
					lpKey->AddRef();
					HANDLE hKey = lpKey->Get();
					try
					{
						lpKey->OnEvent( pEvent, dwNumberBytes );
					}
					catch( iocp_exception* e )
					{
						if( e )
						{
							e->do_exception();
							delete e;
						}
					}

					if( pEvent )
					{
						pEvent->Release();
					}
					else
					{
						ASSERT( FALSE );
					}

					if( lpKey->Release() == 1 )
					{
						pTHIS->disassociat( hKey );
					}
				}
				else if( pEvent )
				{
					pEvent->OnEvent( lpKey, dwNumberBytes );
					pEvent->Release();
				}
			}
		}
	}
	return 0;
}
