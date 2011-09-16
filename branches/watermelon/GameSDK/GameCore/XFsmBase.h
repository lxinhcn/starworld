#pragma once
#include "header.h"
#ifndef _USE_BOOST
	#include <functional>
	using namespace std;
	using namespace std::tr1;
	using namespace std::tr1::placeholders;
#else
	#include "boost\function.hpp"
	#include "boost\bind.hpp"
	using namespace boost;
#endif

namespace XGC
{
	class CGameObject;
	class CStateMachine;
	
	struct CORE_API StateParams
	{
		byte_t	type;
		byte_t	code;
		union
		{
			struct
			{
				byte_t bit1 : 1;
				byte_t bit2 : 1;
				byte_t bit3 : 1;
				byte_t bit4 : 1;
				byte_t bit5 : 1;
				byte_t bit6 : 1;
				byte_t bit7 : 1;
				byte_t bit8 : 1; // time out flags
			};
			byte_t mark;
		};
	};

	typedef bool (*GuardFunction)( CGameObject& Object, CStateMachine& StateMachine, StateParams& Params );
	typedef function< bool ( CGameObject& Object, CStateMachine& Fsm, StateParams& Params ) >	GuardFunctor;// 守护条件

	enum { eStateType_Fsm, eStateType_Attack, eStateType_Move, eStateType_Idle };
	class CORE_API CStateBase
	{
		friend class CStateMachine;
	public:
		explicit CStateBase( byte_t type, byte_t code, const GuardFunctor& Fn = xgc_nullptr );

		//--------------------------------------------------------------//
		//	created:	2:2:2010   14:37
		//	filename: 	FsmBase.h
		//	author:		Albert.xu

		//	Description : 	获取参数信息
		//--------------------------------------------------------------//
		virtual StateParams& GetParams() = 0;

		__inline byte_t GetCode(){ return m_StateCode; }
		__inline byte_t GetType(){ return m_StateType; }

	protected:
		virtual ~CStateBase(void);

		//--------------------------------------------------------//
		//	created:	21:1:2010   14:33
		//	filename: 	State
		//	author:		Albert.xu
		//
		//	purpose:	进入状态前调用
		//	pEntity:	进入状态的对象指针
		//	return:		TRUE 进入状态成功， false进入状态失败
		//--------------------------------------------------------//
		virtual bool OnEnter( CGameObject& pObject, StateParams& Params ) = 0;

		//--------------------------------------------------------//
		//	created:	21:1:2010   14:34
		//	filename: 	State
		//	author:		Albert.xu
		//
		//	purpose:	更新状态
		//	pEntity:	进入状态的对象指针
		//--------------------------------------------------------//
		virtual bool OnUpdate( CGameObject& pObject );

		//--------------------------------------------------------//
		//	created:	21:1:2010   14:35
		//	filename: 	State
		//	author:		Albert.xu
		//
		//	purpose:	离开状态
		//	pEntity:	进入状态的对象指针
		//--------------------------------------------------------//
		virtual void OnLeave( CGameObject& Object ) = 0;

		//---------------------------------------------------//
		// [12/17/2010 Albert]
		// Description:	获取当前状态 
		//---------------------------------------------------//
		virtual CStateBase* GetCurrentState(){ return this; }

		////---------------------------------------------------//
		//// [12/17/2010 Albert]
		//// Description:	获取当前状态 
		////---------------------------------------------------//
		//virtual CStateBase* GetCurrentState(){ return this; }
	protected:
		byte_t m_StateType; // 状态类型
		byte_t m_StateCode; // 状态代码
		GuardFunctor m_GuardFunctor;
		CStateMachine *m_StateMachinePtr;
	};

	class CORE_API CStateMachine : public CStateBase
	{
	private:
		// 不允许拷贝构造
		CStateMachine( const CStateMachine& rsh );

	public:
		CStateMachine( byte_t code, int nStateCount, const GuardFunctor& Fn = xgc_nullptr );

		~CStateMachine();

		//---------------------------------------------------//
		// [12/11/2010 Albert]
		// Description:	获取状态运行时间 
		//---------------------------------------------------//
		unsigned int GetStateLifetime()const;

		//--------------------------------------------------------//
		//	created:	22:1:2010   14:10
		//	filename: 	State
		//	author:		Albert.xu
		//
		//	purpose:	设置状态映射表
		//--------------------------------------------------------//
		template< class StateType >
		bool CreateState( int nIndex, byte_t nCode, const GuardFunctor& Fn )
		{
			if( nIndex >= m_nStateCount ) 
				return false;

			m_pCurrentState = m_pStatePool[nIndex] = new StateType( nCode, Fn );
			m_pCurrentState->m_StateMachinePtr = this;
			return true;
		}

		//--------------------------------------------------------//
		//	created:	22:1:2010   14:10
		//	filename: 	State
		//	author:		Albert.xu
		//
		//	purpose:	无条件迁移状态
		//--------------------------------------------------------//
		bool Switch( CGameObject& Object, StateParams& Params );

		//---------------------------------------------------//
		// [12/18/2010 Albert]
		// Description:	更新状态 
		//---------------------------------------------------//
		bool UpdateState( CGameObject& Object );

		//---------------------------------------------------//
		// [12/17/2010 Albert]
		// Description:	获取当前状态 
		//---------------------------------------------------//
		CStateBase* GetCurrentState(){ return m_pCurrentState?m_pCurrentState->GetCurrentState():xgc_nullptr; }

	protected:
		//---------------------------------------------------//
		// [12/11/2010 Albert]
		// Description:	初始化状态机 
		//---------------------------------------------------//
		bool InitStateMachine( int nStateCount );

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
		//	created:	21:1:2010   14:34
		//	filename: 	State
		//	author:		Albert.xu
		//
		//	purpose:	更新状态
		//	pEntity:	进入状态的对象指针
		//--------------------------------------------------------//
		virtual bool OnUpdate( CGameObject& Object );

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
		virtual StateParams& GetParams(){ static StateParams Params = { -1, 0, 0 }; return Params; }
		
	private:
		int			m_nStateCount;
		clock_t		m_beginTime;
		CStateBase	*m_pCurrentState;
		CStateBase	**m_pStatePool;

		xgc_stack< CStateBase* > m_InterruptStack;
	};
}