#include "StdAfx.h"
#include "TransQueue.h"

CTransQueue::CTransQueue()
{
}

CTransQueue::~CTransQueue(void)
{
}

void CTransQueue::put( transaction *trans )
{
	autolock lock( m_section );
	m_transqueue.push( trans );
}

transaction* CTransQueue::get()
{
	autolock lock( m_section );
	if( m_transqueue.empty() )
		return nullptr;

	transaction *ret = m_transqueue.front();
	m_transqueue.pop();
	return ret;
}
