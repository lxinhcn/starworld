#include "stdafx.h"
#include "XObject.h"

CORE_API _uint32 dwTlsIndex = TLS_OUT_OF_INDEXES;
namespace XGC
{
	CXObjectList::CXObjectList( size_t nLen )
		: m_nCount( 0 )
		, m_nCurID( 0x81000000 )
		, m_nRound( 0x01 )
		, m_nLen( nLen )
	{
		m_pObjList = new CXObject*[nLen];
		if( !m_pObjList )	throw( "cannot alloc enough memorys!" );

		ZeroMemory( m_pObjList, sizeof( CXObject* )*nLen );
	}

	CXObjectList::~CXObjectList()
	{
		SAFE_DELETE_ARRAY( m_pObjList );
	}

	CXObjectList& CXObjectList::GetInstance()
	{
		static CXObjectList objlist( OBJ_MIN );
		return objlist;
	}

	bool CXObjectList::IsValidID( object_handle& h )
	{
		if( h.position < 0 || h.position >= m_nLen ) 
			return false;
		CXObject* pObj = m_pObjList[h.position];
		return ( pObj && ( *(size_t*)&h == pObj->GetObjID() ) );
	}

	CXObject* CXObjectList::GetObj( identifier nID, _uint32 type )
	{
		object_handle& h = (object_handle&)nID;
		if( IsValidID( h ) && m_pObjList[h.position]->IsTypeOf( type ) )
		{
			return m_pObjList[h.position];
		}

		return NULL;
	}

	identifier CXObjectList::AddObj( CXObject* pObj )
	{
		if( m_nCount >= m_nLen )	return INVALID_OBJID;
		object_handle& h = (object_handle&)m_nCurID;

		// 这里通过未引用队列可以进一步优化以降低查找时间
		#pragma message( "int CXObjectList::AddObj( CXObject* pObj ) 这里通过未引用队列可以进一步优化以降低查找时间" )
		while( m_pObjList[h.position] != NULL )
		{
			h.position++;
			if( h.position >= m_nLen )
			{
				h.position = 0;
				++h.round;
			}
		}

		m_pObjList[h.position] = pObj;
		m_nCount++;
		return m_nCurID;
	}

	void CXObjectList::DelObj( identifier nID )
	{
		object_handle& h = (object_handle&)nID;
		ASSERT_MSG( m_pObjList[h.position] && m_pObjList[h.position]->GetObjID() == nID, _T("不等的ID，删除对象时出错。") );
		if( m_pObjList[h.position]->GetObjID() != nID )	return;

		m_pObjList[h.position] = NULL;
		m_nCount--;
	}

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
		m_nID = CXObjectList::GetInstance().AddObj( this );
	}

	CXObject::CXObject( bool bIsParent, bool bIsTypeList )
	: m_nID( INVALID_OBJID )
	, m_nParentID( INVALID_OBJID )
	, m_bIsParent( bIsParent )
	, m_bIsTypeList( bIsTypeList )
	, m_nStatus( OBJST_NORMAL )
	{
		m_nID = CXObjectList::GetInstance().AddObj( this );
	}

	CXObject::~CXObject()
	{
		CXObjectList::GetInstance().DelObj( m_nID );
		ThisTriggerMgr()->TriggerEvent( m_nID, trigger_event_destroy, true );
	}

	bool CXObject::AddChild( identifier nID, bool bChangeParent /*= true*/ )
	{
		CXObject* pObj = CXObjectList::GetInstance().GetObj( nID );
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
				CXObject* pParent = CXObjectList::GetInstance().GetObj( pObj->GetParent() );
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
		CXObject* pObj = CXObjectList::GetInstance().GetObj( nID );
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
		CXObject* pParent = CXObjectList::GetInstance().GetObj( GetParent() );
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
		CXObjectList& objlist = CXObjectList::GetInstance();
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
		CXObjectList& objlist = CXObjectList::GetInstance();
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