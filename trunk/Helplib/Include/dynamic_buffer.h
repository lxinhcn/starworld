#pragma once

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

class TOOLLIB_API CDynamicBuffer
{
public:
	CDynamicBuffer( DWORD chunksize = 4 * 1024 );
	~CDynamicBuffer();

	//--------------------------------------------------------------------------------------------------------//
	// 设置Chunk大小
	// dwSize	:	设定的大小
	//--------------------------------------------------------------------------------------------------------//
	VOID	SetChunkSize( DWORD dwSize );

	//--------------------------------------------------------------------------------------------------------//
	// 得到缓冲区地址
	// return	:	缓冲区地址
	//--------------------------------------------------------------------------------------------------------//
	LPBYTE	Get();

	//--------------------------------------------------------------------------------------------------------//
	// 得到缓冲区长度
	// return	:	缓冲区长度
	//--------------------------------------------------------------------------------------------------------//
	ULONG	Size()const;

	//--------------------------------------------------------------------------------------------------------//
	// 填充缓冲区
	// offset	:	缓冲区偏移量
	// buf		:	需要填入的缓冲区
	// size		:	缓冲区大小
	// return	:	实际写入的字节数
	//--------------------------------------------------------------------------------------------------------//
	ULONG	Fill( ULONG offset, CONST LPBYTE buf, ULONG size );

	//--------------------------------------------------------------------------------------------------------//
	// 填充类型
	// offset	:	缓冲区偏移量
	// buf		:	值
	// bReversal:	是否反续
	// return	:	实际写入的字节数
	//--------------------------------------------------------------------------------------------------------//
	template< typename T >
	ULONG FillType( ULONG offset, T buf, bool bReversal = true )
	{
		if( bReversal )
		{
			buf = helper::ReversalValue( buf );
		}
		return Fill( offset, (CONST LPBYTE)&buf, sizeof( T ) );
	}

	//------------- Warning ---------------//
	// 如果使用指针类型 则会产生一个编译期错误, 用于提醒你， 使用指针是非法的！！
	// 产生错误的原因是因为没有返回值
	//--------------------------------------------------------------------------------------------------------//
	template< typename T >
	ULONG FillType( ULONG offset, T* buf, bool bReversal = true ){}

protected:
	void CleanUp( BOOL bFree = FALSE );	

private:
	typedef std::vector< UCHAR* >	CChunkVec;

	CChunkVec	m_chunkvec;		// 以16k为一个块组成的缓冲区列表
	UCHAR*		m_head;			// 整理缓冲，整理chunklist后所有的缓冲数据被放置在头所指的缓冲区中。
	ULONG		m_headsize;		// 整理缓冲区大小
	ULONG		m_size;			// 有效缓冲的大小。
	DWORD		m_chunksize;	// 块大小
};

