#include "StdAfx.h"
#include "NetworkService.h"
#include <process.h>

BEGIN_DISPATCHER_TABLE( CNetworkService, message_type )
	DECLARE_DISPATCH( SYSTEM_MESSAGE_TYPE, OnSystemMessage )
	//DECLARE_DISPATCH( MMO_LOGON_CLIENT_MESSAGE_TYPE, OnServerMessage )
END_DISPATCHER_TABLE( CNetworkService, message_type )

BEGIN_DISPATCHER_TABLE( CNetworkService, system_code )
	DECLARE_DISPATCH( EVENT_ACCEPT, OnNetworkAccept )
	DECLARE_DISPATCH( EVENT_CLOSE, OnNetworkClose )
END_DISPATCHER_TABLE( CNetworkService, system_code )
	
BEGIN_DISPATCHER_TABLE( CNetworkService, server_code )
	//DECLARE_DISPATCH( C2S_REGIST_REQUEST, OnRegistRequest )
	//DECLARE_DISPATCH( C2S_LOGON_REQUEST, OnLogonRequest )
END_DISPATCHER_TABLE( CNetworkService, server_code )

CNetworkService::CNetworkService( _uint16 nPort )
: m_nPort( nPort )
, m_bWork( false )
{
	for( int i = 0; i < _countof(database); ++i )
	{
		database[i].start( "Driver=SQL Server;Server=Albert\\SQLEXPRESS;UID=Albert;PWD=winner;Database=LogonDB" );
	}

	m_thread_h = _beginthreadex( NULL, 0, Svc, (void*)this, 0, NULL );
}

CNetworkService::~CNetworkService(void)
{
	m_bWork = false;
	WaitForSingleObject( (HANDLE)m_thread_h, INFINITE );
	for( int i = 0; i < _countof(database); ++i )
	{
		database[i].stop();
	}
}

__declspec( thread ) _uint32		network_h = -1;
__declspec( thread ) userdata_ptr	puserdata = 0;
unsigned int __stdcall CNetworkService::Svc( _lpvoid pParam )
{
	IMessageQueue	*message_queue_ptr = NULL;
	CNetworkService *pService = (CNetworkService*)pParam;

	long_ptr server_h = StartServer( "0.0.0.0", pService->m_nPort, &message_queue_ptr, 0 );

	pService->m_bWork = true;
	INetPacket *pPacket = NULL;
	while( pService->m_bWork )
	{
		pPacket = message_queue_ptr->PopMessage();
		if( pPacket )
		{

			network_h = pPacket->handle();
			puserdata = (userdata_ptr)pPacket->userdata();

			if( puserdata == 0 )
			{
				long_ptr param[1] = { (long_ptr)&network_h };
				ExecuteState( Operator_GetUserdata, (long_ptr)param );
				puserdata = (userdata_ptr)param[0];
			}

			// 处理数据
			pService->Process( pPacket->data(), pPacket->size() );

			pPacket->release();
		}
	}

	CloseServer( server_h );
	message_queue_ptr->Release();
	return 0;
}

void CNetworkService::Process( const char* data, size_t size )
{
	unsigned char type = *data;
	func pFunc = DISPATCHER_GET( message_type, type );
	if( pFunc )
		return (this->*pFunc)( data+1, size-1 );
}

void CNetworkService::OnSystemMessage( const char* data, size_t size )
{
	unsigned char type = *data;
	func pFunc = DISPATCHER_GET( system_code, type );
	if( pFunc )
		return (this->*pFunc)( data+1, size-1 );
}

void CNetworkService::OnServerMessage( const char* data, size_t size )
{
	unsigned char type = *data;
	func pFunc = DISPATCHER_GET( server_code, type );
	if( pFunc )
		return (this->*pFunc)( data+1, size-1 );
}

//////////////////////////////////////////////////////////////////////////
// 系统消息
//////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------//
//	created:	8:12:2009   17:56
//	filename: 	NetworkService
//	author:		Albert.xu
//
//	purpose:	用户建立连接
//--------------------------------------------------------//
void CNetworkService::OnNetworkAccept( const char* data, size_t size )
{
	userdata_ptr pud = new userdata();
	pud->id = 0;
	pud->order = 0;
	long_ptr param[2] = { (long_ptr)&network_h, (long_ptr)pud };
	ExecuteState( Operator_SetUserdata, (long_ptr)param );
}

//--------------------------------------------------------//
//	created:	8:12:2009   17:56
//	filename: 	NetworkService
//	author:		Albert.xu
//
//	purpose:	用户断开连接
//--------------------------------------------------------//
void CNetworkService::OnNetworkClose( const char* data, size_t size )
{
	long_ptr param[1] = { (long_ptr)&network_h };
	ExecuteState( Operator_GetUserdata, (long_ptr)param );
	userdata_ptr ud = (userdata_ptr)param[0];
	delete ud;
}

//////////////////////////////////////////////////////////////////////////
// 服务器逻辑消息
//////////////////////////////////////////////////////////////////////////
void CNetworkService::OnRegistRequest( const char *data, size_t size )
{
	//MMO_RegistRequest msg;
	//bufstream bsf( data, size, 0 );
	//bsf >> msg;
}

void CNetworkService::OnLogonRequest( const char *data, size_t size )
{

}
