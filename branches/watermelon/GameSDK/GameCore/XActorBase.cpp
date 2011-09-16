#include "StdAfx.h"
#include "XActorBase.h"
#include "ObjectControlerBase.h"

namespace XGC
{
	BEGIN_IMPLEMENT_ATTRIBUTE( XActorBase, CGameObject )
		MAP_ATTRIBUTE( attrActorType )	// 角色类型
		MAP_ATTRIBUTE( attrHealth )		// 生命值
		MAP_ATTRIBUTE( attrMagic )		// 魔法值
		MAP_ATTRIBUTE( attrEnergy )		// 能量值
		MAP_ATTRIBUTE( attrDamage )		// 已累计技能伤害
		MAP_ATTRIBUTE( attrHealthMax )	// 生命值上限
		MAP_ATTRIBUTE( attrMagicMax )	// 魔法值上限
		MAP_ATTRIBUTE( attrEnergyMax )	// 能量值上限
		MAP_ATTRIBUTE( attrBeatSpeed )	// 击退速度
		MAP_ATTRIBUTE( attrFaintTime )	// 眩晕时间
		MAP_ATTRIBUTE( attrAbnormalTime )	// 特殊状态时间
		MAP_ATTRIBUTE( attrActorBornTime )	// 出生时间
	END_IMPLEMENT_ATTRIBUTE()
	XActorBase::XActorBase(void)
		: m_hEyeshotGroup( -1 )
		, m_ActorStatus( ActorStatus_Born )
		, m_hAttacker( INVALID_OBJECT_ID )
		, m_hAvengement( INVALID_OBJECT_ID )
		, m_nDamageMax( 0 )
		, m_hResetStatusTimerHandler( INVALID_TIMER_HANDLE )
	{
		Using( true, true, true );
		memset( &m_nEnemySit, -1, sizeof(m_nEnemySit) );
	}

	XActorBase::~XActorBase(void)
	{
	}

	//---------------------------------------------------//
	// [11/19/2010 Albert]
	// Description:	 
	//---------------------------------------------------//
	void XActorBase::UnderAttack( xObject hAttacker, int nDamage )
	{
		if( nDamage == 0 )
			return;

		if( m_ActorStatus == ActorStatus_Dead )
			return;

		m_hAttacker = hAttacker;
		CAttribute attrActorBase = GetLocalAttribute( true );
		attrActorBase.ChangeIntegerValue_Range( attrHealth, -nDamage, 0, GetLocalAttrib( attrHealthMax ) );
		attrActorBase.ChangeIntegerValue_Range( attrDamage,  nDamage, 0, GetLocalAttrib( attrHealthMax ) );
	
		if( hAttacker != INVALID_OBJECT_ID )
		{
			DamageMap::iterator pos = m_DamageMap.find( hAttacker );
			if( pos != m_DamageMap.end() )
			{
				if( pos->second += abs( nDamage ) > m_nDamageMax )
					m_hAvengement = hAttacker;
			}
			else
			{
				m_DamageMap.insert( DamageMap::value_type( hAttacker, abs(nDamage) ) );
				if( nDamage > m_nDamageMax )
					m_hAvengement = hAttacker;
			}
		}
		m_hAttacker = INVALID_OBJECT_ID;
	}

	//---------------------------------------------------//
	// [8/21/2009 Albert]
	// Description:	角色死亡
	//---------------------------------------------------//
	void XActorBase::Dead( xObject hAttacker )
	{
		if( m_ActorStatus != ActorStatus_Dead )
		{
			SetActorStatus( ActorStatus_Dead );
			SetTarget( INVALID_OBJECT_ID );
			ClearEnemySit( INVALID_OBJECT_ID );
			Trigger( TypeActorBase, hAttacker, 0, ActorEvent_Dead );
		}
	}

	//---------------------------------------------------//
	// [8/24/2009 Albert]
	// Description:	设置角色状态
	//---------------------------------------------------//
	void XActorBase::SetActorStatus( ActorStatus status, float fTime, int nMode )
	{
		if( m_ActorStatus == ActorStatus_Dead )
			return;

		if( fTime != 0.0f )
		{
			int nRelease = 0;
			if( m_hResetStatusTimerHandler != INVALID_TIMER_HANDLE )
			{
				nRelease = ThisTimer().remove_event( m_hResetStatusTimerHandler );
			}

			if( status == m_ActorRestonStatus )
			{
				switch( nMode )
				{
				case 0: // 覆盖
					m_hResetStatusTimerHandler = ThisTimer().insert_event( bind( &XActorBase::ResetActorStatus, this, _1, status ), 1, -1, TIMER_SECONDS(fTime) );
					break;
				case 1: // 顺延
					m_hResetStatusTimerHandler = ThisTimer().insert_event( bind( &XActorBase::ResetActorStatus, this, _1, status ), 1, -1, TIMER_SECONDS(fTime) + nRelease );
					break;
				case 2: // 取最小
					m_hResetStatusTimerHandler = ThisTimer().insert_event( bind( &XActorBase::ResetActorStatus, this, _1, status ), 1, -1, min( TIMER_SECONDS(fTime), nRelease ) );
					break;
				case 3: // 取最大
					m_hResetStatusTimerHandler = ThisTimer().insert_event( bind( &XActorBase::ResetActorStatus, this, _1, status ), 1, -1, max( TIMER_SECONDS(fTime), nRelease ) );
					break;
				}
			}
			else
			{
				m_hResetStatusTimerHandler = ThisTimer().insert_event( bind( &XActorBase::ResetActorStatus, this, _1, status ), 1, -1, TIMER_SECONDS(fTime) );
				m_ActorRestonStatus = status;
			}

		}
		else
		{
			if( status != ActorStatus_Live )
				SetObjectFlag( CGameObject::Flag_Move, false );
			else
				SetObjectFlag( CGameObject::Flag_Move, true );

			m_ActorStatus = status;
		}
	}

	//---------------------------------------------------//
	// [12/23/2010 Albert]
	// Description:	重置角色状态 
	//---------------------------------------------------//
	bool XActorBase::ResetActorStatus( timer_t handle, ActorStatus eStatus )
	{
		if( m_ActorStatus != ActorStatus_Dead )
			m_ActorStatus = eStatus;

		if( eStatus != ActorStatus_Live )
			SetObjectFlag( CGameObject::Flag_Move, false );
		else
			SetObjectFlag( CGameObject::Flag_Move, true );

		
		m_hResetStatusTimerHandler = INVALID_TIMER_HANDLE;
		return true;
	}

	void XActorBase::OnAddChild( CXObject* pChild )
	{
		if( pChild->IsTypeOf( TypeSkillBase ) )
		{
			m_SkillContainer.push_back( pChild->GetObjectID() );
		}
		__super::OnAddChild( pChild );
	}

	//---------------------------------------------------//
	// [11/19/2010 Albert]
	// Description:	当移除的对象为关注对象，则从列表中取出 
	//---------------------------------------------------//
	void XActorBase::OnRemoveChild( CXObject* pChild, bool bRelease )
	{
		xType type = pChild->GetType();

		if( pChild->IsTypeOf( TypeSkillBase ) )
		{
			m_SkillContainer.erase( std::find( m_SkillContainer.begin(), m_SkillContainer.end(), pChild->GetObjectID() ) );
		}

		__super::OnRemoveChild( pChild, bRelease );
	}

	static XVector3 Direction[] = 
	{
		XVector3( 0.0f, 0.0f, 0.0f ),
		XVector3( 1.0f, 0.0f, 0.0f ),
		XVector3( 0.5f, 0.0f, 0.5f ),
		XVector3( 0.0f, 0.0f, 1.0f ),
		XVector3( -0.5f, 0.0f, 0.5f ),

		XVector3( -1.0f, 0.0f, 0.0f ),
		XVector3( -0.5f, 0.0f, -0.5f ),
		XVector3( 0.0f, 0.0f, -1.0f ),
		XVector3( 0.5f, 0.0f, -0.5f ),
	};

	//---------------------------------------------------//
	// [12/27/2010 Albert]
	// Description:	获取空闲的攻击位置 
	//---------------------------------------------------//
	const XVector3& XActorBase::GetEnemySit( xObject hEnemy )
	{
		int nEmptyIndex = 0;
		for( int i = 1; i < _countof(m_nEnemySit); ++i )
		{
			if( !ObjectPool.IsValidObject( m_nEnemySit[i] ) )
				nEmptyIndex = i;
			else if( m_nEnemySit[i] == hEnemy )
				return Direction[i];
		}

		m_nEnemySit[nEmptyIndex] = hEnemy;
		return Direction[nEmptyIndex];

	}

	//---------------------------------------------------//
	// [12/27/2010 Albert]
	// Description:	清除攻击位置 
	//---------------------------------------------------//
	void XActorBase::ClearEnemySit( xObject hEnemy )
	{
		for( int i = 0; i < _countof(m_nEnemySit); ++i )
		{
			if( hEnemy == m_nEnemySit[i] || !ObjectPool.IsValidObject( m_nEnemySit[i] ) )
				m_nEnemySit[i] = INVALID_OBJECT_ID;
		}
	}

	//---------------------------------------------------//
	// [12/30/2010 Albert]
	// Description:	清除仇恨 
	//---------------------------------------------------//
	void XActorBase::ClearAvengement( xObject hTarget )
	{
		if( hTarget == INVALID_OBJECT_ID )
			m_DamageMap.clear();
		else
		{
			m_DamageMap.erase( hTarget );
			if( m_DamageMap.empty() )
				m_hAvengement = INVALID_OBJECT_ID;
			else
				m_hAvengement = m_DamageMap.begin()->first;
		}
	}

	//---------------------------------------------------//
	// [12/27/2010 Albert]
	// Description:	设置当前攻击的目标 
	//---------------------------------------------------//
	const XVector3& XActorBase::SetTarget( xObject hTarget )
	{
		XActorBase* pOldTarget = ObjectPool.GetObject< XActorBase >( m_hTarget );
		if( pOldTarget )
		{
			pOldTarget->ClearEnemySit( GetObjectID() );
		}

		XActorBase* pTarget = ObjectPool.GetObject< XActorBase >( hTarget );
		if( pTarget )
		{
			m_hTarget = hTarget;
			return pTarget->GetEnemySit( GetObjectID() );
		}

		return Direction[0];
	}

}
