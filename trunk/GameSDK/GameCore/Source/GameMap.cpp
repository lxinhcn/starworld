#include "StdAfx.h"
#include <math.h>
#include "GameMap.h"
#include "GameActorAI.h"
//////////////////////////////////////////////////////////////////////////
// CGameMap
// 初始化地图
void CGameMap::InitializeMap( _uint32 nBlockWidth, _uint32 nBlockHeight, SIZE siBlockSize, POINT ptTransfrom )
{
	m_pBlockArray = new CMapBlock[nBlockWidth*nBlockHeight];

	m_siMapSize		= xgcSize( nBlockWidth, nBlockHeight );
	m_siBlockSize	= siBlockSize;
	m_ptTransfrom	= ptTransfrom;

	m_InvBW = 1.0f/siBlockSize.cx;
	m_InvBH = 1.0f/siBlockSize.cy;
}

void CGameMap::DestroyMap()
{
	m_siMapSize		= xgcSize( 0, 0 );
	m_siBlockSize	= xgcSize( 0, 0 );
	m_ptTransfrom	= xgcPoint( 0, 0 );
	m_InvBW = 0.0f;
	m_InvBH = 0.0f;

	SAFE_DELETE_ARRAY( m_pBlockArray );
}

inline CMapBlock* CGameMap::GetBlock( int x, int y )const
{
	if( x < 0 || x >= m_siMapSize.cx || y < 0 || y >= m_siMapSize.cy )	return NULL;

	return m_pBlockArray?m_pBlockArray + (y*m_siMapSize.cx+x):NULL;
}

//// 得到当前矩形区域内的所有格子
//// rc		:	矩形区域
//// retQueue	:	返回的格子队列
//int CGameMap::GetCollisionBlock( const xgcRect& rc, CBlockList& retList )const
//{
//	int by = rc.top/(int)GetBlockHeight();
//	int bx = rc.left/(int)GetBlockWidth();
//
//	int ey = rc.bottom/(int)GetBlockHeight();
//	int ex = rc.right/(int)GetBlockWidth();
//	for( int y = by; y <= ey; y++ )
//	{
//		for( int x = bx; x <= ex; x++ )
//		{
//			retList.push_back( GetBlock( x, y ) );
//		}
//	}
//	return retList.size();
//}

//bool CGameMap::GetCollisionBlock( const xgcRect& rc, fnBlockCallback callback )const
//{
//	int by = rc.top/GetBlockHeight();
//	int bx = rc.left/GetBlockWidth();
//
//	int ey = rc.bottom/GetBlockHeight();
//	int ex = rc.right/GetBlockWidth();
//	for( int y = by; y <= ey; y++ )
//	{
//		for( int x = bx; x <= ex; x++ )
//		{
//			if( callback( GetBlock( x, y ) ) == false )
//				return false;
//		}
//	}
//	return true;
//}

xgcRect CGameMap::GetBlockArea( const xgcRect& rcCollision )const
{
	xgcRect rc = WorldToMap( rcCollision );

	int bx = int( ( rc.left + ( m_siBlockSize.cx >> 1 ) )*m_InvBW );
	int by = int( ( rc.top + ( m_siBlockSize.cy >> 1) )*m_InvBH );

	int ex = int( rc.Width()*m_InvBW + bx );
	int ey = int( rc.Height()*m_InvBH + by );

	return xgcRect( bx, by, ex, ey );
}

//---------------------------------------------------//
// [8/7/2009 Albert]
// Description:	获取对象占用的格子范围
//---------------------------------------------------//
xgcRect	CGameMap::GetBlockArea( float fPosX, float fPosY, float fRadius )const
{
	// WorldToMap
	fPosX -= m_ptTransfrom.x;
	fPosY -= m_ptTransfrom.y;

	float x0 = (fPosX - fRadius)*m_InvBW;
	float y0 = (fPosY - fRadius)*m_InvBH;
	float x1 = (fPosX + fRadius)*m_InvBW;
	float y1 = (fPosY + fRadius)*m_InvBH;

	return xgcRect( int(x0), int(y0), int(x1), int(y1) );
}

int CGameMap::GetBlockList( const xgcRect& rcBlockArea, CBlockList& ret )const
{
	for( int y = rcBlockArea.top; y <= rcBlockArea.bottom; y++ )
	{
		for( int x = rcBlockArea.left; x <= rcBlockArea.right; x++ )
		{
			ret.push_back( GetBlock( x, y ) );
		}
	}
	return ret.size();
};

//---------------------------------------------------//
// [9/3/2009 Albert]
// Description:	获取区域内的所有对象ID
//---------------------------------------------------//
int	CGameMap::GetTargets( const CBlockList& bl, CTargetSet& ret )
{
	struct _for_each
	{
		CTargetSet& _set;
		_for_each( CTargetSet& set ): _set( set ){}
		void operator()( _uint32 id ){ _set.insert( id ); }
	};

	CBlockList::const_iterator ci = bl.begin();
	while( ci != bl.end() )
	{
		CMapBlock *pBlock = *ci;

		if( pBlock )
		{
			pBlock->for_each_object( _for_each( ret ) );
		}
		++ci;
	}
	return ret.size();
}

//---------------------------------------------------//
// [9/3/2009 Albert]
// Description:	获取区域内的所有对象ID
//---------------------------------------------------//
int	CGameMap::GetBlockList( xgcPoint ptBegin, xgcPoint ptEnd, int nWidth, CBlockList& ret )const
{
	int x0, x = x0 = ptBegin.x;
	int y0, y = y0 = ptBegin.y;
	int x1 = ptEnd.x;
	int y1 = ptEnd.y;

	const int w = 3;
	if( x0 != x1 || y0 != y1 )
	{
		// 将起笔的地方放在方向所指的最前端.这样可以优化掉自身的碰撞检测.
		int sx = ((x1-x0)>0)?1:-1;
		int sy = ((y1-y0)>0)?1:-1;

		int dx = abs( x1 - x0 );
		int dy = abs( y1 - y0 );

		bool steep = dy > dx;
		if( steep )
		{
			//std::swap(w,h);
			std::swap(x,y);
			std::swap(dx,dy);
			std::swap(sx,sy);
		}

		int e = (dy << 1) - dx;

		for( int i = 0; i < dx; ++i )
		{
			int le = e;
			while( e >= 0 )
			{
				y += sy;
				e -= (dx << 1);
			}
			x += sx;
			e += (dy << 1);

			for( int i = 1, j = 1; i <= w; ++i, j*=-1 )
			{
				CMapBlock* pBlock = steep?GetBlock( y + (i >> 1)*j, x ):GetBlock( x, y + (i >> 1)*j );
				// CLogger::GetInstance( _T("Log") )->WriteLog( _T("直线检测[%d,%d]"), steep?(y + (i >> 1)*j):x, steep?x:(y + (i >> 1)*j) );
				if( pBlock && !pBlock->Empty() )
				{
					ret.push_back( pBlock );
				}
			}
		}
	}
	return ret.size();
}

// 检测碰撞
// return	:	是否有碰撞
// pBlock	:	和那个格子检测碰撞
// CollisionRect	:	那个物体检测碰撞
bool CGameMap::TestCollision( CMapBlock* pBlock, CGameObject *pObj )
{
	//if( !pBlock )	
	//	return false;

	//if( pBlock->m_bAllow == false ) 
	//	return true;

	//CXObjectPool& ObjList = CXObjectPool::GetInstance();
	//CDynamicObject* pDynamicObj = static_cast< CDynamicObject* >( ObjList.GetObj( nObjID, TypeDynamicObject ) );
	//ASSERT_MSG( pDynamicObj, _T("碰撞检测传入无效的对象ID！") );

	//if( pDynamicObj == NULL )
	//	return false;

	//CMapBlock::CObjList::iterator iter = pBlock->m_ObjList.begin();
	//while( iter != pBlock->m_ObjList.end() )
	//{
	//	int nID = *iter++;
	//	CGameObject* pObj = static_cast< CGameObject* >( ObjList.GetObj( nID, TypeGameObject ) );
	//	ASSERT_MSG( pObj, _T("无效的指针参数。") );
	//	if( pObj && nObjID != nID )
	//	{
	//		xgcRect& rc2 = xgcRect( pObj->GetCollision() );
	//		if( rc.IntersectRect( rc2, CollisionRect ) )
	//		{
	//			//TRACE( "检测到格子[%d,%d]的碰撞\n", pBlock->m_ptPos.x, pBlock->m_ptPos.y );
	//			if( pObj->Think( nObjID, CGameActorAI::event_beCollision, NULL, 0 ) )
	//			{
	//				return true;
	//			}

	//			if( pDynamicObj->Think( *iter, CGameActorAI::event_beCollision, NULL, 0 ) )
	//			{
	//				return true;
	//			}
	//		}
	//	}
	//}
	return false;
}

//	Obj		:	被移动的对象
//	enDir	:	移动方向
//	nStep	:	移动步长
//	return	:	是否移动
bool CGameMap::DynamicMoveTo( CDynamicObject* pObj, XVector3 &vPosition, bool bCollisionTest )
{
	if( pObj->GetLocalAttrib( DynamicObject_attrCanMove ) == 0 )
	{
		vPosition = pObj->GetPosition();
		return false;
	}

	//XVector3 vCurPosition = pObj->GetNextPosition();
	//XVector3 vDirection = vPosition - vCurPosition;
	//vDirection.Normalize();
	// 小于速度的极限值则直接返回
	float fPosX = pObj->GetPosX();
	float fPosY = pObj->GetPosZ();
	int x0, x = x0 = int(((int)fPosX - m_ptTransfrom.x)*m_InvBW);
	int y0, y = y0 = int(((int)fPosY - m_ptTransfrom.y)*m_InvBH);
	int x1 = int(((int)vPosition[0] - m_ptTransfrom.x)*m_InvBW);
	int y1 = int(((int)vPosition[2] - m_ptTransfrom.y)*m_InvBH);

	DEBUG_CODE( CMapBlock *pBlock = GetBlock(x,y); if( pBlock ) ASSERT_MSG( pBlock->CheckExist(pObj->GetObjID()), _T("发现飞机。") ); )
	bool ret = true;
	if( x0 != x1 || y0 != y1 )
	{
		if( bCollisionTest )
		{
			// 将起笔的地方放在方向所指的最前端.这样可以优化掉自身的碰撞检测.
			int sx = ((x1-x0)>0)?1:-1;
			int sy = ((y1-y0)>0)?1:-1;

			int dx = abs( x1 - x0 );
			int dy = abs( y1 - y0 );

			bool steep = dy > dx;
			if( steep )
			{
				//std::swap(w,h);
				std::swap(x,y);
				std::swap(dx,dy);
				std::swap(sx,sy);
			}

			int e = (dy << 1) - dx;

			for( int i = 0; i < dx; ++i )
			{
				int le = e;
				while( e >= 0 )
				{
					y += sy;
					e -= (dx << 1);
				}
				x += sx;
				e += (dy << 1);

				if( TestCollision( steep?GetBlock( y, x ):GetBlock( x, y ), pObj ) )
				{
					// 将x,y设为最后一个能走的Block的坐标
					x -= sx;
					while( le >= 0 )
					{
						y -= sy;
						le -= (dx << 1);
					}
					vPosition[0] = fPosX + ((steep?y:x) - x0)*m_siBlockSize.cx;
					vPosition[2] = fPosY + ((steep?x:y) - y0)*m_siBlockSize.cy;
					x1 = int(((int)vPosition[0] - m_ptTransfrom.x)*m_InvBW);
					y1 = int(((int)vPosition[2] - m_ptTransfrom.y)*m_InvBH);
					ret = false;
					break;
				}
			}
		}
		ExchangeBlock( pObj, &xgcPoint( x0, y0 ), &xgcPoint( x1, y1 ) );
		// CLogger::GetInstance( _T("Log") )->WriteLog( _T("对象%08x 移动到[%d,%d]"), pObj->GetObjID(), x1, y1 );
	}
	OnDynamicMove( pObj, vPosition );
	return ret;
}

void CGameMap::OnDynamicMove( CDynamicObject *pObj, XVector3 &vPosition )
{
	ASSERT_POINTER( pObj );
	if( pObj )
		pObj->SetPosition( vPosition );
}

void CGameMap::ExchangeBlock( CDynamicObject *pObj, const xgcPoint *pOldBlock, const xgcPoint *pNewBlock )
{
	// 变换格子引用
	// 取消旧的格子引用
	if( pOldBlock )
	{
		CMapBlock* pBlock = GetBlock( pOldBlock->x, pOldBlock->y );
		if( pBlock )
		{
			//CLogger::GetInstance( LOG )->WriteLog( _T("del object at [%d,%d] %08x"), x1, y1, pObj->GetObjID() );
			VERIFY( pBlock->DelObject( pObj->GetObjID() ) != 0 );
		}
	}

	if( pNewBlock )
	{
		CMapBlock* pBlock = GetBlock( pNewBlock->x, pNewBlock->y );
		if( pBlock )
		{
			//CLogger::GetInstance( LOG )->WriteLog( _T("add object at [%d,%d] %08x"), x1, y1, pObj->GetObjID() );
			pBlock->AddObject( pObj->GetObjID() );
			DEBUG_CODE( pObj->SetBlock( pBlock ); )
		}
	}
}

bool CGameMap::PreAddChild( CXObject* pObj )	// 将对象放入地图
{
	ASSERT( pObj );
	if( pObj == NULL )
		return false;

	if( pObj->IsTypeOf( TypeDynamicObject ) )
	{
		CDynamicObject* pDynamicObject = static_cast< CDynamicObject* >( pObj );
		xgcPoint pt( 
			int(((int)pDynamicObject->GetPosX() - m_ptTransfrom.x)*m_InvBW), 
			int(((int)pDynamicObject->GetPosZ() - m_ptTransfrom.y)*m_InvBH));
		ExchangeBlock( pDynamicObject, NULL, &pt );
	}
	else if( pObj->IsTypeOf( TypeGameObject ) )
	{
		CGameObject* pGameObject = static_cast< CGameObject* >( pObj );
		_uint32 nObjID = pGameObject->GetObjID();

		float cx = pGameObject->GetWidth();
		float cy = pGameObject->GetHeight();

		CBlockList	retList;
		xgcPoint pt( 
			int(((int)pGameObject->GetPosX() - m_ptTransfrom.x)*m_InvBW), 
			int(((int)pGameObject->GetPosZ() - m_ptTransfrom.y)*m_InvBH));
		xgcRect rcBlockArea( pt, pt );
		rcBlockArea.InflateRect( int(cx/2.0f), int(cy/2.0f) );
		GetBlockList( rcBlockArea, retList );

		if( retList.empty() )
		{
			ASSERT_MSG( false, _T("没有取到任何格子对象，给的碰撞盒子是正确的吗？") );
		}

		//char str[1024];
		//int nLen = _snprintf( str, sizeof( str ), "当前占有格子" );
		CBlockList::const_iterator citer = retList.begin();
		while( citer != retList.end() )
		{
			CMapBlock* pBlock = *citer;
			ASSERT_MSG( pBlock, _T("无效的指针参数。") );
			//nLen += _snprintf( str + nLen, sizeof( str ) - nLen, "[%d,%d]", pBlock->m_ptPos.x, pBlock->m_ptPos.y );
			if( pBlock )
			{
				pBlock->AddObject( nObjID );
			}
			++citer;
		}
	}
	return true;
}

bool CGameMap::PreRemoveChild( CXObject* pObj, bool bRelease )	// 将对象从地图移出
{
	ASSERT( pObj );
	if( pObj == NULL )
		return false;

	if( pObj->IsTypeOf( TypeDynamicObject ) )
	{
		CDynamicObject *pDynamicObject = static_cast< CDynamicObject* >( pObj );
		xgcPoint pt( 
			int(((int)pDynamicObject->GetPosX() - m_ptTransfrom.x)*m_InvBW), 
			int(((int)pDynamicObject->GetPosZ() - m_ptTransfrom.y)*m_InvBH));

		ExchangeBlock( pDynamicObject, &pt, NULL );
	}
	else if( pObj->IsTypeOf( TypeGameObject ) )
	{
		CGameObject *pGameObject = static_cast< CGameObject* >( pObj );

		CBlockList	retList;
		xgcPoint pt( 
			int(((int)pGameObject->GetPosX() - m_ptTransfrom.x)*m_InvBW), 
			int(((int)pGameObject->GetPosZ() - m_ptTransfrom.y)*m_InvBH));

		float cx = pGameObject->GetWidth();
		float cy = pGameObject->GetHeight();

		xgcRect rcBlockArea( pt, pt );
		rcBlockArea.InflateRect( int(cx/2.0f), int(cy/2.0f) );

		GetBlockList( rcBlockArea, retList );
		CBlockList::const_iterator citer = retList.begin();
		while( citer != retList.end() )
		{
			CMapBlock* pBlock = *citer;
			ASSERT_MSG( pBlock, _T("无效的指针参数。") );
			if( pBlock )
				pBlock->DelObject( pGameObject->GetObjID() );
			++citer;
		}
	}
	return true;
}