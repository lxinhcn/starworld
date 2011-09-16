#include "StdAfx.h"
#include "XQuest.h"
#include "XActorBase.h"

namespace XGC
{
	CXQuestObjective::CXQuestObjective( _string strName, _uint16 nFinishCount )
		: CXObject( false, false, false )
		, m_strName( strName )
		, m_nFinishCount( nFinishCount )
		, m_nCount( 0 )
		, m_nSerial( 0 )
	{

	}

	CXQuestObjective::~CXQuestObjective(void)
	{

	}

	bool CXQuestObjective::Increment()
	{
		++m_nCount;
		if( m_nCount == m_nFinishCount )
		{
			CXQuest *pQuest = static_cast< CXQuest* >( ObjectPool.GetObject( GetParent() ) );
			if( pQuest )
			{
				pQuest->ObjectiveNotify( m_nSerial );
			}
			return true;
		}
		return false;
	}

	int CXQuestObjective::OnKill( xObject hKiller, xObject hDead )
	{
		XActorBase *pActor = static_cast< XActorBase* >( ObjectPool.GetObject( hDead, TypeActorBase ) );
		if( pActor )
		{
			_lpctstr name = pActor->GetStrAttrib( _T("name") );
			ASSERT_POINTER( name );
			if( name && m_strName == name && Increment() )
			{
				XActorBase *pKiller = static_cast< XActorBase* >( ObjectPool.GetObject( hKiller, TypeActorBase ) );

				if( pKiller )
				{
					// 任务完成则将触发器摘除
					// ThisTriggerMgr()->DismissTrigger( hKiller, 0, this, &CXQuestObjective::OnKill );
					// pKiller->DismissTrigger( hDead, 0 );
				}
			}
		}
		return false;
	}

	int CXQuestObjective::OnItem( xObject hSource, long_ptr lParam )
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 任务
	//////////////////////////////////////////////////////////////////////////
	CXQuest::CXQuest(void)
		: CXObject( true, false, false )
		, m_FinishMask( 0 )
		, m_Mask( 0 )
	{
	}

	CXQuest::~CXQuest(void)
	{
	}

	void CXQuest::OnAddChild( CXObject* pChild )
	{
		int nChildCount = GetChildrenCount();
		if( nChildCount >= 16 )
		{
			XGC_WRITELOG( _T("ServerCore"), _T("任务目标已经超过数量限制.") );
			return;
		}

		ASSERT( pChild && pChild->IsTypeOf(TypeQuestObjective) );
		CXQuestObjective* pObjective = static_cast< CXQuestObjective* >( pChild );
		pObjective->m_nSerial = (1<<nChildCount);
		m_FinishMask |= pObjective->m_nSerial;
	}

	void CXQuest::ObjectiveNotify( _uint16 nSerial )
	{
		m_Mask |= nSerial;
		if( m_Mask == m_FinishMask )
			OnQuestFinished();
	}
}
