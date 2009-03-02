#pragma once

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

#include <queue>

template< typename T, bool bThread = true, int max_count = 1024 * 8 >
class CLookaside
{
protected:
	typedef std::queue< T* >	CTQueue;
	CTQueue						m_queue;
	CRITICAL_SECTION			m_CriSection;
	T							m_Check;
public:
	CLookaside()
	{
		if( bThread ) InitializeCriticalSection(&m_CriSection);
		memset( &m_Check, 0xfd, GetAllocSize() );
	};
	virtual ~CLookaside()
	{
		{
			if( bThread ) CCriticalLock lock( m_CriSection );
			while( !m_queue.empty() )
			{
				T*	p = m_queue.front();
				ASSERT( memcmp( &m_Check, p, GetAllocSize() ) == 0 );
				delete p;
				m_queue.pop();
			}
		}
		if( bThread ) DeleteCriticalSection(&m_CriSection);
	}

	T*	Alloc()
	{
		if( bThread ) CCriticalLock lock( m_CriSection );
		if( m_queue.empty() )
			return new T;
		else
		{
			T* p = m_queue.front();
			m_queue.pop();

			ASSERT( memcmp( &m_Check, p, GetAllocSize() ) == 0 );
	
			//if( bClass ) new(p) T;
			return p;
		}
	}

	void Release( T*& p )
	{
		if( bThread ) CCriticalLock lock( m_CriSection );
		if( m_queue.size() >= max_count )	delete p;
		else
		{
			memset( p, 0xfd, GetAllocSize() );
			m_queue.push( p );
			//if( bClass ) p->~T();
			p = NULL;
		}
	}

	size_t GetAllocSize()
	{
		return sizeof( T );
	}
};
