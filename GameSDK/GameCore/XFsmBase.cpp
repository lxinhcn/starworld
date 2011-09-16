#pragma once
#include "stdafx.h"
#include "header.h"
#include "GameObject.h"

namespace XGC
{
	CStateBase::CStateBase( byte_t type, byte_t code, const GuardFunctor& Fn )
		: m_GuardFunctor( Fn )
		, m_StateMachinePtr( xgc_nullptr )
		, m_StateType( type )
		, m_StateCode( code )
	{
	}

	CStateBase::~CStateBase(void)
	{

	}

	//--------------------------------------------------------//
	//	created:	21:1:2010   14:34
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	更新状态
	//	pEntity:	进入状态的对象指针
	//--------------------------------------------------------//
	bool CStateBase::OnUpdate( CGameObject& Object )
	{
		if( m_GuardFunctor )
			return m_GuardFunctor( Object, *m_StateMachinePtr, GetParams() );

		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	CStateMachine::CStateMachine( byte_t code, int nStateCount, const GuardFunctor& Fn )
		: CStateBase( eStateType_Fsm, code, Fn )
		, m_pCurrentState( xgc_nullptr )
		, m_pStatePool( xgc_nullptr )
		, m_nStateCount( 0 )
	{
		VERIFY( InitStateMachine( nStateCount ) );
	}

	CStateMachine::~CStateMachine()
	{
		for( int i = 0; i < m_nStateCount; ++i )
			SAFE_DELETE( m_pStatePool[i] );
		free( m_pStatePool );
	}

	bool CStateMachine::InitStateMachine( int nStateCount )
	{
		m_pStatePool = (CStateBase**)malloc( sizeof(CStateBase*)*nStateCount );
		memset( m_pStatePool, 0, sizeof(m_pStatePool) );

		m_nStateCount = nStateCount;
		return true;
	}

	unsigned int CStateMachine::GetStateLifetime()const
	{
		return clock() - m_beginTime;
	}

	//--------------------------------------------------------//
	//	created:	22:1:2010   14:10
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	无条件迁移状态
	//--------------------------------------------------------//
	bool CStateMachine::Switch( CGameObject& Object, StateParams& Params )
	{
		if( Params.type >= m_nStateCount )
			return false;

		CStateBase *pNewState = m_pStatePool[Params.type];
		CStateBase *pOldState = m_pCurrentState;
		
		if( pNewState == NULL )
			return false;

		if( pOldState )
			pOldState->OnLeave( Object );

		m_pCurrentState = pNewState;

		pNewState->OnEnter( Object, Params );

		m_beginTime = clock();
		return true;
	}

	//---------------------------------------------------//
	// [12/18/2010 Albert]
	// Description:	更新状态 
	//---------------------------------------------------//
	bool CStateMachine::UpdateState( CGameObject& Object )
	{
		return OnUpdate( Object );
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
	bool CStateMachine::OnEnter( CGameObject& Object, StateParams& Params )
	{
		return true;
	}

	//--------------------------------------------------------//
	//	created:	21:1:2010   14:34
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	更新状态
	//	pEntity:	进入状态的对象指针
	//--------------------------------------------------------//
	bool CStateMachine::OnUpdate( CGameObject& Object )
	{
		if( __super::OnUpdate( Object ) )
			return true;

		if( m_pCurrentState )
			return m_pCurrentState->OnUpdate( Object );

		return false;
	}

	//--------------------------------------------------------//
	//	created:	21:1:2010   14:35
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	离开状态
	//	pEntity:	进入状态的对象指针
	//--------------------------------------------------------//
	void CStateMachine::OnLeave( CGameObject& Object )
	{
		m_pCurrentState->OnLeave( Object );
	}
}