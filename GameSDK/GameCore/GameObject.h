#pragma once
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#include <bitset>
#include "GameMap.h"

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
	class CObjectControlerBase;
	//////////////////////////////////////////////////////////////////////////
	// 基础对象类， 所有屏幕上的对象都从此类派生
	typedef function< void( xObject, xObject, long_ptr, int_ptr ) > TriggerFunctor;

	class CORE_API CGameObject	:	public CXObject
	{
	public:
		DECLARE_DYNAMICTYPE(CXObject,TypeGameObject);
		friend class CServerMap;
		friend class CStateMachine;
	public:
		CGameObject();
		~CGameObject();

		enum { Flag_Move = 1, };
		enum { Event_MoveBegin, Event_MoveStep, Event_MoveStop, Event_MoveArrived, Event_Stand, Event_EnterEyeshot, Event_LeaveEyeshot };

		// 直接世界坐标
		inline void GetPosition( float fPos[3] )const
		{
			fPos[0] = m_Position[0];
			fPos[1] = m_Position[1];
			fPos[2] = m_Position[2];
		}

		// 直接世界坐标
		inline void GetPosition( XVector3& Position )const
		{
			Position = m_Position;
		}

		// 直接世界坐标
		inline const XVector3& GetPosition()const
		{
			return m_Position;
		}

		//////////////////////////////////////////////////////////////////////////
		// 对象的方向
		//////////////////////////////////////////////////////////////////////////
		inline void SetDirection( float fPosX, float fPosY, float fPosZ )
		{
			m_Direction.SetValue( fPosX, fPosY, fPosZ );
		}

		inline void SetDirection( const float fPos[3] )
		{
			m_Direction[0] = fPos[0];
			m_Direction[1] = fPos[1];
			m_Direction[2] = fPos[2];
		}

		// 直接世界坐标
		inline void GetDirection( float fPos[3] )const
		{
			fPos[0] = m_Direction[0];
			fPos[1] = m_Direction[1];
			fPos[2] = m_Direction[2];
		}

		inline void SetObjectFlag( unsigned int index, bool value )
		{ 
			m_Flags.set( index, value ); 
		}
		inline bool GetObjectFlag( unsigned int index ){ return m_Flags.test( index ); }
		// 直接世界坐标
		inline void GetDirection( XVector3& Position )const
		{
			Position = m_Direction;
		}

		// 直接世界坐标
		inline const XVector3& GetDirection()const
		{
			return m_Direction;
		}

		inline float GetPosX()const{ return m_Position.x; }
		inline float GetPosY()const{ return m_Position.y; }
		inline float GetPosZ()const{ return m_Position.z; }

		inline float GetWidth()const{ return 1.0f; }
		inline float GetHeight()const{ return 1.0f; }

		//////////////////////////////////////////////////////////////////////////

		//---------------------------------------------------//
		// [10/12/2010 Albert]
		// Description:	注册触发器
		//---------------------------------------------------//
		void RegisteTrigger( unsigned int nGroup, const TriggerFunctor &Call );

		//---------------------------------------------------//
		// [10/12/2010 Albert]
		// Description:	解除触发器
		//---------------------------------------------------//
		template< class Functor >
		void DismissTrigger( unsigned int nGroup, const Functor &Call )
		{
			TriggerSignal::_Pairii i = m_TriggerSignal.equal_range( nGroup );
			for( TriggerSignal::iterator j = i.first ; j != i.second; ++j )
			{
				if( j->second == Call )
				{
					m_TriggerSignal.erase( j );
					break;
				}
			}
		}

		//---------------------------------------------------//
		// [11/23/2010 Albert]
		// Description:	触发事件 
		//---------------------------------------------------//
		void Trigger( unsigned int nEvent, xObject hSource, long_ptr lParam, int_ptr wParam );

		//////////////////////////////////////////////////////////////////////////
		//---------------------------------------------------//
		// [11/24/2010 Albert]
		// Description:	重置控制器 
		//---------------------------------------------------//
		void ResetControler( CObjectControlerBase* pControler ){ m_pControler = pControler; }

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	对象进入视野
		// pObject	:	进入视野的对象指针
		//---------------------------------------------------//
		virtual void EnterEyeshot( CGameObject* pObject );

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	对象离开视野
		// pObject	:	离开视野的对象指针
		//---------------------------------------------------//
		virtual void LeaveEyeshot( CGameObject* pObject );

	protected:
		//////////////////////////////////////////////////////////////////////////
		// 对象位置
		inline void SetPosition( float fPosX, float fPosY, float fPosZ )
		{
			m_Position.SetValue( fPosX, fPosY, fPosZ );
		}

		inline void SetPosition( const float fPos[3] )
		{
			m_Position[0] = fPos[0];
			m_Position[1] = fPos[1];
			m_Position[2] = fPos[2];
		}

		//---------------------------------------------------//
		// [11/8/2010 Albert]
		// Description:	进入场景 
		//---------------------------------------------------//
		virtual bool PreEnterMap( CServerMap* pScene ){ return true; }

		//---------------------------------------------------//
		// [11/8/2010 Albert]
		// Description:	离开场景 
		//---------------------------------------------------//
		virtual bool PreLeaveMap( CServerMap* pScene ){ return true; }

		//---------------------------------------------------//
		// [11/8/2010 Albert]
		// Description:	进入场景 
		//---------------------------------------------------//
		virtual void OnEnterMap( CServerMap* pScene ){}

		//---------------------------------------------------//
		// [11/8/2010 Albert]
		// Description:	离开场景 
		//---------------------------------------------------//
		virtual void OnLeaveMap( CServerMap* pScene ){}

		//---------------------------------------------------//
		// [1/5/2011 Albert]
		// Description:	获取迭代器 
		//---------------------------------------------------//
		inline CServerMap::AreaIter AreaIterator()const{ return m_AreaIterator; }

		//---------------------------------------------------//
		// [1/5/2011 Albert]
		// Description:	设置当前区域的迭代器 
		//---------------------------------------------------//
		inline void AreaIterator( const CServerMap::AreaIter& iter ){ m_AreaIterator = iter; }
	private:
		XVector3	m_Position;
		XVector3	m_Direction;

	protected:
		typedef xgc_multimap< unsigned int, TriggerFunctor >	TriggerSignal;

		std::bitset< 32 >		m_Flags;
		CObjectControlerBase*	m_pControler;
		TriggerSignal			m_TriggerSignal;
		CServerMap::AreaIter	m_AreaIterator;
	};

	extern CORE_API timer& ThisTimer();
	class CORE_API CWatcherObject : public CGameObject
	{
		DECLARE_DYNAMICTYPE( CGameObject, TypeWatcher )
	public:
		CWatcherObject( int nCount, float fRaduis, float fRate, xType FillterType )
			: mRaduis( fRaduis )
			, mFillterType( FillterType )
			, mCount( nCount )
			, mScanTimerHandler( INVALID_TIMER_HANDLE )
		{
			mScanTimerHandler = ThisTimer().insert_event( bind( &CWatcherObject::ScanRange, this, _1 ), -1, TIMER_SECONDS(fRate) );
		}

		~CWatcherObject()
		{
			ThisTimer().remove_event( mScanTimerHandler );
			mScanTimerHandler = INVALID_TIMER_HANDLE;
		}
	private:
		bool ScanRange( timer_t handler )
		{
			for( ObjectSet::iterator i = mObjectSet.begin(); i != mObjectSet.end(); ++i )
			{
				CGameObject* pObject = ObjectPool.GetObject< CGameObject >( *i );
				if( pObject == xgc_nullptr )
					continue;

				XVector3 vDistance = pObject->GetPosition() - GetPosition();
				if( vDistance.Length() <= mRaduis )
				{
					Trigger( TypeWatcher, GetObjectID(), xgc_nullptr, 0 );
					if( --mCount == 0 )
						Destroy();
					break;
				}
			}

			return false;
		}

		//---------------------------------------------------//
		// [11/10/2010 Albert]
		// Description:	进入视野 
		//---------------------------------------------------//
		void EnterEyeshot( CGameObject* pObject )
		{
			if( pObject->IsTypeOf( mFillterType ) )
			{
				mObjectSet.insert( pObject->GetObjectID() );
			}
		}

		//---------------------------------------------------//
		// [11/10/2010 Albert]
		// Description:	离开视野 
		//---------------------------------------------------//
		void LeaveEyeshot( CGameObject* pObject )
		{
			if( pObject->IsTypeOf( mFillterType ) )
			{
				mObjectSet.erase( pObject->GetObjectID() );
			}
		}

	private:
		typedef xgc_set< xObject > ObjectSet;
		int		mCount;
		xType	mFillterType;
		float	mRaduis;
		timer_t mScanTimerHandler;
		ObjectSet mObjectSet;
	};

}