#pragma once

typedef struct
{
	_uint32 id;
	_uint32 order;
}userdata, *userdata_ptr;

class CNetworkService :	public message_dispatcher< CNetworkService >
{
	IMPLEMENT_DISPATCHER_TABLE( message_type )
	IMPLEMENT_DISPATCHER_TABLE( system_code )
	IMPLEMENT_DISPATCHER_TABLE( server_code )

public:
	CNetworkService(void);
	~CNetworkService(void);

	void Start();
	void Stop();
protected:
	void Process( const char* data, size_t size );

	static unsigned int __stdcall Svc( _lpvoid pParam );

	void OnSystemMessage( const char* data, size_t size );
	void OnServerMessage( const char* data, size_t size );

	//////////////////////////////////////////////////////////////////////////
	// System message
	//////////////////////////////////////////////////////////////////////////
	void OnNetworkConnect( const char* data, size_t size );
	void OnNetworkClose( const char* data, size_t size );

private:
	uintptr_t		m_thread_h;
	_uint16			m_nPort;
	bool			m_bWork;
};

