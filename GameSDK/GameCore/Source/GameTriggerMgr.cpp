#include "StdAfx.h"
#include "GameTriggerMgr.h"

CGameTriggerMgr::CGameTriggerMgr(void)
{
}

CGameTriggerMgr::~CGameTriggerMgr(void)
{
}

void CGameTriggerMgr::RegisterTrigger_i( _uint32 hSource, _uint16 nEvent, TriggerCall* pCall )
{
	SourceHash::iterator i = m_SourceHash.find( hSource );
	if( i == m_SourceHash.end() )
	{
		EventVec& e = m_SourceHash[hSource];
		e.resize( _trigger_event_count );
		if( nEvent < e.size() )
		{
			TriggerList& l = e.at( nEvent );
			l.push_back( pCall );
		}
	}
	else
	{
		EventVec& e = i->second;
		if( nEvent < e.size() )
		{
			TriggerList& l = e.at( nEvent );
			l.push_back( pCall );
		}
	}
}

void CGameTriggerMgr::DismissTrigger_i( _uint32 hSource, _uint16 nEvent, TriggerCall *pCall )
{
	SourceHash::iterator i = m_SourceHash.find( hSource );
	if( i != m_SourceHash.end() )
	{
		EventVec& e = i->second;
		if( nEvent < e.size() )
		{
			TriggerList& l = e.at( nEvent );
			
			TriggerList::iterator c = l.begin();
			while( c != l.end() )
			{
				if( (*c)->Compair( *pCall ) )
				{
					delete *c;
					l.erase( c );
					break;
				}
			}
		}
	}
}


//---------------------------------------------------//
// [9/8/2009 Albert]
// Description:	触发事件
//---------------------------------------------------//
void CGameTriggerMgr::TriggerEvent( _uint32 hSource, _uint16 nEvent, long_ptr lParam )
{
	SourceHash::iterator i = m_SourceHash.find( hSource );
	if( i != m_SourceHash.end() )
	{
		EventVec& e = i->second;
		if( nEvent < e.size() )
		{
			TriggerList& l = e.at( nEvent );
			TriggerList::iterator c = l.begin();
			while( c != l.end() )
			{
				TriggerCall* pCall = *c;
				if( (*pCall)( hSource, lParam ) )
				{
					break;
				}
				++c;
			}
		}

		// 销毁事件会使触发器也销毁.
		if( nEvent == trigger_event_destroy )
		{
			EventVec& e = i->second;
			for( int n = 0; n < _trigger_event_count; ++n )
			{
				TriggerList& l = e.at(n);
				while( !l.empty() )
				{
					delete *l.begin();
					l.pop_front();
				}
			}
			m_SourceHash.erase( i );
		}
	}
}