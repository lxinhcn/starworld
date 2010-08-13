#include "stdafx.h"
#include "XObject.h"
#include "XObjectPool.h"

CORE_API _uint32 dwTlsIndex = TLS_OUT_OF_INDEXES;
namespace XGC
{
	//////////////////////////////////////////////////////////////////////////
	// CXObject
	CXObject::CXObject()
	: m_nID( INVALID_OBJID )
	, m_nParentID( INVALID_OBJID )
	, m_bIsParent( true )
	, m_bIsTypeList( true )
	, m_nStatus( OBJST_NORMAL )
	{
		// 自动列表
		m_nID = CXObjectPool::GetInstance().AddObj( this );
	}

	CXObject::CXObject( bool bIsParent, bool bIsTypeList )
	: m_nID( INVALID_OBJID )
	, m_nParentID( INVALID_OBJID )
	, m_bIsParent( bIsParent )
	, m_bIsTypeList( bIsTypeList )
	, m_nStatus( OBJST_NORMAL )
	{
		m_nID = CXObjectPool::GetInstance().AddObj( this );
	}

	CXObject::~CXObject()
	{
		CXObjectPool::GetInstance().DelObj( m_nID );
		ThisTriggerMgr()->TriggerEvent( m_nID, trigger_event_destroy, true );
	}

	bool CXObject::AddChild( identifier nID, bool bChangeParent /*= true*/ )
	{
		CXObject* pObj = CXObjectPool::GetInstance().GetObj( nID );
		return AddChild( pObj, bChangeParent );
	}

	bool CXObject::AddChild( CXObject* pObj, bool bChangeParent /*= true*/ )
	{
		ASSERT_POINTER( pObj );
		if( !pObj )
			return false;

		if( PreAddChild( pObj ) )
		{
			if( m_bIsParent && bChangeParent )
			{
				CXObject* pParent = CXObjectPool::GetInstance().GetObj( pObj->GetParent() );
				if( pParent )
				{
					pParent->RemoveChild( pObj );
				}
				pObj->SetParent( m_nID );
			}

			m_ChildList.insert( pObj->GetObjID() );

			// 插入到分类表中
			if( m_bIsTypeList )
			{
				CChildList& tl = m_TypeList[pObj->GetType()];
				tl.insert( pObj->GetObjID() );
			}

			OnAddChild( pObj );
			return true;
		}
		return false;
	}

	// 删除子对象ID
	void CXObject::RemoveChild( identifier nID, bool bRelease )
	{
		CXObject* pObj = CXObjectPool::GetInstance().GetObj( nID );
		return RemoveChild( pObj, bRelease );
	}

	void CXObject::RemoveChild( CXObject* pObj, bool bRelease /* = false */ )
	{
		ASSERT_POINTER( pObj );
		_uint32 nID = pObj->GetObjID();
		CChildList::iterator iter = m_ChildList.find( nID );
		if( iter != m_ChildList.end() && PreRemoveChild( pObj, bRelease ) )
		{
			OnRemoveChild( pObj, bRelease );

			m_ChildList.erase( iter );

			if( m_bIsTypeList )
			{
				// 分类表中删除
				CChildList& tl = m_TypeList[pObj->GetType()];
				tl.erase( nID );
			}

			// 根据指示销毁对象
			if( bRelease && m_bIsParent )
			{
				pObj->Destroy();
			}
		}
	}

	bool CXObject::Destroy()
	{
		CXObject* pParent = CXObjectPool::GetInstance().GetObj( GetParent() );
		m_nStatus |= OBJST_DESTROY;
		if( pParent )
		{
			pParent->RemoveChild( this );
		}
		OnDestroy();
		DestroyAllChild();
		delete this;
		return true;
	}

	void CXObject::DestroyAllChild()
	{
		CXObjectPool& objlist = CXObjectPool::GetInstance();
		while( !m_ChildList.empty() )
		{
			int nID = *m_ChildList.begin();

			CXObject* pObj = objlist.GetObj( nID );
			if( !pObj || !pObj->Destroy() )
			{
				return;
			}
		}
	}

	//---------------------------------------------------//
	// [9/21/2009 Albert]
	// Description:	销毁所有子对象
	//---------------------------------------------------//
	void CXObject::DestroyAllChild( int nType )
	{
		CXObjectPool& objlist = CXObjectPool::GetInstance();
		CTypeList::iterator iter = m_TypeList.find( nType );
		if( iter == m_TypeList.end() )
			return;

		CChildList &tl = iter->second;
		while( !tl.empty() )
		{
			int nID = *tl.begin();

			CXObject* pObj = objlist.GetObj( nID );
			if( !pObj || !pObj->Destroy() )
			{
				return;
			}
		}
	}
}