#pragma  once
#ifndef _NETBASE_H
#define _NETBASE_H

#include "defines.h"
//#define _WIN32_WINNT 0x0601

#ifdef NETBASE_EXPORTS
#define NETBASE_API __declspec(dllexport)
#else
#define NETBASE_API __declspec(dllimport)
#endif

//网络通讯包相关
#define NETBASE								_T("Net")
#define MAX_PACKET_SIZE						(1024*4)
#define INVALID_NETWORK_HANDLE				(-1)
namespace XGC
{
	namespace net
	{
#pragma pack(1)

		//消息头定义
		struct MessageHeader
		{
			unsigned char	type;
			unsigned char	code;
		};

#pragma pack()

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		#define SYSTEM_MESSAGE_TYPE	0
		#define EVENT_ACCEPT	0
		#define EVENT_CLOSE		1
		#define EVENT_CONNECT	2
		#define EVENT_ERROR		3
		#define EVENT_PING		4
		#define EVENT_PONG		5

		typedef size_t	network_t;
		typedef size_t	group_t;

		// return 成功 返回新的组ID -1 没有可用的组编号
		struct Param_NewGroup
		{
			network_t self_handle; 
		};
		#define Operator_NewGroup		0	

		// reutrn 0 成功 -1 指定的组已不存在 -2 网络句柄已被添加 -3 添加句柄到组失败
		struct Param_EnterGroup
		{
			group_t		group;
			network_t	handle;
		};
		#define Operator_EnterGroup		1	

		// return 0 成功 -1 指定的组已不存在 -2 网络句柄不存在
		struct Param_LeaveGroup
		{
			group_t		group;
			network_t	handle;
		};
		#define Operator_LeaveGroup		2	

		// return 0 成功 -1 指定的组已不存在
		struct Param_RemoveGroup
		{
			group_t group;
		};
		#define Operator_RemoveGroup	3	

		// return 0 成功 -1 失败
		struct Param_GetUserdata
		{
			network_t	handle;
			long_ptr	userdata_ptr;
		};
		#define Operator_GetUserdata	4

		// return 0 成功 -1 失败
		struct Param_SetUserdata
		{
			network_t	handle;
			long_ptr	userdata_ptr;
		};
		#define Operator_SetUserdata	5	

		// return 0 成功 -1 失败
		struct Param_SetTimeout
		{
			network_t handle;
			unsigned int sec;
			unsigned int msec;
		};
		#define Operator_SetTimeout		6

		// param [in]	in = { _uint32 handle, _uint32 type }; type 0 - local_ip, 1 - local_port, 2 - remont_ip, 3 - remont_port, 4 - connect_time
		// return -1 未找到指定的连接， -2 获取信息时出错
		struct Param_QueryHandleInfo
		{
			network_t	handle;
			short		mask;
			byte_t		data[64];
		};
		#define NET_LOCAL_ADDRESS	1
		#define NET_REMOT_ADDRESS	2
		#define NET_LOCAL_PING		4
		#define Operator_QueryHandleInfo	7

		struct __declspec(novtable) INetPacket
		{
			virtual _lpcstr		data()const = 0;
			virtual size_t		size()const = 0;
			virtual size_t		capacity()const = 0;
			virtual network_t	handle()const = 0;
			virtual long_ptr	userdata()const = 0;
			virtual void		release() = 0;
		};

		struct __declspec(novtable) IMessageQueue
		{
			virtual INetPacket* PickMessage() = 0;
			virtual INetPacket* PopMessage() = 0;
			virtual void PushMessage( INetPacket* ) = 0;
			virtual void Release() = 0;
		};

		struct __declspec(novtable) INetworkHandler
		{
			INetworkHandler()
				: m_pUserdata( NULL )
				, m_nLinkHandle( -1 )
			{

			}

			///function	:	连接关闭时的处理函数。重载此函数时需在做完应用需要做的事情后将连接删除或重置
			///param	:	无
			///return	:	0	成功	-1 失败
			virtual int OnClose( void );

			///function	:	新的远端连接成功时的处理函数
			///param	:	new_handle	新连接的套接字
			///return	:	0	成功	-1 失败
			virtual int OnAccept( _uint32 handle );

			///function	:	成功接受到网络数据包后提交到iocp上层应用处理的函数
			///param	:	data	接受到的数据	在中len范围内可靠有效
			///param	:	len		接受到数据的长度	此数据由iocp保证其真实性。
			///return	:	0	成功	-1 失败
			virtual int OnRecv( INetPacket *pPacket );

			///function	:	成功发送网络数据包后给iocp上层处理的函数，目前暂时未使用
			///param	:	data	成功发送的数据
			///param	:	len		成功发送的数据长度
			///return	:	0	成功	-1 失败
			virtual int OnSend( INetPacket *pPacket );

			///function	:	iocp捕获到错误并提供给应用的处理机会
			///param	:	error	错误代码
			///return	:	0	成功	-1 失败
			virtual int OnError( int error ) = 0;

			//--------------------------------------------------------//
			//	created:	26:1:2010   13:02
			//	filename: 	Netbase
			//	author:		Albert.xu
			//
			//	purpose:	用户数据
			//--------------------------------------------------------//
			long_ptr GetUserdata()const{ return m_pUserdata; }

			//--------------------------------------------------------//
			//	created:	26:1:2010   13:03
			//	filename: 	Netbase
			//	author:		Albert.xu
			//
			//	purpose:	 设置用户数据
			//--------------------------------------------------------//
			void SetUserdata( long_ptr pUserdata ){ m_pUserdata = pUserdata; }

		protected:
			_uint32		m_nLinkHandle;
			long_ptr	m_pUserdata;
		};

		typedef INetworkHandler* (*create_handler_func)( long_ptr lParam );
		typedef int (*dispatchFunctor)( network_t, const unsigned char*, size_t );

		enum eNetLibrary{ asio };
		extern "C"
		{
			NETBASE_API extern bool (*InitNetwork)();
			NETBASE_API extern void (*FiniNetwork)();
			NETBASE_API extern long_ptr (*StartServer)( _lpcstr address, _uint16 port, IMessageQueue** msg_queue_ptr, _uint32 timeout );
			NETBASE_API extern long_ptr (*StartServerEx)( _lpcstr address, _uint16 port, _uint32 timeout, create_handler_func call, long_ptr param );
			NETBASE_API extern void (*CloseServer)( long_ptr server_h );
			NETBASE_API extern bool (*ConnectServer)( _lpcstr address, _uint16 port, IMessageQueue** msg_queue, _uint32 time_out );
			NETBASE_API extern bool (*AsyncConnectServer)( _lpcstr address, _uint16 port, IMessageQueue** msg_queue, _uint32 time_out );
			NETBASE_API extern void*(*AllocPacket)( size_t size );
			NETBASE_API extern void (*SendPacket)( size_t handle, _lpcstr data, size_t size );
			NETBASE_API extern void (*SendLastPacket)( size_t handle, _lpcstr data, size_t size );
			NETBASE_API extern void (*SendPackets)( size_t *handle, _uint32 count, _lpcstr data, size_t size );
			NETBASE_API extern void (*SendToGroup)( _uint32 group, _lpcstr data, size_t size, bool toself );
			NETBASE_API extern void (*CloseLink)( size_t handle );
			NETBASE_API extern long (*ExecuteState)( _uint32 operate_code, long_ptr param );// 返回 0 表示成功, < 0 表示发生错误 > 0 表示可以正常执行,但有其他问题.

			NETBASE_API extern bool (*RegistFunction)( byte_t type, byte_t id, dispatchFunctor fn );
			NETBASE_API extern bool (*UnRegistFunction)( byte_t type, byte_t id );
			NETBASE_API extern int  (*DispatchMessage)( IMessageQueue* msg_queue );
			//--------------------------------------------------------//
			//	created:	3:12:2009   14:27
			//	filename: 	net
			//	author:		Albert.xu
			//
			//	purpose:	加载网络库
			//--------------------------------------------------------//
			NETBASE_API bool CreateNetwork( eNetLibrary );

			//--------------------------------------------------------//
			//	created:	3:12:2009   14:27
			//	filename: 	net
			//	author:		Albert.xu
			//
			//	purpose:	释放网络库
			//--------------------------------------------------------//
			NETBASE_API void DestroyNetwork();
		};
	}
};
#endif // _NETBASE_H