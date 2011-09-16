#include "StdAfx.h"
#include "AttackState.h"
#include "XActorBase.h"
#include "XSkillBase.h"

namespace XGC
{
	CAttackState::CAttackState( byte_t code, const GuardFunctor& Fn )
		: CStateBase( eStateType_Attack, code, Fn )
	{
	}

	CAttackState::~CAttackState(void)
	{
	}

	//---------------------------------------------------//
	// [11/24/2010 Albert]
	// Description:	触发器事件发生时
	//---------------------------------------------------//
	void CAttackState::OnTrigger( xObject hSource, xObject hDest, long_ptr lParam, int_ptr wParam )
	{
		switch( wParam )
		{
		case XSkillBase::SkillCasting:
			mStateParams.bit1 = true;
			break;
		case XSkillBase::SkillCooldown:
			mStateParams.bit2 = true;
			break;
		case XSkillBase::SkillOver:
			mStateParams.bit3 = true;
			break;
		}
	}

	//--------------------------------------------------------//
	//	created:	21:1:2010   14:33
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	进入状态前调用
	//	pEntity:	进入状态的对象指针
	//	return:		TRUE 进入状态成功， false进入状态失败
	//--------------------------------------------------------//
	bool CAttackState::OnEnter( CGameObject& Object, StateParams& Params )
	{
		mStateParams = static_cast< AttackStateParams& >( Params );
		Object.RegisteTrigger( TypeSkillBase, bind(&CAttackState::OnTrigger, this, _1, _2, _3, _4 ) );

		XSkillBase* pSkill = ObjectPool.GetObject< XSkillBase >( mStateParams.hSkill );
		pSkill->MarkCasting( pSkill->GetLocalAttrib( attrCastingTime ) );
		pSkill->MarkCooldown( pSkill->GetLocalAttrib( attrCooldownTime ) );
		pSkill->CastSkill( mStateParams.hTarget );
		return true;
	}

	//--------------------------------------------------------//
	//	created:	21:1:2010   14:35
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	离开状态
	//	pEntity:	进入状态的对象指针
	//--------------------------------------------------------//
	void CAttackState::OnLeave( CGameObject& Object )
	{
		Object.DismissTrigger( TypeSkillBase, bind(&CAttackState::OnTrigger, this, _1, _2, _3, _4 ) );
	}
}