#pragma once
#include "XFsmBase.h"

namespace XGC
{
	class CGameObject;
	class CObjectIdleControler;

	struct CORE_API IdleStateParams : public StateParams
	{
		float fIdleTime;
	};

	class CORE_API CIdleState :	public CStateBase
	{
	public:
		CIdleState( byte_t code, const GuardFunctor& Fn );
		~CIdleState(void);

		enum { IdleTimeout, };
	protected:
		//---------------------------------------------------//
		// [11/24/2010 Albert]
		// Description:	触发器事件发生时
		//---------------------------------------------------//
		void OnTrigger( xObject hSource, xObject hDest, long_ptr lParam, int_ptr wParam );

		//--------------------------------------------------------//
		//	created:	21:1:2010   14:33
		//	filename: 	State
		//	author:		Albert.xu
		//
		//	purpose:	进入状态前调用
		//	pEntity:	进入状态的对象指针
		//	return:		TRUE 进入状态成功， false进入状态失败
		//--------------------------------------------------------//
		virtual bool OnEnter( CGameObject& Object, StateParams& Params );

		//--------------------------------------------------------//
		//	created:	21:1:2010   17:02
		//	filename: 	State
		//	author:		Albert.xu
		//
		//	purpose:	中断
		//--------------------------------------------------------//
		virtual void OnInterrupt();

		//--------------------------------------------------------//
		//	created:	21:1:2010   17:02
		//	filename: 	State
		//	author:		Albert.xu
		//
		//	purpose:	中断恢复现场
		//--------------------------------------------------------//
		virtual void OnIret();

		//--------------------------------------------------------//
		//	created:	21:1:2010   14:35
		//	filename: 	State
		//	author:		Albert.xu
		//
		//	purpose:	离开状态
		//	pEntity:	进入状态的对象指针
		//--------------------------------------------------------//
		virtual void OnLeave( CGameObject& Object );

		//--------------------------------------------------------------//
		//	created:	2:2:2010   14:37
		//	filename: 	FsmBase.h
		//	author:		Albert.xu

		//	Description : 	获取参数信息
		//--------------------------------------------------------------//
		virtual StateParams& GetParams(){ return mStateParams; }

	private:
		CObjectIdleControler*	m_pControler;
		IdleStateParams			mStateParams;
	};
}