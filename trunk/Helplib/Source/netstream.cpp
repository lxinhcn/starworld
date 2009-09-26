#include "StdAfx.h"
#include "netstream.h"

CNetInStream::CNetInStream(void)
{
	m_pbuf		= NULL;
	m_nBufSize	= 0;
	m_nNowSite	= 0;
	m_nLength	= 0;
}

CNetInStream::CNetInStream( char* data, size_t size )
: m_pbuf( data )
, m_nBufSize( size )
, m_nNowSite( 0 )
, m_nLength( 0 )
{
}

CNetInStream::~CNetInStream(void)
{
}

// 该函数主要用于接收缓冲区
// pNewBuffer	:	新的操作缓冲
// nSize		:	缓冲区大小
// return		:	旧的缓冲区地址
char* CNetInStream::attach( char* pNewBuffer, size_t nSize )
{
	char* pbuf	= m_pbuf;
	m_pbuf		= pNewBuffer;
	m_nBufSize	= nSize;
	m_nNowSite	= 0;
	return pbuf;
}

// 分离缓冲区
// return	:	当前的缓冲区指针
char* CNetInStream::detach()
{
	m_nBufSize = 0;
	m_nNowSite = 0;
	char* pbuf = m_pbuf;
	return pbuf;
}

// 从当前位置附加缓冲区
// pszbuf	:	附加的缓冲区首址
// size		:	附加缓冲区的大小
CNetInStream& CNetInStream::write( const char* pszbuf, size_t size )
{
	if( size > m_nBufSize - m_nNowSite )
	{
		throw( _T("buffer over length.") );
	}

	if( m_pbuf + m_nNowSite != pszbuf )
	{
		memcpy( m_pbuf + m_nNowSite, pszbuf, size );
	}
	m_nNowSite += size;
	return *this;
}

// 复位缓冲区偏移量
VOID CNetInStream::reset()
{
	m_nNowSite = 0;
}

bool CNetInStream::set( size_t pos )
{ 
	if( pos < 0 || pos >= m_nBufSize )
	{
		return false;
	}
	else
	{
		m_nNowSite = pos;
		return true;
	}
}

bool CNetInStream::offset( size_t delta )
{ 
	if( m_nNowSite + delta < 0 || m_nNowSite + delta > m_nBufSize )
	{
		return false;
	}
	else
	{
		m_nNowSite += delta;
		return true;
	}
}

void CNetInStream::end()
{
	m_nLength = m_nNowSite;
}

//////////////////////////////////////////////////////////////////////////
// OutStream
//////////////////////////////////////////////////////////////////////////

CNetOutStream::CNetOutStream( const char* data, size_t size )
:m_pbuf( data )
,m_nBufSize( size )
,m_nNowSite( 0 )
{
}

// 复位缓冲区偏移量
void CNetOutStream::reset()
{
	m_nNowSite = 0;
}

size_t	CNetOutStream::read( char* buf, size_t size )
{
	size_t cpysize = min( m_nBufSize - m_nNowSite, size );
	memcpy( buf, m_pbuf + m_nNowSite, cpysize );
	m_nNowSite += cpysize;
	return cpysize;
}

// move pos to
bool CNetOutStream::setpos( size_t pos )
{ 
	if( pos < 0 || pos >= m_nBufSize )
		return false;
	else
	{
		m_nNowSite = pos;
		return true;
	}
}

bool CNetOutStream::offset( size_t delta )
{ 
	if( m_nNowSite + delta < 0 || m_nNowSite + delta > m_nBufSize )
		return false;
	else
	{
		m_nNowSite += delta;
		return true;
	}
}
