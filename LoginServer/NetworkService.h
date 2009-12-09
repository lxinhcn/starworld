#pragma once
#define LOGON_MESSAGE_TYPE 0x10
typedef struct
{
	_uint32 id;
	_uint32 order;
}userdata, *userdata_ptr;
class CNetworkService : public message_dispatcher< CNetworkService >
{
	IMPLEMENT_DISPATCHER_TABLE( message_type )
	IMPLEMENT_DISPATCHER_TABLE( system_code )
	IMPLEMENT_DISPATCHER_TABLE( server_code )
private:
	uintptr_t		m_thread_h;
	_uint16			m_nPort;
	bool			m_bWork;

public:
	CNetworkService( _uint16 nPort );
	~CNetworkService(void);

	void Process( const char* data, size_t size );

protected:
	static unsigned int __stdcall Svc( LPVOID pParam );
	void OnSystemMessage( const char* data, size_t size );
	void OnServerMessage( const char* data, size_t size );

	//////////////////////////////////////////////////////////////////////////
	// System message
	//////////////////////////////////////////////////////////////////////////
	void OnNetworkAccept( const char* data, size_t size );
	void OnNetworkClose( const char* data, size_t size );

	//////////////////////////////////////////////////////////////////////////
	// Logic message
	//////////////////////////////////////////////////////////////////////////
	void OnRegistRequest( const char *data, size_t size );
	void OnLogonRequest( const char *data, size_t size );
};
