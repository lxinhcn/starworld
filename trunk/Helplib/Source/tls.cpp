#include "StdAfx.h"
#include "tls.h"

CTls::CTls()
: m_dwTlsIndex( TLS_OUT_OF_INDEXES )
{
	m_dwTlsIndex = TlsAlloc();
}

CTls::~CTls()
{
	BOOL bRet = TlsFree( m_dwTlsIndex );
	ASSERT( bRet );
}

PVOID CTls::Get()
{
	if( m_dwTlsIndex == TLS_OUT_OF_INDEXES )	return NULL;

	return TlsGetValue( m_dwTlsIndex );
}

BOOL CTls::Set( PVOID pValue )
{
	if( m_dwTlsIndex == TLS_OUT_OF_INDEXES )	return FALSE;

	return TlsSetValue( m_dwTlsIndex, pValue );
}
