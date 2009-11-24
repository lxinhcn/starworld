#include "StdAfx.h"
#include "XObjectPool.h"

namespace XGC
{
	CXObjectPool::CXObjectPool( size_t nLen )
		: m_nCount( 0 )
		, m_nCurID( 0x81000000 )
		, m_nRound( 0x01 )
		, m_nLen( nLen )
	{
		m_pObjectPool = new CXObject*[nLen];
		if( !m_pObjectPool )	throw( "cannot alloc enough memorys!" );

		ZeroMemory( m_pObjectPool, sizeof( CXObject* )*nLen );
	}

	CXObjectPool::~CXObjectPool()
	{
		SAFE_DELETE_ARRAY( m_pObjectPool );
	}

	CXObjectPool& CXObjectPool::GetInstance()
	{
		static CXObjectPool objlist( OBJ_MIN );
		return objlist;
	}

	bool CXObjectPool::IsValidID( identifier id )
	{
		object_handle &h = (object_handle&)id;
		if( h.position < 0 || h.position >= m_nLen ) 
			return false;
		CXObject* pObj = m_pObjectPool[h.position];
		return ( pObj && ( *(size_t*)&h == pObj->GetObjID() ) );
	}

	CXObject* CXObjectPool::GetObj( identifier id, guid type )
	{
		object_handle &h = (object_handle&)id;
		if( IsValidID( id ) && m_pObjectPool[h.position]->IsTypeOf( type ) )
		{
			return m_pObjectPool[h.position];
		}

		return NULL;
	}

	identifier CXObjectPool::AddObj( CXObject* pObj )
	{
		if( m_nCount >= m_nLen )	return INVALID_OBJID;
		object_handle& h = (object_handle&)m_nCurID;

		// 这里通过未引用队列可以进一步优化以降低查找时间
		while( m_pObjectPool[h.position] != NULL )
		{
			h.position++;
			if( h.position >= m_nLen )
			{
				h.position = 0;
				++h.round;
			}
		}

		m_pObjectPool[h.position] = pObj;
		m_nCount++;
		return m_nCurID;
	}

	void CXObjectPool::DelObj( identifier id )
	{
		object_handle& h = (object_handle&)id;
		ASSERT_MSG( m_pObjectPool[h.position] && m_pObjectPool[h.position]->GetObjID() == id, _T("不等的ID，删除对象时出错。") );
		if( m_pObjectPool[h.position]->GetObjID() != id )	return;

		m_pObjectPool[h.position] = NULL;
		m_nCount--;
	}
}