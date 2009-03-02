#include "StdAfx.h"
#include "xtimer.h"

xtimer::xtimer(void)
: m_cur_pos( 0x01000000 )
, m_round( 1 )
, m_object_count( 0 )
, m_next_list( 0 )
, m_timer_round( 0 )
{
}

xtimer::~xtimer(void)
{
	m_TimerVec.clear();

	CTimerEventObjectVec::iterator iter = m_EventVec.begin();
	while( iter != m_EventVec.end() )
	{
		delete *iter;
		++iter;
	}
	m_EventVec.clear();
}

bool xtimer::initialize( unsigned int length, unsigned short max_object_count )
{
	m_TimerVec.resize( length );
	m_EventVec.resize( max_object_count, NULL );

	return true;
}

unsigned int xtimer::insert_event( event_interface* event )
{
	if( m_object_count == m_EventVec.size() ) return -1;
	if( !event ) return -1;

	event->handle = m_cur_pos;
	timer_handle& h = (timer_handle&)m_cur_pos;

	m_EventVec[h.position] = event;
	do
	{
		++h.position;
		if( h.position >= m_EventVec.size() )
		{
			++h.round;
			++m_round;
			h.position = 0;
		}
	}
	while( m_EventVec[h.position] != NULL );

	++m_object_count;
	inner_insert( event->handle );

	return event->handle;
}

xtimer::event_interface* xtimer::get_event( unsigned int handle )
{
	timer_handle& h = (timer_handle&)m_cur_pos;
	if( h.position < m_EventVec.size() )
	{
		event_interface* event = m_EventVec.at(h.position);
		if( event && event->handle == handle )
		{
			return event;
		}
	}
	return NULL;
}

void xtimer::remove_event( unsigned int handle )
{
	event_interface* event = get_event( handle );
	remove_event( event );
}

void xtimer::remove_event( event_interface* event )
{
	if( event )
	{
		timer_handle& h = (timer_handle&)m_cur_pos;
		event_interface* _event = m_EventVec[h.position];
		if( event == _event )
		{
			m_EventVec[h.position] = NULL;
			event->release();
			--m_object_count;
		}
	}
}

void xtimer::inner_insert( unsigned int handle )
{
	event_interface* event = get_event( handle );
	if( !event ) return;

	size_t index = event->timer + m_next_list;
	size_t round = m_timer_round + index/m_TimerVec.size();

	index = index%m_TimerVec.size();
	event->index = (unsigned short)index;
	event->round = (unsigned short)round;

	m_TimerVec[index].push_back( handle );
}

void xtimer::timer()
{
	CTimerEventList& timer_list = m_TimerVec[m_next_list++];
	CTimerEventList::iterator iter = timer_list.begin();
	while( iter != timer_list.end() )
	{
		unsigned int handle = *iter;
		event_interface* event = get_event( handle );
		if( !event )
		{
			iter = timer_list.erase( iter );
			continue;
		}

		if( event->round == m_round )
		{
			unsigned int handle = event->handle;
			bool ret = event->function( handle, event->repeat, event->timer );
			if( get_event( handle ) )
			{
				if( !ret || event->repeat == 0 )
				{
					remove_event( event );
				}
				else
				{
					--event->repeat;
					inner_insert( handle );
				}
			}
			iter = timer_list.erase( iter );
			continue;
		}
		++iter;
	}

	if( m_next_list >= m_TimerVec.size() )
	{
		++m_timer_round;
		m_next_list = 0;
	}
}
