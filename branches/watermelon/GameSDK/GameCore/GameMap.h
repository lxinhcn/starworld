#pragma once
#include "GameObject.h"
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#define BLOCK_ADD		1
#define BLOCK_DEL		2
#define BLOCK_EXCANGE	3
//////////////////////////////////////////////////////////////////////////
// MapBlock 格子管理
namespace XGC
{
	class CGameObject;
	class CORE_API CServerMap	: public CXObject
	{
		DECLARE_DYNAMICTYPE( CXObject, TypeMap );
	protected:
		float m_fCellWidth, m_fCellHeight;
		float m_fTransfromX, m_fTransfromY;

		float m_InvBW, m_InvBH;
		int	m_nCellCountW, m_nCellCountH;

		float m_fAreaWidth, m_fAreaHeight;
		int m_nAreaCountW, m_nAreaCountH;
		int m_nAreaWidthOfCell, m_nAreaHeightOfCell;		// 区域的宽度和高度，以Cell 为单位

		DEBUG_CODE( bool m_bChangeBlock );
		struct MapCell
		{
			union
			{
				struct
				{
					_byte block		: 1;
					_byte barrier	: 1;
					_byte data		: 6;
				};
				_byte mask;
			};
		};

		struct MapCell *m_pCells;	// 地砖数据
		struct MapCell **m_pCell;	// 二维地砖数据

		struct Trigger
		{
			_string strReciverName;
			int nTriggerType;
			int nTriggerCode;
		};

		struct Counter
		{
			Counter( int nCount )
				: nCounter( nCount )
			{
			}

			int nCounter;
		};

		struct Spoorer
		{
			Spoorer( _lpctstr lpszSpoorerName, xObject hSource )
				: strSpoorerName( lpszSpoorerName )
				, hMap( hSource )
			{
			}

			bool operator()( timer_t handle )
			{
				CServerMap* pMap = ObjectPool.GetObject< CServerMap >( hMap );
				if( pMap )
				{
					pMap->ExecuteTrigger( strSpoorerName );
				}

				return false;
			}

			_string strSpoorerName;
			xObject hMap;
		};

		struct AreaSet : public xgc_set< xObject >
		{
			int x;
			int y;
		}*m_pAreas;

		typedef xgc_multimap< _string, CXAction* > ReciverMap;
		typedef xgc_multimap< _string, Trigger > TriggerMap;
		typedef xgc_map< _string, Counter > CounterMap;
		typedef xgc_map< _string, Spoorer > SpoorerMap;

		ReciverMap m_ReciverMap;
		TriggerMap m_TriggerMap;
		CounterMap m_CounterMap;
		SpoorerMap m_SpoorerMap;
	public:
		typedef AreaSet::iterator	AreaIter;

	protected:
		CServerMap();
		virtual ~CServerMap()
		{
			DestroyMap();
		}

		// 初始化地图
		//---------------------------------------------------//
		// [1/4/2011 Albert]
		// Description:	初始化地图 
		//---------------------------------------------------//
		void InitializeMap( int nCellCountW, int nCellCountH, int nAreaWidthOfCell, int nAreaHeightOfCell, float fCellWidth, float fCellHeight, float fTransfromX, float fTransfromY );

		//---------------------------------------------------//
		// [1/4/2011 Albert]
		// Description:	销毁地图 
		//---------------------------------------------------//
		void DestroyMap();

	public:
		inline	int	GetCellCountW()const		{ return m_nCellCountW; }
		inline	int	GetCellCountH()const		{ return m_nCellCountH; }
		inline	int	GetAreaCountW()const		{ return m_nAreaCountW; }
		inline	int	GetAreaCountH()const		{ return m_nAreaCountH; }

		inline	float GetCellWidth()const	{ return m_fCellWidth; }
		inline	float GetCellHeight()const	{ return m_fCellHeight; }
		inline	float GetAreaWidth()const	{ return m_fAreaWidth; }
		inline	float GetAreaHeight()const	{ return m_fAreaHeight; }
		inline	float GetWorldWidth()const	{ return m_nCellCountW*m_fCellWidth; }
		inline	float GetWorldHeight()const	{ return m_nCellCountH*m_fCellHeight; }

		inline iPoint WorldToCell( float x, float y )const{ return iPoint( int((x+m_fTransfromX)/m_fCellWidth), int((y+m_fTransfromY)/m_fCellHeight ) ); }
		inline fPoint CellToWorld( int x, int y )const{ return fPoint( (x+0.5f)*m_fCellWidth - m_fTransfromX, (y+0.5f)*m_fCellHeight - m_fTransfromY ); }

		inline iPoint WorldToArea( float x, float y )const{ return iPoint( int((x+m_fTransfromX)/m_fAreaWidth), int((y+m_fTransfromY)/m_fAreaHeight) ); }
		inline fPoint AreaToWorld( int x, int y )const{ return fPoint( (x+0.5f)*m_fAreaWidth - m_fTransfromX, (y+0.5f)*m_fAreaHeight - m_fTransfromY ); }

		//---------------------------------------------------//
		// [1/5/2011 Albert]
		// Description:	在地图中添加场景对象 
		//---------------------------------------------------//
		bool Insert( CGameObject* pObject, const XVector3& vPosition );

		//---------------------------------------------------//
		// [1/5/2011 Albert]
		// Description:	从场景中移除对象 
		//---------------------------------------------------//
		void Remove( CGameObject* pObject );

		//---------------------------------------------------//
		// [1/5/2011 Albert]
		// Description:	创建计数器 
		//---------------------------------------------------//
		void CreateCounter( _lpctstr lpszCounterName, int nCount )
		{
			m_CounterMap.insert( CounterMap::value_type( lpszCounterName, Counter( nCount ) ) );
		}

		//---------------------------------------------------//
		// [1/5/2011 Albert]
		// Description:	创建追踪器 
		//---------------------------------------------------//
		void CreateSpoorer( _lpctstr lpszSpoorerName, float fSecond );

		//---------------------------------------------------//
		// [1/5/2011 Albert]
		// Description:	增加计数器 
		//---------------------------------------------------//
		void IncreaseCounter( _lpctstr lpszCounterName, int nInc )
		{
			CounterMap::iterator i = m_CounterMap.find( lpszCounterName );
			if( i == m_CounterMap.end() )
				return;

			Counter& c = i->second;
			c.nCounter -= nInc;
			if( c.nCounter == 0 )
			{
				ExecuteTrigger( lpszCounterName );
			}
		}

		void ExecuteTrigger( const _string& strSourceName )
		{
			TriggerMap::_Pairii tRange = m_TriggerMap.equal_range( strSourceName );
			for( TriggerMap::iterator t = tRange.first; t != tRange.second; ++t )
			{
				// 根据源的名字查找触发事件
				ReciverMap::_Pairii i = m_ReciverMap.equal_range( t->second.strReciverName );
				for( ReciverMap::iterator r = i.first; r != i.second; ++r )
				{
					r->second->ActiveAction( INVALID_OBJECT_ID, GetObjectID() );
				}
			}
		}

		//---------------------------------------------------//
		// [12/29/2010 Albert]
		// Description:	 根据世界坐标获取区域
		//---------------------------------------------------//
		inline AreaSet* GetArea( float x, float y )const;

		//---------------------------------------------------//
		// [12/29/2010 Albert]
		// Description:	获取区域 
		//---------------------------------------------------//
		inline AreaSet* GetArea( int x, int y )const;

		//---------------------------------------------------//
		// [8/7/2009 Albert]
		// Description:	移动对象
		//---------------------------------------------------//
		bool DynamicMoveTo( CGameObject* pObject, XVector3 &vPosition, bool bCollisionTest = true );

		//---------------------------------------------------//
		// [12/22/2010 Albert]
		// Description:	插入接收器
		//---------------------------------------------------//
		void InsertReciver( const _string& strReciverName, CXAction* pAction )
		{
			m_ReciverMap.insert( ReciverMap::value_type( strReciverName, pAction ) );
		}

		//---------------------------------------------------//
		// [12/22/2010 Albert]
		// Description:	 插入触发器 
		//---------------------------------------------------//
		void InsertTrigger( const _string& strSourceName, int nTriggerType, int nTriggerCode, const _string& strReciverName );

		//---------------------------------------------------//
		// [1/5/2011 Albert]
		// Description:	建立触发器 
		//---------------------------------------------------//
		void BuildTrigger( const _string& strSourceName, CGameObject* pServerObject );

	protected:
		//---------------------------------------------------//
		// [12/22/2010 Albert]
		// Description:	触发事件 
		//---------------------------------------------------//
		void TriggerReciver( xObject hSource, xObject hTarget, long_ptr lParam, int_ptr wParam, int nTriggerCode, CXAction* pAction )
		{
			if( wParam == nTriggerCode )
			{
				pAction->ActiveAction( hSource, GetObjectID() );
			}
		}

		//---------------------------------------------------//
		// [1/4/2011 Albert]
		// Description:	测试是否碰撞 
		//---------------------------------------------------//
		inline bool TestCollisionBlock( int x, int y );

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	对象所占格子发生改变时调用
		//---------------------------------------------------//
		bool ExchangeBlock( CGameObject* pObject, const iPoint& vOldPosition, const iPoint& vNewPosition, const iSize& iEyeshot );

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	通知格子内的所有对象,有人进入了视野
		// nObjID:		被通知的对象ID
		// pObj:		正在移动的对象
		//---------------------------------------------------//
		void NotifyEnterEyeshot( CGameObject* pObj, xObject nObjID );

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	通知格子内的所有对象,有人离开了视野
		// nObjID:		被通知的对象ID
		// pObj:		正在移动的对象
		//---------------------------------------------------//
		void NotifyLeaveEyeshot( CGameObject* pObj, xObject nObjID );

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	碰撞检测
		//---------------------------------------------------//
		bool TestCollision( int x0, int y0, int &x1, int &y1 );

	};
}
