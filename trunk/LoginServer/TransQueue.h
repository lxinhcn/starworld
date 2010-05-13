#pragma once
#include <queue>

class CTransQueue
{
public:
	CTransQueue( _uint32 length_max );
	~CTransQueue(void);

	void put( transaction &trans );
	transaction get();

private:
	section		m_section;
	semaphore	m_full;
	semaphore	m_empty;

	std::queue< transaction >	m_transqueue;
};
