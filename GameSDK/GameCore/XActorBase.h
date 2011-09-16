#pragma once

#include "GameObject.h"

namespace XGC
{
	class CObjectControlerBase;
	class CORE_API XActorBase : public CGameObject
	{
		DECLARE_DYNAMICTYPE( CGameObject, TypeActorBase );
		DECLARE_ATTRIBUTE(ActorBase_attrCount);
		friend class CObjectControlerBase;
		friend class CServerMap;
	public:
		XActorBase(void);
		~XActorBase(void);

		enum ActorStatus { ActorStatus_Born, ActorStatus_Live, ActorStatus_Pain, ActorStatus_Faint, ActorStatus_Fall, ActorStatus_Abnormal, ActorStatus_Dead, };
		enum ActorEvent
		{ 
			ActorEvent_Damage	= 1,
			ActorEvent_Pain		= 2, 
			ActorEvent_Faint	= 4, 
			ActorEvent_Fall		= 8, 
			ActorEvent_Beat		= 16, 
			ActorEvent_Dead		= 32, 
		};
		//---------------------------------------------------//
		// [11/19/2010 Albert]
		// Description:	 受到攻击
		//---------------------------------------------------//
		void UnderAttack( xObject hAttacker, int nDamage );

		//---------------------------------------------------//
		// [11/19/2010 Albert]
		// Description:	获取仇恨最高的角色 
		//---------------------------------------------------//
		xObject GetAttacker()const{ return m_hAttacker; }

		//---------------------------------------------------//
		// [11/19/2010 Albert]
		// Description:	获取仇恨最高的角色 
		//---------------------------------------------------//
		xObject GetAvengement()const{ return m_hAvengement; }

		//---------------------------------------------------//
		// [12/27/2010 Albert]
		// Description:	获取正在攻击的目标 
		//---------------------------------------------------//
		xObject GetTarget()const{ return m_hTarget; }

		//---------------------------------------------------//
		// [12/27/2010 Albert]
		// Description:	设置当前攻击的目标 
		//---------------------------------------------------//
		const XVector3& SetTarget( xObject hTarget );

		//---------------------------------------------------//
		// [8/21/2009 Albert]
		// Description:	角色死亡
		//---------------------------------------------------//
		void Dead( xObject hAttacker );

		//---------------------------------------------------//
		// [12/28/2010 Albert]
		// Description:	角色重生 
		//---------------------------------------------------//
		void Relive(){ m_ActorStatus = ActorStatus_Born; }

		//---------------------------------------------------//
		// [8/24/2009 Albert]
		// Description:	获取角色状态
		//---------------------------------------------------//
		ActorStatus GetActorStatus()const{ return m_ActorStatus; }

		//---------------------------------------------------//
		// [8/24/2009 Albert]
		// Description:	设置角色状态
		//---------------------------------------------------//
		void SetActorStatus( ActorStatus status, float fTime = 0.0f, int nMode = 0 );

		//---------------------------------------------------//
		// [8/21/2009 Albert]
		// Description:	获取视野组
		//---------------------------------------------------//
		unsigned int GetEyeshotGroup()const{ return m_hEyeshotGroup; }

		//---------------------------------------------------//
		// [11/19/2010 Albert]
		// Description:	获取技能 
		//---------------------------------------------------//
		xObject GetSkillObject( unsigned int nIndex )const
		{ 
			ASSERT_RETURN( nIndex < m_SkillContainer.size(), INVALID_OBJECT_ID );
			return m_SkillContainer[nIndex]; 
		}

		//---------------------------------------------------//
		// [11/19/2010 Albert]
		// Description:	获取技能个数 
		//---------------------------------------------------//
		size_t GetSkillCount()const
		{
			return m_SkillContainer.size();
		}

		//---------------------------------------------------//
		// [12/27/2010 Albert]
		// Description:	获取空闲的攻击位置 
		//---------------------------------------------------//
		const XVector3& GetEnemySit( xObject hEnemy );

		//---------------------------------------------------//
		// [12/27/2010 Albert]
		// Description:	清除攻击位置 
		//---------------------------------------------------//
		void ClearEnemySit( xObject hEnemy );

		//---------------------------------------------------//
		// [12/30/2010 Albert]
		// Description:	清除仇恨 
		//---------------------------------------------------//
		void ClearAvengement( xObject hTarget = INVALID_OBJECT_ID );
	protected:
		typedef xgc_map< xObject, int >	DamageMap;
		typedef xgc_vector< xObject >	SkillContainer;
		typedef SkillContainer::iterator		SkillIterator;
		typedef SkillContainer::const_iterator	constSkillIterator;

	private:
		//---------------------------------------------------//
		// [12/23/2010 Albert]
		// Description:	重置角色状态 
		//---------------------------------------------------//
		bool ResetActorStatus( timer_t handle, ActorStatus eStatus );

		//---------------------------------------------------//
		// [11/19/2010 Albert]
		// Description:	当加入的对象为关注的类型，则进行管理。
		//---------------------------------------------------//
		virtual void OnAddChild( CXObject* pChild );

		//---------------------------------------------------//
		// [11/19/2010 Albert]
		// Description:	当移除的对象为关注对象，则从列表中取出 
		//---------------------------------------------------//
		virtual void OnRemoveChild( CXObject* pChild, bool bRelease );

	protected:
		_uint32			m_hEyeshotGroup;	// 视野组
		timer_t			m_hResetStatusTimerHandler;
	private:
		SkillContainer	m_SkillContainer;
		DamageMap		m_DamageMap;
		ActorStatus		m_ActorStatus;		// 角色状态
		ActorStatus		m_ActorRestonStatus;// 角色重置状态

		xObject			m_hAttacker;		// 攻击者
		xObject			m_hAvengement;		// 仇恨
		xObject			m_hTarget;			// 当前目标
		int				m_nDamageMax;
		unsigned int	m_nEnemySit[8];		// 攻击者的位置，8个
	};
}
