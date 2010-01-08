#include "StdAfx.h"
#include "TransQueue.h"

CTransQueue::CTransQueue( _uint32 length_max )
: m_full( 0, length_max )
, m_empty( length_max, length_max )
{
}

CTransQueue::~CTransQueue(void)
{
}

void CTransQueue::put( transaction &trans )
{
	while( WaitForSingleObject( m_full, INFINITE ) != WAIT_OBJECT_0 );

	CCriticalLock lock( m_critical );
	m_transqueue.push( trans );
	m_empty.unlock();
}

transaction CTransQueue::get()
{
	while( WaitForSingleObject( m_empty, INFINITE ) != WAIT_OBJECT_0 );

	CCriticalLock lock( m_critical );
	transaction ret = m_transqueue.front();
	m_transqueue.pop();
	m_full.unlock();
	return ret;
}
