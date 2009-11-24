#pragma once
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif

#include <hash_map>
#include "Loki/Allocator.h"
#include "Loki/Function.h"
#include "XObjectPool.h"

namespace XGC
{
	enum TRIGGER_EVENT
	{
		trigger_event_destroy,
		trigger_event_dead,
		trigger_event_attack,
		trigger_event_hit,
		_trigger_event_count,
	};

	class CORE_API CGameTriggerMgr
	{
	public:
		CGameTriggerMgr(void);
		~CGameTriggerMgr(void);

	private:
		enum FuncType{ TypeMemberFunc, TypeFunction };
		struct TriggerCall
		{
			TriggerCall( const FuncType nType )
				: _nType( nType )
			{

			}

			virtual int operator()( _uint32 hSource, long_ptr lParam ) = 0;

			virtual bool Compair( const TriggerCall& rsh )const = 0;

			const FuncType _nType;
		};

		template< typename PointToClass, typename PointToMember >
		struct MemberFuncHandler : public TriggerCall
		{
			MemberFuncHandler( const PointToClass &C, PointToMember M )
				: TriggerCall( TypeMemberFunc )
				, pC( C )
				, pM( M )
			{

			}

			MemberFuncHandler( const MemberFuncHandler& rsh )
				: TriggerCall( TypeMemberFunc )
				, pC( rsh.pC )
				, pM( rsh.pM )
			{

			}

			virtual bool Compair( const TriggerCall& rsh )const
			{
				if( _nType != rsh._nType )
					return false;

				const MemberFuncHandler& MemberFunc = static_cast< const MemberFuncHandler& >( rsh );
				return pC == MemberFunc.pC && pM == MemberFunc.pM;
			}

			__inline bool operator==( const MemberFuncHandler& rsh )
			{
				return pC == rsh.pC && pM == rsh.pM;
			}

			__inline bool operator!=( const MemberFuncHandler& rsh )
			{
				return !operator==( rsh );
			}

			int operator()( _uint32 hSource, long_ptr lParam )
			{
				return (pC->*pM)( hSource, lParam );
			}
		private:
			PointToClass pC;
			PointToMember pM;
		};

		template< typename TriggerFunc >
		struct FunctionHandler : public TriggerCall
		{
			FunctionHandler( TriggerFunc &Func )
				: TriggerCall( TypeFunction )
				, _Func( Func )
			{
			}

			virtual bool Compair( const TriggerCall& rsh )const
			{
				if( _nType != rsh._nType )
					return false;

				const FunctionHandler& Func = static_cast< const FunctionHandler& >( rsh );
				return Func._Func == _Func;
			}

			__inline bool operator==( const TriggerFunc &Func )const
			{
				return Func._Func == _Func;
			}

			__inline bool operator!=( const TriggerFunc &Func )const
			{
				return Func._Func != _Func;
			}

			int operator()( _uint32 hSource, long_ptr lParam )
			{
				return _Func( hSource, lParam );
			}
		private:
			TriggerFunc _Func;
		};

		void RegisterTrigger_i( _uint32 hSource, _uint16 nEvent, TriggerCall* pCall );

		void DismissTrigger_i( _uint32 hSource, _uint16 nEvent, TriggerCall *pCall );

	public:
		//---------------------------------------------------//
		// [9/8/2009 Albert]
		// Description:	注册触发器
		//---------------------------------------------------//
		template< class TriggerFunc >
		void RegisterTrigger( _uint32 hSource, _uint16 nEvent, TriggerFunc &call )
		{
			RegisterTrigger_i( hSource, nEvent, new FunctionHandler< TriggerFunc >( call ) );
		}

		template< class Func, class Member >
		void RegisterTrigger( _uint32 hSource, _uint16 nEvent, Func *pFunc, Member *pMember )
		{
			RegisterTrigger_i( hSource, nEvent, new MemberFuncHandler< Func, Member >( pFunc, pMember ) );
		}

		//---------------------------------------------------//
		// [9/8/2009 Albert]
		// Description:	解除触发器
		//---------------------------------------------------//
		template< class TriggerFunc >
		void DismissTrigger( _uint32 hSource, _uint16 nEvent, TriggerFunc &call )
		{
			FunctionHandler< TriggerCall > handler( call );
			DismissTrigger_i( hSource, nEvent, &handler );
		}

		template< class Func, class Member >
		void DismissTrigger( _uint32 hSource, _uint16 nEvent, Func pFunc, Member pMember )
		{
			MemberFuncHandler< Func, Member > handler( pFunc, pMember );
			DismissTrigger_i( hSource, nEvent, &handler );
		}

		//---------------------------------------------------//
		// [9/8/2009 Albert]
		// Description:	触发事件
		//---------------------------------------------------//
		void TriggerEvent( _uint32 nSourceID, _uint16 nEventID, long_ptr lParam = 0 );

	private:
		typedef std::list< TriggerCall*, ALLOCATOR< TriggerCall* > >		TriggerList;
		typedef std::vector< TriggerList, ALLOCATOR< TriggerList > >	EventVec;
		typedef stdext::hash_map< _uint32, EventVec >	SourceHash;

		SourceHash m_SourceHash;
	};

	//--------------------------------------------------------------//
	//	created:	25:9:2009   11:06
	//	filename: 	ServerTriggerMgr.h
	//	author:		Albert.xu

	//	Descript : 	触发器的动作适配器
	//--------------------------------------------------------------//
	struct CORE_API ActionAdapter
	{
		_uint32 m_hAction;
		ActionAdapter( _uint32 hAction )
			: m_hAction( hAction )
		{

		}

		int operator()( _uint32 hSource, long_ptr lParam )
		{
			CXAction* pAction = static_cast< CXAction* >( CXObjectPool::GetInstance().GetObj( m_hAction, TypeAction ) );
			if( pAction )
			{
				CXObject *pObj = CXObjectPool::GetInstance().GetObj( hSource );
				pAction->SetOwnerParam( 0, (int)lParam );
				pAction->ResetFlag();
				return pAction->DoAction( pObj );
			}
			return 0;
		}

		bool operator==( const ActionAdapter &rsh )const
		{ 
			return m_hAction == rsh.m_hAction; 
		}
	};

	//--------------------------------------------------------------//
	//	created:	25:9:2009   11:06
	//	filename: 	ServerTriggerMgr.h
	//	author:		Albert.xu

	//	Descript : 	触发器的动作适配器，通过概率触发
	//--------------------------------------------------------------//
	struct CORE_API ActionAdapterRate
	{
		_uint32 m_hAction;
		_uint32 m_nRandom;
		ActionAdapterRate( _uint32 hAction, _uint32 nRandom )
			: m_hAction( hAction )
			, m_nRandom( nRandom )
		{

		}

		int operator()( _uint32 hSource, long_ptr lParam )
		{
			if( XMath::Random(0, 1000) <= m_nRandom )
			{
				CXAction* pAction = static_cast< CXAction* >( CXObjectPool::GetInstance().GetObj( m_hAction, TypeAction ) );
				if( pAction )
				{
					CXObject *pObj = CXObjectPool::GetInstance().GetObj( hSource );
					pAction->SetOwnerParam( 0, (int)lParam );
					pAction->ResetFlag();
					return pAction->DoAction( pObj );
				}
			}
			return 0;
		}
		bool operator==( const ActionAdapterRate &rsh )const
		{ 
			return m_hAction == rsh.m_hAction; 
		}
	};
};