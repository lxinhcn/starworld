#include "StdAfx.h"
#include "GameScene.h"
#include "ResourceMng.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////
// CMap
// 初始化地图
VOID CMap::InitializeMap( uint32 dwMapWidth, uint32 dwMapHeight, int nRoleID )
{
	m_pBlockArray = new CMapBlock[dwMapWidth*dwMapHeight];

	m_dwMapWidth	= dwMapWidth;
	m_dwMapHeight	= dwMapHeight;

	if( CXObjectList::GetInstance().IsValidID( nRoleID ) )
	{
		m_nRoleID = nRoleID;
		PutObjToMap( nRoleID );
	}
}

inline CMapBlock* CMap::GetBlock( uint32 x, uint32 y )const
{
	if( x < 0 || x >= m_dwMapWidth || y < 0 || y >= m_dwMapHeight )	return NULL;

	return m_pBlockArray?m_pBlockArray + (y*m_dwMapWidth+x):NULL;
}

struct EnumChildParam
{
	CMap*	pMap;
	float	fDeltaTime;
};

uint32	CMap::Update( float fDeltaTime )
{
	// 逻辑控制，包括
	// 翻帧，真正的帧速率由Animation中的AniSpeed控制。
	// 地图对象的逻辑
	CAnimationList::const_iterator citer	= m_AnimationList.begin();
	CAnimationList::const_iterator citerend = m_AnimationList.end();
	while( citer != citerend )
	{
		(*citer)->Update( fDeltaTime);
		++citer;
	}

	// 每个加入地图的对象的更新逻辑
	CXObjectList& objlist = CXObjectList::GetInstance();
	EnumChildParam param = { this, fDeltaTime };
	EnumChild( OnEnumChild, (LPVOID)&param );
	return TRUE;
}

bool CMap::OnEnumChild( int nID, CXObject* pObj, LPVOID pParam )
{
	if( pObj && pObj->IsType( TypeMapobj ) )
	{
		CGameObject* pMapObj = static_cast< CGameObject* >( pObj );
		EnumChildParam* param = (EnumChildParam*)pParam;
		pMapObj->Update( *param->pMap, param->fDeltaTime );
		if( pMapObj->IsType( TypeDynamic ) )
		{
			CDynamic* pMan = static_cast< CDynamic* >( pMapObj );
			CXVariant IsRun;
			pMan->GetLocalAttrib( Dynamic_attrMoveState, IsRun );
			if( IsRun )
			{
				param->pMap->MoveObject( *pMan, pMan->GetDirection(), pMan->GetSpeed()*param->fDeltaTime );
			}
		}
	}
	return true;
}

bool CMap::OnAddChild(int nID )
{
	PutObjToMap( nID );
	return __super::OnAddChild( nID );
}

bool CMap::OnRemoveChild( int nID, bool bRelease )
{
	RemoveObjFromMap( nID );
	return __super::OnRemoveChild( nID, bRelease );
}

// 得到当前矩形区域内的所有格子
// rc		:	矩形区域
// retQueue	:	返回的格子队列

BOOL	CMap::GetCollisionBlock( const RECT& rc, CBlockList& retList )const
{
	int by = rc.top/GetBlockHeight();
	int bx = rc.left/GetBlockWidth();

	int ey = rc.bottom/GetBlockHeight();
	int ex = rc.right/GetBlockWidth();
	for( int y = by; y <= ey; y++ )
	{
		for( int x = bx; x <= ex; x++ )
		{
			retList.push_back( GetBlock( x, y ) );
		}
	}
	return TRUE;
}

// 检测碰撞
// return	:	是否有碰撞
// pBlock	:	和那个格子检测碰撞
// CollisionRect	:	那个物体检测碰撞
BOOL	CMap::TestCollision( CMapBlock* pBlock, const x_rect& CollisionRect, int nObjID )
{
	if( !pBlock )	return FALSE;
	x_rect rc;
	CMapBlock::CObjList::iterator iter = pBlock->m_ObjList.begin();
	CXObjectList& ObjList = CXObjectList::GetInstance();
	while( iter != pBlock->m_ObjList.end() )
	{
		int nID = *iter++;
		CGameObject* pObj = static_cast< CGameObject* >( ObjList.GetObj( nID ) );
		ASSERT_MSG( pObj, _T("无效的指针参数。") );
		if( pObj && nObjID != nID )
		{
			x_rect& rc2 = x_rect( pObj->GetCollision() );
			if( rc.IntersectRect( rc2, CollisionRect ) )
			{
				//TRACE( "检测到格子[%d,%d]的碰撞\n", pBlock->m_ptPos.x, pBlock->m_ptPos.y );
				if( pObj->Think( nObjID, event_beCollision, NULL, 0 ) )
				{
					return TRUE;
				}

				CGameObject* pDynamicObj = static_cast< CDynamic* >( ObjList.GetObj( nObjID ) );
				ASSERT_MSG( pDynamicObj->IsType( TypeDynamic ), _T("不正确的动态类型！") );
				if( pDynamicObj->Think( *iter, event_Collision, NULL, 0 ) )
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

//	Obj		:	被移动的对象
//	enDir	:	移动方向
//	nStep	:	移动步长
//	return	:	是否移动
BOOL	CMap::MoveObject( CDynamic& Obj, float fDirection, float fSpeed )
{
	if( fSpeed == 0.0f )	return FALSE;

	float fPosX, fPosY, fPosZ;
	Obj.GetPos( fPosX, fPosY, fPosZ );

	float fNewPosX = fPosX;
	float fNewPosY = fPosY;
	float fNewPosZ = fPosZ;

	int nMapRightLeg	= (m_dwMapWidth - 1)*m_siBlockSize.cy;
	int nMapDownLeg		= (m_dwMapHeight - 1)*m_siBlockSize.cx;
	BOOL bRet = TRUE;
	const x_rect& ObjRect = Obj.GetCollision();
	// legRect 能包围碰撞矩形的最小边界矩形。
	// 出于这样的想法，如果改变后的碰撞矩形仍包含在最小边界矩形中的话，
	// 则不再需要维护格子引用列表，否则将重新计算格子对将当先对象的引用。
	x_rect legRect(
	ObjRect.left/m_siBlockSize.cx*m_siBlockSize.cx,	// 整数规格化
	ObjRect.top/m_siBlockSize.cy*m_siBlockSize.cy,	// 整数规格化
	( ObjRect.right + m_siBlockSize.cx )/m_siBlockSize.cx*m_siBlockSize.cx,
	( ObjRect.bottom + m_siBlockSize.cy )/m_siBlockSize.cy*m_siBlockSize.cy );

	float fSpeedX = fSpeed*sin( fDirection );
	if( fSpeedX > (float)m_siBlockSize.cx )
	{
		fSpeedX = (float)m_siBlockSize.cx;
	}
	fNewPosX = fPosX + fSpeedX;

	float fSpeedZ = fSpeed*cos( fDirection );
	if( fSpeedZ > m_siBlockSize.cy )
	{
		fSpeedZ = (float)m_siBlockSize.cy;
	}
	fNewPosZ = fPosZ + fSpeedZ;

	// 将角色坐标设置为新坐标
	//Obj.SetPos( fNewPosX, fNewPosY, fNewPosZ );

	x_rect NewObjRect( ObjRect );
	NewObjRect.OffsetRect( (int)fNewPosX - (int)fPosX, (int)fNewPosZ - (int)fPosZ );

	//if( bRet )
	{
		// 得到新的边界矩形所包含的格子
		CBlockList	retList;
		GetCollisionBlock( NewObjRect, retList ); 
		CBlockList::const_iterator citerList = retList.begin();
		while( citerList != retList.end() )
		{
			// 和每个格子里的对象进行碰撞检测
			CMapBlock* pBlock = *citerList;
			//TRACE( "Block[%d,%d] was detected!\n", pBlock->m_ptPos.x, pBlock->m_ptPos.y );
			if( pBlock && TestCollision( pBlock, NewObjRect, Obj.GetObjID() ) )
			{
				return FALSE;
			}
			++citerList;
		}

		// 如果上层的移动判定失败则返回
		if( !OnDynamicMove( Obj, fNewPosX, fNewPosY, fNewPosZ ) )	return FALSE;

		Obj.SetPos( fNewPosX, fNewPosY, fNewPosZ );
		const x_rect& NewObjRect = Obj.GetCollision();
		x_rect NewLegRect(
			NewObjRect.left/m_siBlockSize.cx*m_siBlockSize.cx,
			NewObjRect.top/m_siBlockSize.cy*m_siBlockSize.cy,
			( NewObjRect.right + m_siBlockSize.cx )/m_siBlockSize.cx*m_siBlockSize.cx,
			( NewObjRect.bottom + m_siBlockSize.cy )/m_siBlockSize.cy*m_siBlockSize.cy );

		if( !legRect.PtInRect( NewObjRect.TopLeft() ) ||
			!legRect.PtInRect( NewObjRect.BottomRight() ) || 
			NewObjRect != legRect )
		{
			// 新的碰撞矩形不再边界矩形内的时候要重新调整格子引用
			Obj.SetPos( fPosX, fPosY, fPosZ );
			RemoveObjFromMap( Obj.GetObjID() );
			// RemoveChild( Obj.GetObjID() );
			Obj.SetPos( fNewPosX, fNewPosY, fNewPosZ );
			PutObjToMap( Obj.GetObjID() );
			// AddChild( Obj.GetObjID() );
		}
	}
	return bRet;
}

BOOL	CMap::PutObjToMap( int nObjID )			// 将对象放入地图
{
	CXObject* pObj = CXObjectList::GetInstance().GetObj( nObjID );
	if( !pObj )	return FALSE;
	if( !pObj->IsType( TypeMapobj ) )	return FALSE;
	CGameObject* pGameObj = static_cast< CGameObject* >( pObj );
	x_rect rc = pGameObj->GetCollision();
	CBlockList	retList;
	GetCollisionBlock( rc, retList );

	//char str[1024];
	//int nLen = _snprintf( str, sizeof( str ), "当前占有格子" );
	CBlockList::const_iterator citer = retList.begin();
	while( citer != retList.end() )
	{
		CMapBlock* pBlock = *citer;
		ASSERT_MSG( pBlock, _T("无效的指针参数。") );
		//nLen += _snprintf( str + nLen, sizeof( str ) - nLen, "[%d,%d]", pBlock->m_ptPos.x, pBlock->m_ptPos.y );
		pBlock->AddObject( nObjID );
		pGameObj->SetAttrib( TypeMapobj, Mapobj_attrScene, GetObjID() );
		++citer;
	}
	//_snprintf( str + nLen, sizeof( str ) - nLen, "\n" );
	//TRACE( str );
	// m_ObjMap[nObjID] = pGameObj;
	//pObj->SetParent( GetObjID() );
	//pObj->SetAttrib( TypeMapobj, Mapobj_attrScene, GetObjID() );
	//AddChild( nObjID );
	return TRUE;
}

BOOL	CMap::RemoveObjFromMap( int nObjID )	// 将对象从地图移出
{
	CGameObject* pObj = ( CGameObject* )CXObjectList::GetInstance().GetObj( nObjID );
	if( !pObj )	return FALSE;
	x_rect rc = pObj->GetCollision();
	CBlockList	retList;
	GetCollisionBlock( rc, retList );
	CBlockList::const_iterator citer = retList.begin();
	while( citer != retList.end() )
	{
		CMapBlock* pBlock = *citer;
		ASSERT_MSG( pBlock, _T("无效的指针参数。") );
		pBlock->DelObject( nObjID );
		pObj->SetAttrib( TypeMapobj, Mapobj_attrScene, -1 );
		++citer;
	}
	// m_ObjMap.erase( nObjID );
	//pObj->SetParent( INVALID_OBJID );
	//pObj->SetAttrib( TypeMapobj, Mapobj_attrScene, INVALID_OBJID );
	//RemoveChild( nObjID );
	return TRUE;
}