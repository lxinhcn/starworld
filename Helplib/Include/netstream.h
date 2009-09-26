#pragma once
#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS
#pragma warning (disable: 4251)
#include <tchar.h>
#include <string>

#include "loki/TypeTraits.h"

class TOOLLIB_API CNetInStream
{
public:
	CNetInStream(void);
	CNetInStream( char* pbuf, size_t nSize );
	~CNetInStream(void);

	// attach a buffer and return now buffer point
	char*	
		attach( char* pNewBuffer, size_t nSize );

	// detach a buffer
	char*	
		detach();

	CNetInStream& 
		write( const char* pszbuf, size_t size );
	// reset all state of this class 
	// for example, now site , len, buffer point and so on
	void	
		reset();

	// get buf point
	char*	
		get()const{ return m_pbuf; }

	// get now length
	size_t	
		length()const{ return m_nLength; }

	// move pos to
	bool	
		set( size_t pos );

	bool	
		offset( size_t delta );

	void	
		end();

	template< typename T >
	CNetInStream& write( const T& val )
	{
		if( m_nNowSite + sizeof( T ) < m_nBufSize )
		{
			*( (T*)( m_pbuf + m_nNowSite ) ) = val;
			m_nNowSite += sizeof( T );
		}
		else
		{
			throw( _T("buffer over length.") );
		}
		return *this;
	}

	inline CNetInStream& write( bool	val ){ return write< bool	>( val ); }
	inline CNetInStream& write( char	val ){ return write< char	>( val ); }
	inline CNetInStream& write( short	val ){ return write< short >( val ); }
	inline CNetInStream& write( int		val ){ return write< int	>( val ); }
	inline CNetInStream& write( long	val ){ return write< long	>( val ); }
	inline CNetInStream& write( float	val ){ return write< float >( val ); }
	inline CNetInStream& write( double	val ){ return write< double>( val ); }

	inline CNetInStream& write( unsigned char	val ){ return write< unsigned char	>( val ); }
	inline CNetInStream& write( unsigned short	val ){ return write< unsigned short>( val ); }
	inline CNetInStream& write( unsigned int	val ){ return write< unsigned int	>( val ); }
	inline CNetInStream& write( unsigned long	val ){ return write< unsigned long	>( val ); }

	CNetInStream& write( const char* val )
	{
		char* pbuf = m_pbuf + m_nNowSite;
		size_t nLen = strlen( val ) + 1;
		if( m_nNowSite + nLen < m_nBufSize )
		{
			memcpy( pbuf, val, nLen );
			m_nNowSite += nLen;
		}
		else
		{
			throw( _T("buffer over length.") );
		}
		return *this;
	}

	CNetInStream& write( char* val ){ return write< const char* >( val ); }
	CNetInStream& write( const wchar_t* val )
	{
		char* pbuf = m_pbuf + m_nNowSite;
		size_t nLen = (wcslen( val ) + 1)*sizeof(wchar_t);
		if( m_nNowSite + nLen < m_nBufSize )
		{
			memcpy( pbuf, val, nLen );
			m_nNowSite += nLen;
		}
		else
		{
			throw( _T("buffer over length.") );
		}
		return *this;
	}
	typedef void ( *pfnControl )( CNetInStream& netstream );

	inline CNetInStream& write( wchar_t* val ){ return write< const wchar_t* >( val ); }
	inline CNetInStream& write( pfnControl pfn ){ pfn( *this ); return *this; }

	template< typename T >
	CNetInStream& operator << ( T val ){ return write< T >( val ); }

	typedef void ( *pfnControl )( CNetInStream& netstream );
	template< >
	CNetInStream& operator << ( pfnControl pfn )
	{
		pfn( *this );
		return *this;
	}

	friend void TOOLLIB_API __cdecl end( CNetInStream& netstreams );
private:
	char*	m_pbuf;
	size_t	m_nBufSize;
	size_t	m_nNowSite;
	size_t	m_nLength;
};

inline void TOOLLIB_API __cdecl end( CNetInStream& netstream )
{
	netstream.m_nLength = netstream.m_nNowSite;
}

class TOOLLIB_API CNetOutStream
{
public:
	CNetOutStream( const char* data, size_t size );

	void reset();

	const char* get()const{ return m_pbuf + m_nNowSite; }

	const char* get( size_t size ){ const char* p = m_pbuf + m_nNowSite; m_nNowSite += size; return p;}

	// get now release length
	size_t length()const{ return m_nBufSize - m_nNowSite; }

	// get buffer size;
	size_t size()const{ return m_nBufSize; }

	size_t read( char* buf, size_t size );

	template< typename T >
	void read( T& val )
	{
		val = *( (T*)( m_pbuf + m_nNowSite ) );
		m_nNowSite += sizeof( T );
		if( m_nNowSite > m_nBufSize )
		{
			throw( _T( "too many read bitys" ) );
		}
	}

	template< >
	void read( const char*& val )
	{
		val = m_pbuf + m_nNowSite;
		m_nNowSite += strlen( val ) + 1;
		if( m_nNowSite > m_nBufSize )
		{
			throw( _T( "too many read bitys" ) );
		}
	}

	template< >
	void read( const wchar_t*& val )
	{
		val = (wchar_t*) (m_pbuf + m_nNowSite);
		m_nNowSite += (wcslen( val ) + 1)*sizeof(wchar_t);
		if( m_nNowSite > m_nBufSize )
		{
			throw( _T( "too many read bitys" ) );
		}
	}

	// operator 
	template< typename T >
		CNetOutStream& operator >> ( T& val )
	{
		read( val );
		return *this;
	}

	// move pos to
	bool setpos( size_t pos );
	bool offset( size_t delta );

private:
	const char*	m_pbuf;
	size_t		m_nBufSize;
	size_t		m_nNowSite;
};
