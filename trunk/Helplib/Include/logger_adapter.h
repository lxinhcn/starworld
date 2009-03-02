#pragma once
#ifndef _LOGGER_ADAPTER__H
#define _LOGGER_ADAPTER__H

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

class CLogger;
class TOOLLIB_API CLoggerAdapter
{
friend class CLogger;
public:
	enum ENLEVEL
	{ 
		ADAPTER_DEBUG,	// 调试，调试环境下输出
		ADAPTER_ERROR,	// 错误，任何环境下输出
		ADAPTER_WRONG,	// 警告，通过OutputDebugString显示
	};
	CLoggerAdapter( LPCTSTR strAdapterName, CLoggerAdapter::ENLEVEL enLevel = CLoggerAdapter::ADAPTER_ERROR )
		: m_strName( strAdapterName ),
		m_enLevel( enLevel )
	{}

	virtual ~CLoggerAdapter(){}

	//--------------------------------------------------------------------------------------------------------//
	// 得到适配器名字
	//--------------------------------------------------------------------------------------------------------//
	LPCTSTR	GetName()const{ return m_strName.c_str(); }
	ENLEVEL GetLevel()const{ return m_enLevel; }
protected:
	//--------------------------------------------------------------------------------------------------------//
	// 写日志
	// szBuffer		:	日志缓冲区，由用户提供
	// size			:	缓冲区大小
	//--------------------------------------------------------------------------------------------------------//
	virtual int WriteLog( _string strLoggerName, LPCTSTR szBuffer, size_t size ) = 0;

private:
	_string		m_strName;
	ENLEVEL		m_enLevel;
};

class TOOLLIB_API CSocketAdapter	:	public CLoggerAdapter
{
public:
	CSocketAdapter( LPCTSTR szAdapterName, CLoggerAdapter::ENLEVEL enLevel = CLoggerAdapter::ADAPTER_DEBUG )
		: CLoggerAdapter( szAdapterName, enLevel ){}
	virtual ~CSocketAdapter();

	//--------------------------------------------------------------------------------------------------------//
	// 连接日志服务器
	// szHost		:	日志服务器ip
	// nPort		:	日志服务器端口
	// return		:	连接是否成功
	//--------------------------------------------------------------------------------------------------------//
	bool Connect( LPCTSTR szHost, int nPort );

	//--------------------------------------------------------------------------------------------------------//
	// 关联一个Socket
	// socket		:	指定的套接字
	// return		:	是否成功
	//--------------------------------------------------------------------------------------------------------//
	bool AttachSocket( DWORD s );

protected:
	//--------------------------------------------------------------------------------------------------------//
	// 写日志
	// szBuffer		:	日志缓冲区，由用户提供
	// size			:	缓冲区大小
	//--------------------------------------------------------------------------------------------------------//
	virtual int WriteLog( _string strLoggerName, LPCTSTR szBuffer, size_t size );

private:
	SOCKET	m_socket;
};

class TOOLLIB_API CFileAdapter	:	public CLoggerAdapter
{
public:
	enum EN_FILEMODE
	{ 
		split_none	= 0,
		split_date	= 1,
		split_line	= 2,
	};
	//////////////////////////////////////////////////////////////////////////
	// 构造函数
	// szAdapterName	:	适配器名字
	// nWriteVelocity	:	写入速率，每多少行强制写入一次 0 为不强制写入 1 为每次都写入 其他值为每多少行写入
	CFileAdapter( LPCTSTR szAdapterName, int nWriteVelocity = 10, CLoggerAdapter::ENLEVEL enLevel = CLoggerAdapter::ADAPTER_ERROR )
		: CLoggerAdapter( szAdapterName, enLevel )
		, m_file( NULL )
		, m_nWriteVelocity( nWriteVelocity )
		, m_lpSplitParam( 0 )
		, m_nCurLine( 0 )
	{}
	virtual ~CFileAdapter();

	//--------------------------------------------------------------------------------------------------------//
	// 打开日志文件
	// szFilename	:	日志文件名
	// enSplitMode	:	分割模式 -split_date 按日期分割 -split_line 按行数分割 -split_none 不分割
	// lpParam		:	-split_line 模式时 该参数接受一个最大文件行数
	//					-split_date 模式时 该参数必须为 0
	// return		:	文件是否打开
	//--------------------------------------------------------------------------------------------------------//
	bool Open( LPCTSTR szFilename, LPCTSTR szExt = _T("log"), EN_FILEMODE enSplitMode = split_none, LPARAM lpParam = 0 );

	//--------------------------------------------------------------------------------------------------------//
	// 关闭日志文件
	//--------------------------------------------------------------------------------------------------------//
	void Close();

protected:
	//--------------------------------------------------------------------------------------------------------//
	// 写日志
	// szBuffer		:	日志缓冲区，由用户提供
	// size			:	缓冲区大小
	//--------------------------------------------------------------------------------------------------------//
	virtual int WriteLog( _string strLoggerName, LPCTSTR szBuffer, size_t size );

	//--------------------------------------------------------------------------------------------------------//
	// 根据SplitMode生成一个文件名
	// lptszBuf		:	缓冲区
	// nCount		:	缓冲区字符个数
	// return		:	填充了的缓冲区字符个数
	//--------------------------------------------------------------------------------------------------------//
	int CombinedFileName( LPTSTR lptszBuf, size_t nCount );
private:
	_string		m_strFilename;
	_string		m_strExt;
	FILE*		m_file;
	int			m_nWriteVelocity;
	EN_FILEMODE	m_enSplitMode;
	LPARAM		m_lpSplitParam;
	UINT		m_nCurLine;
};

class TOOLLIB_API CWindowAdapter :	public CLoggerAdapter
{
public:
	explicit CWindowAdapter( HWND hEditHwnd, LPCTSTR lpszName = _T("WindowLog") );
	virtual ~CWindowAdapter(void);

protected:
	//--------------------------------------------------------------------------------------------------------//
	// 写日志
	// szBuffer		:	日志缓冲区，由用户提供
	// size			:	缓冲区大小
	//--------------------------------------------------------------------------------------------------------//
	virtual int WriteLog( _string strLoggerName, LPCTSTR szBuffer, size_t size );

private:
	CONST HWND	m_hEditHwnd;
	INT			m_nTotalLen;
};

class TOOLLIB_API CConsoleAdapter :	public CLoggerAdapter
{
public:
	explicit CConsoleAdapter( LPCTSTR lpszName = _T("WindowLog"), CLoggerAdapter::ENLEVEL enLevel = CLoggerAdapter::ADAPTER_ERROR );
	virtual ~CConsoleAdapter(void);

protected:
	//--------------------------------------------------------------------------------------------------------//
	// 写日志
	// szBuffer		:	日志缓冲区，由用户提供
	// size			:	缓冲区大小
	//--------------------------------------------------------------------------------------------------------//
	virtual int WriteLog( _string strLoggerName, LPCTSTR szBuffer, size_t size );
};

#endif // _LOGGER_ADAPTER__H