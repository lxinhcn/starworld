#pragma once
#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

#include <WinSock2.h>
class TOOLLIB_API CNetEvent;
class TOOLLIB_API ISocketBase
{
	friend class CNetEvent;
public:
	operator SOCKET()const{ return s; }
	SOCKET operator=( SOCKET sock ){ s = sock; return s; }
private:
	virtual void OnAccept()	= 0;
	virtual void OnRead()	= 0;
	virtual void OnWrite()	= 0;
	virtual void OnClose()	= 0;
	virtual void OnConnect()= 0;

	virtual void Release()	= 0;
private:
	SOCKET	s;
	LONG	hHandle;
};

/*
	由于EventSelect 模型是一种被动数据接收的模型，所以需要一条
	线程来管理这些已经成为该模型内的套接字。如此，我们不能直接
	使用socket而是间接的加了一个Handle对象来标示已被托管的套接
	字。
*/
class TOOLLIB_API CNetEvent
{
public:
	CNetEvent( LONG nMaxSocketCount = 64 );
	~CNetEvent(void);

	BOOL	Connect( LPCSTR lpszHost, int nPort, DWORD dwTimeOut = 100 );
	VOID	Disconnect();
	INT		Send( const char* data, int len );
	bool	IsConnected()const { return m_bConnect; }

	bool	Start( int nPort );
	void	Stop();

protected:
	LONG	CreateSocketHandle( SOCKET s = INVALID_SOCKET );
	BOOL	CloseSocketHandle( LONG hHandle );
	BOOL	IsWork()const{ return m_bWork; }
	bool	IsValidSocketHandle( LONG hHandle )const{ return hHandle >= 0 && hHandle < m_nMaxSocketCount; }

	virtual ISocketBase* AllocClient() = 0;

	static unsigned int __stdcall EventThreadLoop( LPVOID lParam );

private:
	SOCKET*			m_pSocketArray;		// socket	数组
	WSAEVENT*		m_pEventArray;		// Event	数组
	ISocketBase**	m_pClientArray;		// Client	数组
	LONG			m_nMaxSocketCount;	// 最大的Socket连接数量
	LONG			m_nListenIndex;		// 服务器监听端口的索引号
	volatile LONG	m_nTotalNet;		// 当前连接的数量。
	bool			m_bConnect;			// 是否连接

	HANDLE			m_hThread;			// 工作线程句柄
	volatile LONG	m_bWork;			// 是否在工作。
	CCritical		m_Lock;				// 共享锁
};
