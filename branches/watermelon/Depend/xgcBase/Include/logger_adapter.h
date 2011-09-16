#pragma once
#ifndef _LOGGER_ADAPTER__H
#define _LOGGER_ADAPTER__H

#ifdef EXPORT_API
#define EXPORT_LIB __declspec(dllexport)
#else
#define EXPORT_LIB __declspec(dllimport)
#endif

#include "logger.h"
namespace XGC
{
	namespace common
	{
		class EXPORT_LIB CLoggerAdapter
		{
		public:
			enum logger_level	{ debug, trace, warning, error, };

			CLoggerAdapter( _lpctstr strAdapterName, CLoggerAdapter::logger_level enLevel = CLoggerAdapter::debug )
				: m_strName( strAdapterName )
				, m_enLevel( enLevel )
			{}

			virtual ~CLoggerAdapter(){}

			//--------------------------------------------------------------------------------------------------------//
			// 得到适配器名字
			//--------------------------------------------------------------------------------------------------------//
			_lpctstr	GetName()const{ return m_strName.c_str(); }
			CLoggerAdapter::logger_level GetLevel()const{ return m_enLevel; }

			//--------------------------------------------------------------------------------------------------------//
			// 写日志
			// szBuffer		:	日志缓冲区，由用户提供
			// size			:	缓冲区大小
			//--------------------------------------------------------------------------------------------------------//
			virtual int Write( _lpcstr szBuffer, size_t size ) = 0;

		private:
			_string		m_strName;
			CLoggerAdapter::logger_level	m_enLevel;
		};

		//class CSocketAdapter	:	public CLoggerAdapter
		//{
		//public:
		//	CSocketAdapter( _lpctstr szAdapterName, CLoggerAdapter::ENLEVEL enLevel = CLoggerAdapter::ADAPTER_DEBUG )
		//		: CLoggerAdapter( szAdapterName, enLevel ){}
		//	virtual ~CSocketAdapter();
		//
		//	//--------------------------------------------------------------------------------------------------------//
		//	// 连接日志服务器
		//	// szHost		:	日志服务器ip
		//	// nPort		:	日志服务器端口
		//	// return		:	连接是否成功
		//	//--------------------------------------------------------------------------------------------------------//
		//	bool Connect( _lpctstr szHost, int nPort );
		//
		//	//--------------------------------------------------------------------------------------------------------//
		//	// 关联一个Socket
		//	// socket		:	指定的套接字
		//	// return		:	是否成功
		//	//--------------------------------------------------------------------------------------------------------//
		//	bool AttachSocket( DWORD s );
		//
		//protected:
		//	//--------------------------------------------------------------------------------------------------------//
		//	// 写日志
		//	// szBuffer		:	日志缓冲区，由用户提供
		//	// size			:	缓冲区大小
		//	//--------------------------------------------------------------------------------------------------------//
		//	virtual int WriteLog( _string strLoggerName, _lpctstr szBuffer, size_t size );
		//
		//private:
		//	SOCKET	m_socket;
		//};

		class EXPORT_LIB CFileAdapter	:	public CLoggerAdapter
		{
		public:
			enum split_mode
			{ 
				split_none	= 0,
				split_date	= 1,
				split_line	= 2,
			};

			enum file_mode
			{
				file_text = 0,
				file_binary = 1,
			};
			//////////////////////////////////////////////////////////////////////////
			// 构造函数
			// szAdapterName	:	适配器名字
			// nWriteVelocity	:	写入速率，每多少行强制写入一次 0 为不强制写入 1 为每次都写入 其他值为每多少行写入
			CFileAdapter( _lpctstr szAdapterName, int nWriteVelocity = 10, CLoggerAdapter::logger_level enLevel = CLoggerAdapter::debug )
				: CLoggerAdapter( szAdapterName, enLevel )
				, m_file( -1 )
				, m_nWriteVelocity( nWriteVelocity )
				, m_lpSplitParam( 0 )
				, m_nCurLine( 0 )
				, m_enSplitMode( split_none )
				, m_enFileMode( file_text )
			{}
			virtual ~CFileAdapter();

			//---------------------------------------------------//
			// [3/3/2011 Albert]
			// Description:	获取文件名 
			//---------------------------------------------------//
			_lpctstr GetFilename()const{ return m_strFilename.c_str(); }

			//---------------------------------------------------//
			// [3/3/2011 Albert]
			// Description:	获取扩展名 
			//---------------------------------------------------//
			_lpctstr GetFileExt()const{ return m_strExt.c_str(); }

			//--------------------------------------------------------------------------------------------------------//
			// 打开日志文件
			// szFilename	:	日志文件名
			// enSplitMode	:	分割模式 -split_date 按日期分割 -split_line 按行数分割 -split_none 不分割
			// lpParam		:	-split_line 模式时 该参数接受一个最大文件行数
			//					-split_date 模式时 该参数必须为 0
			// return		:	文件是否打开
			//--------------------------------------------------------------------------------------------------------//
			bool Open( _lpctstr szFilename, _lpctstr szExt = _T("log"), CFileAdapter::file_mode enFileMode = file_text, split_mode enSplitMode = split_none, ulong_ptr lpParam = 0 );

			//--------------------------------------------------------------------------------------------------------//
			// 关闭日志文件
			//--------------------------------------------------------------------------------------------------------//
			void Close();

		protected:
			//---------------------------------------------------//
			// [3/3/2011 Albert]
			// Description:	打开文件成功后调用 
			//---------------------------------------------------//
			virtual bool OnOpenFile( _lpctstr tszFilename ){ return true; }

			//---------------------------------------------------//
			// [3/3/2011 Albert]
			// Description:	关闭文件前调用 
			//---------------------------------------------------//
			virtual void OnCloseFile(){};

			//--------------------------------------------------------------------------------------------------------//
			// 写日志
			// szBuffer		:	日志缓冲区，由用户提供
			// size			:	缓冲区大小
			//--------------------------------------------------------------------------------------------------------//
			virtual int Write( _lpcstr szBuffer, size_t size );

			//--------------------------------------------------------------------------------------------------------//
			// 根据SplitMode生成一个文件名
			// lptszBuf		:	缓冲区
			// nCount		:	缓冲区字符个数
			// return		:	填充了的缓冲区字符个数
			//--------------------------------------------------------------------------------------------------------//
			int CombinedFileName( _lptstr lptszBuf, size_t nCount );
		private:
			int			m_file;
			_string		m_strFilename;
			_string		m_strExt;
			int			m_nWriteVelocity;
			split_mode	m_enSplitMode;
			file_mode	m_enFileMode;
			ulong_ptr	m_lpSplitParam;
			_uint32		m_nCurLine;
		};

		class EXPORT_LIB CShareFileAdapter : public CFileAdapter
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			// 构造函数
			// szAdapterName	:	适配器名字
			// nWriteVelocity	:	写入速率，每多少行强制写入一次 0 为不强制写入 1 为每次都写入 其他值为每多少行写入
			CShareFileAdapter( _lpctstr szAdapterName, int nWriteVelocity = 10, CLoggerAdapter::logger_level enLevel = CLoggerAdapter::debug )
				: CFileAdapter( szAdapterName, nWriteVelocity, enLevel )
				, m_hMutex( INVALID_HANDLE_VALUE )
			{}

		protected:
			//---------------------------------------------------//
			// [3/3/2011 Albert]
			// Description:	打开文件成功后调用 
			//---------------------------------------------------//
			virtual bool OnOpenFile( _lpctstr tszFilename );

			//---------------------------------------------------//
			// [3/3/2011 Albert]
			// Description:	关闭文件前调用 
			//---------------------------------------------------//
			virtual void OnCloseFile();

			//--------------------------------------------------------------------------------------------------------//
			// 写日志
			// szBuffer		:	日志缓冲区，由用户提供
			// size			:	缓冲区大小
			//--------------------------------------------------------------------------------------------------------//
			virtual int Write( _lpcstr szBuffer, size_t size );

		private:
			HANDLE		m_hMutex;

		};

		class EXPORT_LIB CWindowAdapter :	public CLoggerAdapter
		{
		public:
			explicit CWindowAdapter( HWND hEditHwnd, WPARAM wTextLimitLen = 1024*16, _lpctstr lpszName = _T("WindowLog") );
			virtual ~CWindowAdapter(void);

		protected:
			//--------------------------------------------------------------------------------------------------------//
			// 写日志
			// szBuffer		:	日志缓冲区，由用户提供
			// size			:	缓冲区大小
			//--------------------------------------------------------------------------------------------------------//
			virtual int Write( _lpcstr szBuffer, size_t size );

		private:
			CONST HWND	m_hEditHwnd;
			WPARAM		m_nMaxLen;
			INT			m_nTotalLen;
		};

		class EXPORT_LIB CConsoleAdapter :	public CLoggerAdapter
		{
		public:
			explicit CConsoleAdapter( HANDLE hOutput, _lpctstr lpszName = _T("ConsoleLog"), _uint16 nCodePage = 936, CLoggerAdapter::logger_level enLevel = CLoggerAdapter::debug );
			virtual ~CConsoleAdapter(void);

		protected:
			//--------------------------------------------------------------------------------------------------------//
			// 写日志
			// szBuffer		:	日志缓冲区，由用户提供
			// size			:	缓冲区大小
			//--------------------------------------------------------------------------------------------------------//
			virtual int Write( _lpcstr szBuffer, size_t size );

		private:
			HANDLE m_hOutput;
		};

		class EXPORT_LIB CDebugViewAdapter :	public CLoggerAdapter
		{
		public:
			explicit CDebugViewAdapter( _lpctstr lpszName = _T("DebugViewLog"), _uint16 nCodePage = 936, CLoggerAdapter::logger_level enLevel = CLoggerAdapter::debug );
			virtual ~CDebugViewAdapter(void);

		protected:
			//--------------------------------------------------------------------------------------------------------//
			// 写日志
			// szBuffer		:	日志缓冲区，由用户提供
			// size			:	缓冲区大小
			//--------------------------------------------------------------------------------------------------------//
			virtual int Write( _lpcstr szBuffer, size_t size );
		};

	}
}

#endif // _LOGGER_ADAPTER__H
