#include "stdafx.h"
#include ".\netpackage.h"

CNetPackage::CNetPackage( size_t package_size )
: m_pbuf( new char[package_size] )
{
	m_release_len = m_pbuf?package_size:0;
	m_package_size = m_pbuf?package_size:0;

	ASSERT( m_pbuf );
	if( !m_pbuf ) throw( std::runtime_error( "bad alloc size" ) );
}

CNetPackage::~CNetPackage(void)
{
	delete[] m_pbuf;
	m_pbuf = NULL;
}

//--------------------------------------------------------------------------------------------------------//
// 根据设定的长度截取消息长度
// pbuf	:	塞入的缓冲区首址
// size	:	缓冲区长度
// return	:	有多少字节被存储
//--------------------------------------------------------------------------------------------------------//
size_t CNetPackage::push( const char* pbuf, size_t size )
{
	if( m_release_len == 0 )	return 0;
	if( pbuf )
	{
		size_t cpysize = min( size, m_release_len );
		memcpy( m_pbuf + m_package_size - m_release_len, pbuf, cpysize );
		m_release_len -= cpysize;
		return cpysize;
	}
	return 0;
}
