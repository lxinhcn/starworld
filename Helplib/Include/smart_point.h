#ifndef _SMART_POINT_H
#define _SMART_POINT_H

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

class TOOLLIB_API CountPtr
{
public:
	CountPtr():m_ref( 1 ){}
	inline LONG addref()
	{
		if( m_ref <= 0 )	return 0;
		return ::InterlockedIncrement( &m_ref );
	}

	inline LONG release()
	{
		ASSERT( m_ref != 0 );
		return ::InterlockedDecrement( &m_ref );
	}

private:
	volatile LONG m_ref;
};

template < class _Tx >
class StandardDisposePolicy
{
public:
	void dispose( _Tx* point ){ delete point; }
};

template < class _Tx, class _Dispose = StandardDisposePolicy< _Tx > >
class smart_point
{
public:
	typedef _Tx*		element_type;
	typedef _Dispose	dispose_type;
	smart_point() : m_pCount( NULL ), m_point( NULL ){}

	explicit smart_point( _Tx* point )
	{
		m_pCount	= new CountPtr;
		m_point		= point;
	}

	smart_point( int point )
	: m_pCount( NULL )
	, m_point( NULL )
	{
		ASSERT( point == NULL );
	}

	smart_point( smart_point< _Tx, dispose_type > const& point )
	{
		if( point.m_pCount && point.m_pCount->addref() )
		{
			m_pCount	= point.m_pCount;
			m_point		= point.m_point;
		}
		else
		{
			m_pCount	= NULL;
			m_point		= NULL;
		}
	}

	virtual ~smart_point()
	{
		release();
	}

	//LONG addref()
	//{
	//	if( m_pCount )
	//	{
	//		return m_pCount->addref();
	//	}
	//	return 0;
	//}

	void release()
	{
		if( !m_pCount || !m_point )	return;
		if( m_pCount->release() == 0 )
		{
			_Tx* point = m_point;
			m_point = NULL;
			m_dispose.dispose( point );

			CountPtr*	pCount = m_pCount;
			m_pCount	= NULL;
			delete		pCount;
		}
	}

	inline _Tx* get()const{	return m_point;	}
	inline _Tx* operator ->()const{ return get();  }
	inline _Tx& operator  *()const{ return *get(); }

	inline operator bool()const{ return m_point != NULL; }

	smart_point< _Tx, dispose_type >& operator=( smart_point< _Tx, dispose_type > const& point )
	{
		release();
		if( point.m_pCount )
		{
			m_pCount = point.m_pCount;
			LONG ref = m_pCount->addref();
			if( ref )
			{
				m_point	= point.m_point;
			}
			else
			{
				m_point = NULL;
			}
		}
		return *this;
	}

	smart_point< _Tx, dispose_type >&	operator =( _Tx* point )
	{
		release();
		m_pCount	= new CountPtr();
		m_point		= point;

		return *this;
	}

private:
	_Dispose		m_dispose;
	CountPtr*		m_pCount;
	_Tx*			m_point;
};
#endif // _SMART_POINT_H