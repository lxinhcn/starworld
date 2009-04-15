#include "StdAfx.h"
#include "ObjAction.h"
#include "GameScene.h"

//CActionChangeDir::CActionChangeDir( float fDirection, int nHostID )
//:CAction( action_ChangeDir, nHostID, INVALID_OBJID )
//,m_fDir( fDirection )
//{
//
//}
//
//CActionChangeDir::~CActionChangeDir(void)
//{
//}
//
//// 执行动作
//BOOL CActionChangeDir::Execute( const CMap& Map )
//{
//	CXObject* pObj = CXObjectList::GetInstance().GetObj( m_nSrcID );
//	ASSERT_MSG( pObj->IsType( TypeDynamic ), "错误的对象类型。" );
//	if( !pObj->IsType( TypeDynamic ) )	return FALSE;
//	CDynamic* pDynObj = reinterpret_cast< CDynamic* >( pObj );
//	pDynObj->SetDirection( m_fDir );
//	SetFinish( TRUE );
//	return TRUE;
//}
//
////////////////////////////////////////////////////////////////////////////
//// CActionWaitUserInput
//int CActionWaitUserInput::m_nUserInput = FALSE;
//CActionWaitUserInput::CActionWaitUserInput()
//:CAction( action_WaitUserInput, INVALID_OBJID, INVALID_OBJID )
//{
//}
//// 在执行完后需要把m_nUserInput的值设置回来。
//BOOL CActionWaitUserInput::Execute( const CMap& Map )
//{
//	SetFinish( m_nUserInput );
//	return TRUE;
//}
//
////////////////////////////////////////////////////////////////////////////
//// CActionLookfor
//
//CActionLookfor::CActionLookfor( int nSrcObjID )
//:CAction( action_Lookfor, nSrcObjID, INVALID_OBJID )
//{
//}
//
//BOOL CActionLookfor::Execute( const CMap& Map )
//{
//	//CXObject* pObj = CXObjectList::GetInstance().GetObj( m_nSrcID );
//	//if( !pObj->IsType( TypeDynamic ) )	return FALSE;
//
//	//CDynamic* pDynObj = reinterpret_cast< CDynamic* >( pObj );
//	//x_rect rc = pDynObj->GetCollision();
//	//rc.InflateRect( Map.GetBlockWidth()/2, Map.GetBlockHeight()/2 );
//	//x_point ptCenter = rc.CenterPoint();
//	//int nObjInBlockX = ptCenter.x / Map.GetBlockWidth();
//	//int nObjInBlockY = ptCenter.y / Map.GetBlockHeight();
//
//	//CMap::CBlockList retList;
//	//Map.GetCollisionBlock( pDynObj->GetCollision(),retList );
//
//	//CDynamic::EN_DIRECTION enDir = pDynObj->GetOldDirection();
//	//switch( enDir )
//	//{
//	//case CDynamic::DirNone:
//	//	break;
//	//case CDynamic::DirUp:
//	//	nObjInBlockY--;
//	//	break;
//	//case CDynamic::DirLeft:
//	//	nObjInBlockX--;
//	//	break;
//	//case CDynamic::DirDown:
//	//	nObjInBlockY++;
//	//	break;
//	//case CDynamic::DirRight:
//	//	nObjInBlockX++;
//	//	break;
//	//default:
//	//	break;
//	//}
//
//	//if( (uint32)nObjInBlockX >= Map.GetMapWidth() )	nObjInBlockX = Map.GetMapWidth() - 1;
//	//if( (uint32)nObjInBlockX < 0 )	nObjInBlockX = 0;
//	//if( (uint32)nObjInBlockY >= Map.GetMapHeight() ) nObjInBlockY = Map.GetMapHeight() - 1;
//	//if( (uint32)nObjInBlockY < 0 )	nObjInBlockY = 0;
//
//	//CMap::CBlockList::const_iterator citer = retList.begin();
//	//while( citer != retList.end() )
//	//{
//	//	CMapBlock* pBlock = *citer;
//	//	if( pBlock )
//	//	{
//	//		int x, y;
//	//		pBlock->GetPos( x, y );
//	//		if( x == nObjInBlockX && y == nObjInBlockY )
//	//		{
//	//			CMapBlock::CObjList::const_iterator citerobj = pBlock->m_ObjList.begin();
//	//			while( citerobj != pBlock->m_ObjList.end() )
//	//			{
//	//				CXObject* pObj = CXObjectList::GetInstance().GetObj( *citerobj );
//	//				if( pObj->IsType( TypeMapobj ) )
//	//				{
//	//					CGameObject* pGameObj = reinterpret_cast< CGameObject* >( pObj );
//	//					pGameObj->Think( m_nSrcID, event_Talk, NULL, 0 );
//	//				}
//	//				++citerobj;
//	//			}
//	//		}
//	//	}
//	//	++citer;
//	//}
//	//SetFinish( TRUE );
//	return TRUE;
//}
