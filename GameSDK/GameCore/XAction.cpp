#include "StdAfx.h"
#include "XAction.h"
namespace XGC
{
	extern CORE_API timer& ThisTimer();
	//////////////////////////////////////////////////////////////////////////
	// Action
	CXAction::CXAction( ActionFunc Functor, cbuffer Params, float fDelay )
		: CXObject( false, false, false )
		, m_Functor( Functor )
		, m_Params( Params )
		, m_fDelay( fDelay )
	{

	}

	CXAction::~CXAction()
	{
	}

	int CXAction::ActiveAction( xObject hSource, xObject hTarget )
	{
		if( m_fDelay == 0.0f )
		{
			return DoAction( hSource, hTarget );
		}
		else
		{
			struct OnTimer
			{
				xObject _hSource, _hTarget, _hAction;
				OnTimer( xObject hSource, xObject hTarget, xObject hAction )
					: _hSource( hSource )
					, _hTarget( hTarget )
					, _hAction( hAction )
				{
				}

				//---------------------------------------------------//
				// [9/13/2009 Albert]
				// Description:	定时器事件
				//---------------------------------------------------//
				bool operator()( unsigned int handle )
				{
					CXAction* pAction = static_cast< CXAction* >( ObjectPool.GetObject( _hAction ) );
					if( pAction )
					{
						pAction->DoAction( _hSource, _hTarget );
					}
					return true;
				}
			};

			// 插入状态作用定时器
			ThisTimer().insert_event( 
				OnTimer( hSource, hTarget, GetObjectID() ), 
				1, 
				TIMER_SECONDS( 0.0f ),
				TIMER_SECONDS( m_fDelay ) );
		}
		return 0;
	}

	int CXAction::DoAction( xObject hSource, xObject hTarget )
	{
		return m_Functor( hSource, hTarget, m_Params.base(), m_Params.capacity() );
	}

	//////////////////////////////////////////////////////////////////////////
	// Status class
	//////////////////////////////////////////////////////////////////////////
	CXStatusInstance::CXStatusInstance()
	{
	}

	CXStatusInstance::~CXStatusInstance()
	{
	}

	//---------------------------------------------------//
	// [12/14/2010 Albert]
	// Description:	添加作用 
	//---------------------------------------------------//
	void CXStatusInstance::AppendEffect( const StatusFunc& Functor, cbuffer Params )
	{
		effect e = { Params, Functor };
		m_EffectContainer.push_back( e );
	}

	//---------------------------------------------------//
	// [11/19/2010 Albert]
	// Description:	执行状态 
	//---------------------------------------------------//
	void CXStatusInstance::DoStatus( xObject nIdentity, long_ptr& pUserdata, eStatus eFlag )
	{
		for( EffectContainer::iterator i = m_EffectContainer.begin(); i != m_EffectContainer.end(); ++i )
		{
			i->mFunctor( nIdentity, (int)eFlag, pUserdata, i->mParams.base(), i->mParams.capacity() );
		}
	}

	CXStatus::~CXStatus()
	{
		ThisTimer().remove_event( m_hUpdateTimerHandler );
		m_hUpdateTimerHandler = INVALID_TIMER_HANDLE;
	}

	void CXStatus::InstallTimer( float fLifetime, float fInterval, float fDelay )
	{
		DoStatus( status_append );

		if( fLifetime != 0.0f )
		{
			m_hLifetimeHandler = ThisTimer().insert_event( bind( &CXStatus::OnStatusOver, this, _1 ), 1, -1, TIMER_SECONDS(fLifetime) );
		}

		if( fInterval != 0.0f )
		{
			// 插入状态作用定时器
			m_hUpdateTimerHandler = ThisTimer().insert_event( 
				bind( &CXStatus::OnUpdate, this, _1 ), 
				int((fLifetime - fDelay)/fInterval), 
				TIMER_SECONDS( fInterval ),
				TIMER_SECONDS( fDelay ) );
		}

	}

	bool CXStatus::OnUpdate( timer_t handle )
	{
		_tevent *event = ThisTimer().get_event( handle );
		m_pInstance->DoStatus( GetParent(), m_Userdata, status_active );

		return false;
	}

	bool CXStatus::OnStatusOver( timer_t handle )
	{
		ThisTimer().remove_event( m_hUpdateTimerHandler );
		ThisTimer().remove_event( m_hLifetimeHandler );
		m_hUpdateTimerHandler = INVALID_TIMER_HANDLE;
		m_hLifetimeHandler = INVALID_TIMER_HANDLE;

		Destroy();
		return true;
	}
}
