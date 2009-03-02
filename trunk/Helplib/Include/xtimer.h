#pragma once
#define LOKI_CLASS_LEVEL_THREADING
#include <loki/TypeManip.h>
#include <loki/Function.h>
#include <loki/Singleton.h>
#include <loki/TypeTraits.h>
#include <loki/Threads.h>

typedef Loki::SmallObject< Loki::ClassLevelLockable, 1024, 128 > CPoolAllocator;
class TOOLLIB_API xtimer
{
public:
	xtimer(void);
	~xtimer(void);

	typedef Loki::Function< bool( unsigned int /*handle*/, unsigned short /*repeat*/, unsigned int /*timer*/) > event_function;
	struct event_interface
	{
		friend class xtimer;
		event_interface( event_function _function, unsigned short _repeat, unsigned int _timer )
			: handle( -1 )
			, function( _function )
			, repeat( _repeat )
			, timer( _timer )
			, index( 0 )
		{

		}

		~event_interface()
		{
			function = NULL;
			handle = -1;
			repeat = -1;
			timer = 0;
			index = 0;
		}

		void release(){ delete this; }
	private:
		event_function	function;
		unsigned int	handle;
		unsigned int	timer;
		unsigned short	repeat;
		unsigned short	index;

		size_t			round;
	};

public:
	bool initialize( unsigned int length, unsigned short max_object_count );

	void timer();

	unsigned int insert_event( event_interface* event );

	event_interface* get_event( unsigned int handle );

	void remove_event( unsigned int handle );

	void remove_event( event_interface* event );

protected:
	void inner_insert( unsigned int handle );

private:
	typedef std::list< unsigned int >		CTimerEventList;
	typedef std::vector< CTimerEventList >	CTimerEventListVec;
	typedef std::vector< event_interface* >	CTimerEventObjectVec;

	CTimerEventListVec		m_TimerVec;
	CTimerEventObjectVec	m_EventVec;

	struct timer_handle{ unsigned int position:24; unsigned int round:8; };

	size_t m_next_list;
	size_t m_timer_round;

	unsigned int m_object_count;
	unsigned int m_cur_pos;
	unsigned int m_round;
};

typedef xtimer::event_interface	event_interface;
typedef xtimer::event_function	event_function;