#pragma once
#include <queue>

class CTransQueue
{
public:
	CTransQueue();
	~CTransQueue(void);

	void put( transaction *trans );
	transaction* get();

private:
	section		m_section;
	std::queue< transaction* >	m_transqueue;
};
