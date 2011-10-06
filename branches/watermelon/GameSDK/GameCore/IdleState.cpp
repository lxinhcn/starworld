#include "StdAfx.h"
#include "IdleState.h"
#include "GameObject.h"
#include "ObjectControlerBase.h"
namespace XGC
{
	CIdleState::CIdleState( byte_t code, const GuardFunctor& Fn )
		: CStateBase( eStateType_Idle, code, Fn )
		, m_pControler( new CObjectIdleControler() )
	{
	}

	CIdleState::~CIdleState(void)
	{
		SAFE_DELETE( m_pControler );
	}

	//---------------------------------------------------//
	// [11/24/2010 Albert]
	// Description:	触发器事件发生时
	//---------------------------------------------------//
	void CIdleState::OnTrigger( xObject hSource, xObject hDest, long_ptr lParam, int_ptr wParam )
	{
		// ASSERT_RETURN( hSource == hDest, void(0) );
		switch( wParam )
		{
		case CGameObject::Event_Stand:
			mStateParams.bit1 = true;
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
	//	return:		true 进入状态成功， false进入状态失败
	//--------------------------------------------------------//
	bool CIdleState::OnEnter( CGameObject& Object, StateParams& Params )
	{
		mStateParams = static_cast< IdleStateParams& >( Params );

		Object.RegisteTrigger( TypeGameObject, bind(&CIdleState::OnTrigger, this, _1, _2, _3, _4 ) );
		m_pControler->Attach( &Object );
		m_pControler->Idle( mStateParams.fIdleTime );

		return true;
	}

	//--------------------------------------------------------//
	//	created:	21:1:2010   17:02
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	中断
	//--------------------------------------------------------//
	void CIdleState::OnInterrupt()
	{
		m_pControler->PauseUpdate();
	}

	//--------------------------------------------------------//
	//	created:	21:1:2010   17:02
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	中断恢复现场
	//--------------------------------------------------------//
	void CIdleState::OnIret()
	{
		m_pControler->ResumeUpdate();
	}

	//--------------------------------------------------------//
	//	created:	21:1:2010   14:35
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	离开状态
	//	pEntity:	进入状态的对象指针
	//--------------------------------------------------------//
	void CIdleState::OnLeave( CGameObject& Object )
	{
		m_pControler->Detach();
		Object.DismissTrigger( TypeGameObject, bind(&CIdleState::OnTrigger, this, _1, _2, _3, _4 ) );
	}
}
