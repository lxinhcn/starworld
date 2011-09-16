#include "stdafx.h"
#include "XObject.h"
#include "XObjectPool.h"
#include "XStaticName.h"

namespace XGC
{
	CORE_API _uint32 dwTlsIndex = TLS_OUT_OF_INDEXES;
	bool RuntimeClass::IsDerivedFrom(const RuntimeClass* pBaseClass) const
	{
		// simple SI case
		const RuntimeClass* pClassThis = this;
		while (pClassThis != NULL)
		{
			if (pClassThis == pBaseClass)
				return true;

			pClassThis = pClassThis->m_pBaseClass;
		}
		return false;       // walked to the top, no match
	}

	//const struct RuntimeClass CXObject::classCXObject =
	//{ "CXObject", sizeof(CXObject), NULL, NULL };

	//////////////////////////////////////////////////////////////////////////
	// CXObject
	CXObject::CXObject( bool bUseChildList /*= true*/, bool bUseStrAttrib /*= true*/, bool bUseValAttrib /*= true*/ )
	: m_nID( INVALID_OBJECT_ID )
	, m_nParentID( INVALID_OBJECT_ID )
	, m_nStatus( OBJST_NORMAL )
	, m_ChildList( bUseChildList?new CChildList:xgc_nullptr )
	, m_strAttrib( bUseStrAttrib?new CStrAttribMap:xgc_nullptr )
	, m_valAttrib( bUseValAttrib?new CValAttribMap:xgc_nullptr )
	{
		// 自动列表
		m_nID = CXObjectPool::GetInstance().AddObject( this );
	}

	CXObject::CXObject( const CXObject& rsh )
	{
		// 自动列表
		m_nID = CXObjectPool::GetInstance().AddObject( this );
	}

	CXObject::~CXObject()
	{
		CXObjectPool::GetInstance().DelObject( m_nID );
		SAFE_DELETE( m_ChildList );
		SAFE_DELETE( m_strAttrib );
		SAFE_DELETE( m_valAttrib );
	}

	//RuntimeClass* CXObject::GetRuntimeClass() const
	//{
	//	return _RUNTIME_CLASS(CXObject);
	//}

	//bool CXObject::IsKindOf( const RuntimeClass* pRuntimeClass )const
	//{
	//	return GetRuntimeClass()->IsDerivedFrom( pRuntimeClass );
	//}

	void CXObject::Using( bool bUseChildList, bool bUseStrAttrib, bool bUseValAttrib )
	{
		if( bUseChildList && m_ChildList == xgc_nullptr )
			m_ChildList = new CChildList;
		else if( bUseChildList == false )
			SAFE_DELETE( m_ChildList );

		if( bUseStrAttrib && m_strAttrib == xgc_nullptr )
			m_strAttrib = new CStrAttribMap;
		else if( bUseStrAttrib == false )
			SAFE_DELETE( m_strAttrib );

		if( bUseValAttrib && m_valAttrib == xgc_nullptr )
			m_valAttrib = new CValAttribMap;
		else if( bUseValAttrib == false )
			SAFE_DELETE( m_valAttrib );
	}

	bool CXObject::AddChild( xObject nID, bool bChangeParent /*= true*/ )
	{
		CXObject* pObj = CXObjectPool::GetInstance().GetObject( nID );
		return AddChild( pObj, bChangeParent );
	}

	bool CXObject::AddChild( CXObject* pObj, bool bChangeParent /*= true*/ )
	{
		ASSERT_RETURN( pObj, false );
		ASSERT_RETURN( m_ChildList, false );

		if( PreAddChild( pObj ) )
		{
			if( bChangeParent )
			{
				CXObject* pParent = CXObjectPool::GetInstance().GetObject( pObj->GetParent() );
				if( pParent )
				{
					pParent->RemoveChild( pObj );
				}
				pObj->SetParent( m_nID );
			}

			m_ChildList->push_back( pObj->GetObjectID() );

			OnAddChild( pObj );
			return true;
		}
		return false;
	}

	// 删除子对象ID
	void CXObject::RemoveChild( xObject nID, bool bRelease )
	{
		CXObject* pObj = CXObjectPool::GetInstance().GetObject( nID );
		return RemoveChild( pObj, bRelease );
	}

	void CXObject::RemoveChild( CXObject* pObj, bool bRelease /* = false */ )
	{
		ASSERT_RETURN( pObj, void(0) );
		ASSERT_RETURN( m_ChildList, void(0) );

		_uint32 nID = pObj->GetObjectID();
		CChildList::iterator iter = std::find( m_ChildList->begin(), m_ChildList->end(), nID );
		if( iter != m_ChildList->end() && PreRemoveChild( pObj, bRelease ) )
		{

			m_ChildList->erase( iter );

			OnRemoveChild( pObj, bRelease );
			// 确认是自己的子物体，根据指示销毁对象
			if( bRelease && pObj->GetParent() == GetObjectID() )
			{
				pObj->SetParent( INVALID_OBJECT_ID );
				pObj->Destroy();
			}
		}
	}

	bool CXObject::QueryChild( xObject nID )
	{
		if( m_ChildList )
		{
			return std::find( m_ChildList->begin(), m_ChildList->end(), nID ) != m_ChildList->end();
		}

		return false;
	}

	bool CXObject::QueryChild( CXObject* pObj )
	{
		if( pObj )
			return QueryChild( pObj );

		return false;
	}

	void CXObject::Destroy()
	{
		if( m_nStatus&OBJST_DESTROY )
			return;

		m_nStatus |= OBJST_DESTROY;
		if( GetParent() != INVALID_OBJECT_ID )
		{
			CXObject* pParent = CXObjectPool::GetInstance().GetObject( GetParent() );
			if( pParent )
			{
				pParent->RemoveChild( this );
			}
		}
		OnDestroy();
		DestroyAllChild();
		delete this;
	}

	void CXObject::DestroyAllChild()
	{
		if( m_ChildList == xgc_nullptr )
			return;

		while( !m_ChildList->empty() )
		{
			int nID = *m_ChildList->begin();

			CXObject* pObj = ObjectPool.GetObject( nID );
			if( pObj )
			{
				pObj->Destroy();
			}
		}
	}
}