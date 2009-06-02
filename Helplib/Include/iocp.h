#pragma once

#include <vector>
#include <queue>
#include <map>
#include <WinSock2.h>
#include <MSWSock.h>

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

#define iocpEvent iocp_event
class iocp_event;
class ICompletionKey
{
public:
	//--------------------------------------------------------------------------------------------------------//
	// 当一个投递完成时调用
	// lpEvent			:	事件对象指针
	// dwNumberOfByte	:	有所少数据被操作
	// return			:	是否释放Key
	//--------------------------------------------------------------------------------------------------------//
	virtual VOID OnEvent( iocpEvent* lpEvent, DWORD dwNumberOfByte ) = 0;

	//--------------------------------------------------------------------------------------------------------//
	// 引用计数
	//--------------------------------------------------------------------------------------------------------//
	virtual inline LONG AddRef() = 0;

	//--------------------------------------------------------------------------------------------------------//
	// 释放引用计数
	//--------------------------------------------------------------------------------------------------------//
	virtual inline LONG Release() = 0;

	//--------------------------------------------------------------------------------------------------------//
	// 得到句柄
	//--------------------------------------------------------------------------------------------------------//
	virtual inline HANDLE Get() = 0;

	//--------------------------------------------------------------------------------------------------------//
	// 关闭句柄
	//--------------------------------------------------------------------------------------------------------//
	virtual inline void Close() = 0;

};

//--------------------------------------------------------------------------------------------------------//
// 事件接口
//--------------------------------------------------------------------------------------------------------//
class TOOLLIB_API iocp_event	:	protected OVERLAPPED
{
public:
	explicit iocp_event( ICompletionKey* pKey, bool bEvent );
	virtual ~iocp_event();

	//--------------------------------------------------------------------------------------------------------//
	// 这里要是为了防止强制类型转换引发的问题
	//--------------------------------------------------------------------------------------------------------//
	operator LPOVERLAPPED() { return static_cast< OVERLAPPED* >( this ); }

	LPOVERLAPPED GetOverlapped() { return static_cast< LPOVERLAPPED >( this ); }

	//--------------------------------------------------------------------------------------------------------//
	// 当该投递完成时调用
	// pKey				:	哪个客户端的投递
	// dwNumberOfByte	:	有多少数据被操作
	//--------------------------------------------------------------------------------------------------------//
	virtual void OnEvent( ICompletionKey* pKey, DWORD dwNumberOfByte ) = 0;
	virtual bool OnError( ICompletionKey* pKey, DWORD dwNumberOfByte, DWORD dwErrorCode ) = 0;
	virtual void Release() = 0;

private:
	ICompletionKey* m_pKey;
};

//--------------------------------------------------------------------------------------------------------//
// 完成键
//--------------------------------------------------------------------------------------------------------//
class iocp_handle
{
public:
	virtual operator HANDLE()const = 0;
	virtual void close() = 0;

protected:
	bool closed;
};

class iocp_file	:	public iocp_handle
{
public:
	iocp_file( HANDLE handle ) : m_handle( handle ){ closed = false; }
	iocp_file( const iocp_file& handle ) : m_handle( handle.m_handle ){ closed = false; }

	virtual operator HANDLE()const { return m_handle; }
	HANDLE operator=( const HANDLE handle ) { m_handle = handle; return *this; }
	HANDLE operator=( const iocp_file& handle ) { m_handle = handle.m_handle; return *this; }

	void close()
	{ 
		if( !closed )
		{ 
			closed = true;
			CloseHandle( m_handle ); 
		}
	}

private:
	HANDLE m_handle;
};

class iocp_socket	:	public iocp_handle
{
public:
	iocp_socket( SOCKET handle ) : m_handle( handle ){ closed = false; }
	iocp_socket( const iocp_socket& handle ) : m_handle( handle.m_handle ){ closed = false; }

	operator SOCKET()const { return m_handle; }
	virtual operator HANDLE()const { return (HANDLE)m_handle; }
	SOCKET operator=( const SOCKET handle ) { m_handle = handle; return *this; }
	SOCKET operator=( const iocp_socket& handle ) { m_handle = handle.m_handle; return *this; }

	void close()
	{ 
		if( !closed )
		{
			closed = true;
			CancelIo( (HANDLE)m_handle );
			shutdown( m_handle, 0 );
			closesocket( m_handle );
		} 
	}

private:
	SOCKET m_handle;
};

template< class _HANDLE_T = iocp_file >
class CCompletionKey	: public ICompletionKey
{
public:
	CCompletionKey( CONST _HANDLE_T& hFile )
	: m_hFile( hFile )
	, m_ref( 1 )
	{
		m_data_vec.resize( 16, 0 );
	}

	virtual ~CCompletionKey()
	{
		Close();
	}

	//--------------------------------------------------------------------------------------------------------//
	// 当一个投递完成时调用
	// lpEvent			:	事件对象指针
	// dwNumberOfByte	:	有所少数据被操作
	// return			:	是否释放Key
	//--------------------------------------------------------------------------------------------------------//
	virtual VOID OnEvent( iocpEvent* lpEvent, DWORD dwNumberOfByte )
	{
		if( lpEvent )
		{
			return lpEvent->OnEvent( this, dwNumberOfByte );
		}
	}

	//--------------------------------------------------------------------------------------------------------//
	// 取得数据的引用
	// index	:	索引
	// remark	;
	/*
		索引范围要求在0-15之间，因为Key只提供了16个数据存放区域。
	*/
	//--------------------------------------------------------------------------------------------------------//
	inline DWORD_PTR& operator[]( int index )
	{
		return m_data_vec[index];
	}

	inline DWORD_PTR& at( int index )
	{
		return m_data_vec[index];
	}

	//--------------------------------------------------------------------------------------------------------//
	// 付值操作符
	//--------------------------------------------------------------------------------------------------------//
	inline void set_handle( CONST _HANDLE_T& handle )
	{
		m_hFile = handle;
	}

	//--------------------------------------------------------------------------------------------------------//
	// 得到句柄
	//--------------------------------------------------------------------------------------------------------//
	virtual inline HANDLE Get()
	{
		return m_hFile;
	}

	inline CONST _HANDLE_T& get_handle()const
	{
		return m_hFile;
	}

	//--------------------------------------------------------------------------------------------------------//
	// 关闭句柄
	//--------------------------------------------------------------------------------------------------------//
	inline void Close()
	{
		m_hFile.close();
	}

	//--------------------------------------------------------------------------------------------------------//
	// 引用计数
	//--------------------------------------------------------------------------------------------------------//
	inline LONG AddRef()
	{
		return InterlockedIncrement( &m_ref );
	}

	//--------------------------------------------------------------------------------------------------------//
	// 释放引用计数
	//--------------------------------------------------------------------------------------------------------//
	inline LONG Release()
	{
		ASSERT( m_ref != 0 );
		LONG ref = InterlockedDecrement( &m_ref );
		if( ref == 0 )
		{
			delete this;
		}
		return ref;
	}

protected:
	typedef std::vector< DWORD_PTR >	CDataVec;

	_HANDLE_T	m_hFile;
	CDataVec	m_data_vec;
	volatile LONG	m_ref;
};

//--------------------------------------------------------------------------------------------------------//
// 提供一个缓冲区管理的模版
//--------------------------------------------------------------------------------------------------------//
template< int Type, int BufferSize >
class iocpEventTemplate	:	public iocpEvent
{
public:
	enum { enType = Type }; // int to type

	explicit iocpEventTemplate( ICompletionKey* pKey, bool bEvent = false )
	: iocpEvent( pKey, bEvent )
	, m_buffer( new char[BufferSize] )
	, m_buffer_size( BufferSize )
	, m_cur_size( 0 )
	{
	}

	~iocpEventTemplate()
	{
		m_cur_size = 0;
		m_buffer_size = 0;
		delete[] m_buffer;
		m_buffer = 0;
	}

	//--------------------------------------------------------------------------------------------------------//
	// 将数据推入缓冲区
	// buf		:	数据首址
	// sizxe	:	数据长度
	// return	:	已经读入的数据长度
	//--------------------------------------------------------------------------------------------------------//
	size_t push( const char* buf, size_t size )
	{
		size_t read_size = min( m_buffer_size - m_cur_size, size );
		char* dest = m_buffer + m_cur_size;
		memcpy( dest, buf, read_size );
		m_cur_size += read_size;
		if( isdone() )
		{
			on_buffer_full();
		}
		return read_size;
	}

	//--------------------------------------------------------------------------------------------------------//
	// 得到缓冲区首址
	//--------------------------------------------------------------------------------------------------------//
	char* get()const
	{
		return m_buffer;
	}

	//--------------------------------------------------------------------------------------------------------//
	// 得到当前数据长度
	// return	:	当前数据长度
	//--------------------------------------------------------------------------------------------------------//
	size_t lenth()const
	{ 
		return m_cur_size;
	}

	//--------------------------------------------------------------------------------------------------------//
	// 得到缓冲区大小
	//--------------------------------------------------------------------------------------------------------//
	size_t size()const
	{
		return m_buffer_size;
	}

	//--------------------------------------------------------------------------------------------------------//
	// 缓冲区是否填充满
	//--------------------------------------------------------------------------------------------------------//
	virtual bool isdone()const
	{ 
		return m_cur_size == m_buffer_size;
	}

	//--------------------------------------------------------------------------------------------------------//
	// 重置缓冲区指针到缓冲头
	//--------------------------------------------------------------------------------------------------------//
	void reset()
	{ 
		m_cur_size = 0;
	}

	// 偏移缓冲区指针
	size_t offset_pos( size_t offset )
	{
		size_t move = min( offset, m_buffer_size - m_cur_size );
		m_cur_size += move;
		return move;
	}

	//--------------------------------------------------------------------------------------------------------//
	// 缓冲区满通知
	//--------------------------------------------------------------------------------------------------------//
	virtual void on_buffer_full(){}
	
	virtual void Release()
	{
		delete this;
	}
private:
	char*	m_buffer;		// 缓冲区指针
	size_t	m_cur_size;		// 当前填充量
	size_t	m_buffer_size;	// 缓冲区大小
};

class TOOLLIB_API iocp
{
public:
	iocp(void);
	virtual ~iocp(void);

	typedef bool EnumComlationKey( HANDLE hHandle, ICompletionKey* pKey, LPVOID lpUserData );
	typedef unsigned __stdcall work( LPVOID Param );
	//--------------------------------------------------------------------------------------------------------//
	// 初始化完成端口
	// return	:	true 初始化成功, flase 初始化失败
	//--------------------------------------------------------------------------------------------------------//
	bool initialize();

	//--------------------------------------------------------------------------------------------------------//
	// 关闭完成端口
	//--------------------------------------------------------------------------------------------------------//
	void uninitialize();
	
	//--------------------------------------------------------------------------------------------------------//
	// 将handle关联到完成端口
	// handle	： 被关联的完成端口
	// pKey		： 完成键，每个handle有一个完成键
	// NumberOfConcurrentThreads	：	这个完成端口上最多允许同时存在的线程数
	//--------------------------------------------------------------------------------------------------------//
	bool associat( HANDLE handle, ICompletionKey* pKey = NULL, DWORD NumberOfConcurrentThreads  = 0 );

	//--------------------------------------------------------------------------------------------------------//
	// 解除handle的关联
	// handle	:	被关联的句柄
	// return	:	和handle对应的Key的指针
	//--------------------------------------------------------------------------------------------------------//
	ICompletionKey* disassociat( HANDLE handle );

	//--------------------------------------------------------------------------------------------------------//
	// 查询handle的关联 会有AddRef 引用操作。
	// handle	:	被关联的句柄
	// return	:	和handle对应的Key的指针
	//--------------------------------------------------------------------------------------------------------//
	ICompletionKey* query( HANDLE handle );

	//--------------------------------------------------------------------------------------------------------//
	// 枚举当前的完成端口句柄和完成键
	// FuncCallback	:	回调函数指针
	// lpData		:	用户数据
	// return		:	是否继续	true - 继续	false - 终止
	//--------------------------------------------------------------------------------------------------------//
	HANDLE enum_key( EnumComlationKey FuncCallback, LPVOID lpData );
	
	//--------------------------------------------------------------------------------------------------------//
	// 开始工作线程
	// dwNumberOfThread	:	工作线程数量 default cpu个数 * 2 + 2
	// pfn				:	工作线程函数
	// lpParam			:	工作线程参数
	//--------------------------------------------------------------------------------------------------------//
	bool start_iocp( DWORD dwNumberOfThread = 0, work pfn = default_work, LPVOID lpParam = NULL );

	//--------------------------------------------------------------------------------------------------------//
	// 停止完成端口
	//--------------------------------------------------------------------------------------------------------//
	void stop_iocp();

	static unsigned __stdcall default_work( LPVOID lpParam );
	operator HANDLE();

public:
	enum worktype { TypeSingle, TypeMultiple }; // 工作方式， 现在有单线，和多线两种方式。
	struct Event
	{
		iocpEvent*	pEvent;
		DWORD		dwNumberOfBytes;
	};

private:
	typedef std::vector< HANDLE >	CWorkThreadVec;
	typedef std::queue< Event* >	CEventQueue;
	typedef std::map< HANDLE, ICompletionKey* >	CKeyMap;

	struct impl;
	impl* m_impl;
};

class TOOLLIB_API iocp_exception
{
public:
	iocp_exception(){}
	virtual ~iocp_exception(){}
	virtual void do_exception() = 0;
};