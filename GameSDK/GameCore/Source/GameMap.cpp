#include "StdAfx.h"
#include <math.h>
#include "GameMap.h"
#include "GameActorAI.h"
//////////////////////////////////////////////////////////////////////////
// CGameMap
// 初始化地图
void CGameMap::InitializeMap( int nMapWidth, int nMapHeight, int nBlockWidth, int nBlockHeight, int nTransfromX, int nTransfromY )
{
	m_pBlockArray = new CMapBlock[nMapWidth*nMapHeight];

	m_nMapWidth		= nMapWidth;
	m_nMapHeight	= nMapHeight;
	m_nBlockSizeX	= nBlockWidth;
	m_nBlockSizeY	= nBlockHeight;
	m_nTransfromX	= nTransfromX;
	m_nTransfromY	= nTransfromY;

	m_InvBW = 1.0f/nBlockWidth;
	m_InvBH = 1.0f/nBlockHeight;
}

void CGameMap::DestroyMap()
{
	m_nMapWidth		= 0;
	m_nMapHeight	= 0;
	m_nBlockSizeX	= 0;
	m_nBlockSizeY	= 0;
	m_nTransfromX	= 0;
	m_nTransfromY	= 0;

	m_InvBW = 0.0f;
	m_InvBH = 0.0f;

	SAFE_DELETE_ARRAY( m_pBlockArray );
}

inline CMapBlock* CGameMap::GetBlock( int x, int y )const
{
	if( x < 0 || x >= m_nMapWidth || y < 0 || y >= m_nMapHeight )	return NULL;

	return m_pBlockArray?m_pBlockArray + (y*m_nMapWidth+x):NULL;
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

RECT CGameMap::GetBlockArea( RECT rcCollision )const
{
	RECT ret, rc = WorldToMap( rcCollision );

	ret.left	= int( ( rc.left + ( m_nBlockSizeX >> 1 ) )*m_InvBW );
	ret.top		= int( ( rc.top + ( m_nBlockSizeY >> 1) )*m_InvBH );

	ret.right	= int( (rc.right - rc.left)*m_InvBW + ret.left );
	ret.bottom	= int( (rc.bottom - rc.top)*m_InvBH + ret.top );

	return ret;
}

//---------------------------------------------------//
// [8/7/2009 Albert]
// Description:	获取对象占用的格子范围
//---------------------------------------------------//
RECT CGameMap::GetBlockArea( float fPosX, float fPosY, float fRadius )const
{
	RECT rc;
	// WorldToMap
	fPosX -= m_nTransfromX;
	fPosY -= m_nTransfromY;

	rc.left		= (long)((fPosX - fRadius)*m_InvBW);
	rc.top		= (long)((fPosY - fRadius)*m_InvBH);
	rc.right	= (long)((fPosX + fRadius)*m_InvBW);
	rc.bottom	= (long)((fPosY + fRadius)*m_InvBH);

	return rc;
}

int CGameMap::GetBlockList( int x0, int y0, int x1, int y1, CBlockList& ret )const
{
	for( int y = y0; y <= y1; y++ )
	{
		for( int x = x0; x <= x1; x++ )
		{
			ret.push_back( GetBlock( x, y ) );
		}
	}
	return ret.size();
}

int CGameMap::GetBlockList( POINT ptBegin, POINT ptEnd, CBlockList& ret )const
{
	return GetBlockList( ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y, ret );
};

int CGameMap::GetBlockList( RECT rcBlockArea, CBlockList& ret )const
{
	return GetBlockList( rcBlockArea.left, rcBlockArea.top, rcBlockArea.right, rcBlockArea.bottom, ret );
};

//---------------------------------------------------//
// [9/3/2009 Albert]
// Description:	获取区域内的所有对象ID
//---------------------------------------------------//
int	CGameMap::GetBlockList( POINT ptBegin, POINT ptEnd, int nWidth, CBlockList& ret )const
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
	int x0, x = x0 = int(((int)fPosX - m_nTransfromX)*m_InvBW);
	int y0, y = y0 = int(((int)fPosY - m_nTransfromY)*m_InvBH);
	int x1 = int(((int)vPosition[0] - m_nTransfromX)*m_InvBW);
	int y1 = int(((int)vPosition[2] - m_nTransfromY)*m_InvBH);

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
					vPosition[0] = fPosX + ((steep?y:x) - x0)*m_nBlockSizeX;
					vPosition[2] = fPosY + ((steep?x:y) - y0)*m_nBlockSizeY;
					x1 = int(((int)vPosition[0] - m_nTransfromX)*m_InvBW);
					y1 = int(((int)vPosition[2] - m_nTransfromY)*m_InvBH);
					ret = false;
					break;
				}
			}
		}
		ExchangeBlock( pObj, x0, y0, x1, y1 );
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

void CGameMap::ExchangeBlock( CDynamicObject *pObj, int x0, int y0, int x1, int y1 )
{
	// 变换格子引用
	// 取消旧的格子引用
	CMapBlock* pBlock = GetBlock( x0, y0 );
	if( pBlock )
	{
		//CLogger::GetInstance( LOG )->WriteLog( _T("del object at [%d,%d] %08x"), x1, y1, pObj->GetObjID() );
		VERIFY( pBlock->DelObject( pObj->GetObjID() ) != 0 );
	}

	pBlock = GetBlock( x1, y1 );
	if( pBlock )
	{
		//CLogger::GetInstance( LOG )->WriteLog( _T("add object at [%d,%d] %08x"), x1, y1, pObj->GetObjID() );
		pBlock->AddObject( pObj->GetObjID() );
		DEBUG_CODE( pObj->SetBlock( pBlock ); )
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
		ExchangeBlock( 
			pDynamicObject, 
			-1, -1, 
			int(((int)pDynamicObject->GetPosX() - m_nTransfromX)*m_InvBW),
			int(((int)pDynamicObject->GetPosZ() - m_nTransfromY)*m_InvBH) );
	}
	else if( pObj->IsTypeOf( TypeGameObject ) )
	{
		CGameObject* pGameObject = static_cast< CGameObject* >( pObj );
		_uint32 nObjID = pGameObject->GetObjID();

		float cx = pGameObject->GetWidth();
		float cy = pGameObject->GetHeight();

		CBlockList	retList;
		int x =	int(((int)pGameObject->GetPosX() - m_nTransfromX)*m_InvBW); 
		int y = int(((int)pGameObject->GetPosZ() - m_nTransfromY)*m_InvBH);

		// rcBlockArea.InflateRect( int(cx/2.0f), int(cy/2.0f) );
		GetBlockList( (int)(x - cx/2.0f), (int)(y - cx/2.0f), (int)(x + cy/2.0f), (int)(y + cy/2.0f), retList );

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
		ExchangeBlock( 
			pDynamicObject, 
			int(((int)pDynamicObject->GetPosX() - m_nTransfromX)*m_InvBW), 
			int(((int)pDynamicObject->GetPosZ() - m_nTransfromY)*m_InvBH),
			-1, -1 );
	}
	else if( pObj->IsTypeOf( TypeGameObject ) )
	{
		CGameObject *pGameObject = static_cast< CGameObject* >( pObj );

		CBlockList	retList;
		int x = int(((int)pGameObject->GetPosX() - m_nTransfromX)*m_InvBW); 
		int y = int(((int)pGameObject->GetPosZ() - m_nTransfromY)*m_InvBH);

		float cx = pGameObject->GetWidth();
		float cy = pGameObject->GetHeight();

		GetBlockList( (int)(x - cx/2.0f), (int)(y - cx/2.0f), (int)(x + cy/2.0f), (int)(y + cy/2.0f), retList );
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