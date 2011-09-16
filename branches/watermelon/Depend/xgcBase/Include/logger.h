#pragma once
#ifdef EXPORT_API
#define EXPORT_LIB __declspec(dllexport)
#else
#define EXPORT_LIB __declspec(dllimport)
#endif


#ifndef _LOGGER_H
#define _LOGGER_H
#pragma warning( disable:4251 )
#include "defines.h"

namespace XGC
{
	namespace common
	{
		class CLoggerAdapter;

		class EXPORT_LIB CLogger
		{
		private:
			CLogger( _lpctstr szLoggerName );
			~CLogger(void);

		public:
			enum logger_mode	{ normal, pause, ignore };
			enum logger_level	{ debug, trace, warning, error, };
			enum header			{ header_date, header_time, header_thread, header_name, header_count, };

			//--------------------------------------------------------------------------------------------------------//
			// 取得日志实例
			// szLogger		:	实例名
			//--------------------------------------------------------------------------------------------------------//
			static CLogger* GetInstance( _lpctstr szLogger );

			static int WriteDateWidgetW( CLogger* pLogger, _lpwstr szBuffer, size_t nCount );
			static int WriteTimeWidgetW( CLogger* pLogger, _lpwstr szBuffer, size_t nCount );
			static int WriteNameWidgetW( CLogger* pLogger, _lpwstr szBuffer, size_t nCount );
			static int WriteCountWidgetW( CLogger* pLogger, _lpwstr szBuffer, size_t nCount );
			static int WriteThreadWidgetW( CLogger* pLogger, _lpwstr szBuffer, size_t nCount );

			static int WriteDateWidgetA( CLogger* pLogger, _lpstr szBuffer, size_t nCount );
			static int WriteTimeWidgetA( CLogger* pLogger, _lpstr szBuffer, size_t nCount );
			static int WriteNameWidgetA( CLogger* pLogger, _lpstr szBuffer, size_t nCount );
			static int WriteCountWidgetA( CLogger* pLogger, _lpstr szBuffer, size_t nCount );
			static int WriteThreadWidgetA( CLogger* pLogger, _lpstr szBuffer, size_t nCount );

			//--------------------------------------------------------------------------------------------------------//
			// 设置日志输出模式
			//--------------------------------------------------------------------------------------------------------//
			static VOID SetLoggerMode( logger_mode en_mode );

			//--------------------------------------------------------------------------------------------------------//
			// 日志实例是否存在
			// szLogger		:	实例名
			//--------------------------------------------------------------------------------------------------------//
			static bool	Have( _lpctstr szLogger );

			//--------------------------------------------------------------------------------------------------------//
			// 清除日至
			// szLogger		:	实例名
			//--------------------------------------------------------------------------------------------------------//
			static void Clear( _lpctstr szLogger );

			//--------------------------------------------------------------------------------------------------------//
			// 清除所有日至
			//--------------------------------------------------------------------------------------------------------//
			static void ClearAll();

			//--------------------------------------------------------------------------------------------------------//
			// 添加一个日至适配器
			// pAdapter		:	适配器指针
			//--------------------------------------------------------------------------------------------------------//
			bool AddAdapter( CLoggerAdapter* pAdapter, bool bCaptrueDebugString = false );

			//--------------------------------------------------------------------------------------------------------//
			// 删除一个适配器
			// szAdapterName	:	适配器名
			//--------------------------------------------------------------------------------------------------------//
			CLoggerAdapter* RemoveAdapter( _lpctstr szAdapterName );

			//---------------------------------------------------//
			// [3/4/2011 Albert]
			// Description:	获取日志格式 
			//---------------------------------------------------//
			int GetHeadW( _lpwstr szFmt, size_t nCount );


			//---------------------------------------------------//
			// [3/4/2011 Albert]
			// Description:	获取日志格式 
			//---------------------------------------------------//
			int GetHeadA( _lpstr szFmt, size_t nCount );

			//---------------------------------------------------//
			// [3/4/2011 Albert]
			// Description:	获取日志格式 
			//---------------------------------------------------//
			int GetHead( _lptstr szFmt, size_t nCount );

			//--------------------------------------------------------------------------------------------------------//
			// 写日至
			// szBuffer		:	日至缓冲区，由用户提供
			// size			:	缓冲区大小
			// fmt			:	格式串
			// ...			:	参数列表
			// return		:	写入的字符数
			//--------------------------------------------------------------------------------------------------------//
			int WriteLogW( _lpwstr szBuffer, size_t nSize, _lpcwstr szFmt, ... );

			//--------------------------------------------------------------------------------------------------------//
			// 写日至
			// szBuffer		:	日至缓冲区，由用户提供
			// ...			:	参数列表
			// return		:	写入的字符数
			// remark		:	小心，不能写过长的字符，4096个字符刚好。
			//--------------------------------------------------------------------------------------------------------//
			int WriteLogW( _lpcwstr szFmt, ... );

			//--------------------------------------------------------------------------------------------------------//
			// 写日至
			// szBuffer		:	日至缓冲区，由用户提供
			// size			:	缓冲区大小
			// fmt			:	格式串
			// ...			:	参数列表
			// return		:	写入的字符数
			//--------------------------------------------------------------------------------------------------------//
			int WriteLogA( _lpstr szBuffer, size_t nSize, _lpcstr szFmt, ... );

			//--------------------------------------------------------------------------------------------------------//
			// 写日至
			// szBuffer		:	日至缓冲区，由用户提供
			// ...			:	参数列表
			// return		:	写入的字符数
			// remark		:	小心，不能写过长的字符，4096个字符刚好。
			//--------------------------------------------------------------------------------------------------------//
			int WriteLogA( _lpcstr szFmt, ... );

			//--------------------------------------------------------------------------------------------------------//
			// 写日至
			// szBuffer		:	日至缓冲区，由用户提供
			// size			:	缓冲区大小
			// fmt			:	格式串
			// ...			:	参数列表
			// return		:	写入的字符数
			//--------------------------------------------------------------------------------------------------------//
			int WriteLog( _lptstr szBuffer, size_t nCount, _lpctstr szFmt, ... );

			//--------------------------------------------------------------------------------------------------------//
			// 写日至
			// szBuffer		:	日至缓冲区，由用户提供
			// ...			:	参数列表
			// return		:	写入的字符数
			// remark		:	小心，不能写过长的字符，4096个字符刚好。
			//--------------------------------------------------------------------------------------------------------//
			int WriteLog( _lpctstr szFmt, ... );

			//--------------------------------------------------------------------------------------------------------//
			// 写日至
			// szLog		:	日志串
			// nSize		:	串长度
			// return		:	写入的字符数
			//--------------------------------------------------------------------------------------------------------//
			int WriteData( const char* data, size_t size );

			//--------------------------------------------------------------------------------------------------------//
			// 设置日志输出模式
			// logger_mode	:	normal - 普通模式, pause - 暂停模式, ignore - 忽略模式 };
			//--------------------------------------------------------------------------------------------------------//
			void SetMode( logger_mode en_mode );

			//--------------------------------------------------------------------------------------------------------//
			// 设置日志输出等级
			// logger_mode	:	debug - 调试输出, trace - 包含DebugOutputString输出, warning - 警告输出, error - 错误输出
			//--------------------------------------------------------------------------------------------------------//
			void SetLevel( logger_level en_level );

			//---------------------------------------------------//
			// [3/4/2011 Albert]
			// Description:	增加日志头 
			//---------------------------------------------------//
			void AddHeaderWidget( int (*pfnWriteW)( CLogger*, _lpwstr, size_t ), int (*pfnWriteA)( CLogger*, _lpstr, size_t ) );
		private:
			struct	IMPL;
			IMPL*	m_pImpl;

			struct HeaderWidget
			{
				int (*WriteW)( CLogger* pLogger, _lpwstr szBuffer, size_t nCount );
				int (*WriteA)( CLogger* pLogger, _lpstr szBuffer, size_t nCount );
			} m_HeaderWidget[8];

			class logobj
			{
			public:
				explicit logobj(): m_pLog( NULL ){}
				explicit logobj( CLogger* pLog ): m_pLog( pLog ){}
				explicit logobj( const logobj& obj ){ m_pLog = obj.m_pLog; }

				~logobj();
				CLogger* operator=( CLogger* pLog ){ m_pLog = pLog;	return m_pLog; }
				CLogger* operator->()const{ return m_pLog; }
				operator CLogger*()const{	return m_pLog; }

			private:
				CLogger* m_pLog;
			};

		protected:
			//--------------------------------------------------------------------------------------------------------//
			// 分发日至到适配器
			// szLog		:	日志串
			// nSize		:	串长度
			//--------------------------------------------------------------------------------------------------------//
			static void DispatchLog( CLogger::IMPL* pImpl, void *data, size_t size );

			static unsigned int __stdcall captrue_debug_thread( void *param );
			static unsigned int __stdcall writer_thread( void *param );
		};
	}
}
#endif // _LOGGER_H