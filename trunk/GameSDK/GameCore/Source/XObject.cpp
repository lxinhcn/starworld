#include "stdafx.h"
#include "XObject.h"

CXObjectList::CXObjectList( int nLen )
{
	m_pObjList = new CXObject*[nLen];
	if( !m_pObjList )	throw( "cannot alloc enough memorys!" );

	ZeroMemory( m_pObjList, sizeof( CXObject* )*nLen );
	m_nLen		= nLen;
	m_nCount	= 0;
	m_nCurID	= 0;
}

CXObjectList::~CXObjectList()
{
	if( m_pObjList )	delete[] m_pObjList;
	m_pObjList = NULL;
}

CXObjectList& CXObjectList::GetInstance()
{
	static CXObjectList objlist( OBJ_MAX );
	return objlist;
}

int		CXObjectList::AddObj( CXObject* pObj )
{
	if( m_nCount >= m_nLen )	return INVALID_OBJID;
	while( m_pObjList[m_nCurID] != NULL )
	{
		m_nCurID++;
		if( m_nCurID >= m_nLen )	m_nCurID = 0;
	}

	m_pObjList[m_nCurID] = pObj;
	m_nCount++;
	return m_nCurID;
}

void	CXObjectList::DelObj( int nID )
{
	ASSERT_MSG( m_pObjList[nID]->GetObjID() == nID, "不等的ID，删除对象时出错。" );
	if( m_pObjList[nID]->GetObjID() != nID )	return;

	m_pObjList[nID] = NULL;
	m_nCount--;
}

//////////////////////////////////////////////////////////////////////////
// CXObject
CXObject::CXObject()
: m_nID( INVALID_OBJID )
, m_nParentID( INVALID_OBJID )
{
	// 自动列表
	m_nID = CXObjectList::GetInstance().AddObj( this );
}

CXObject::~CXObject()
{
	CXObjectList::GetInstance().DelObj( m_nID );
}

void	CXObject::AddChild( int nID )
{
	if( OnAddChild( nID ) )
	{
		CXObject* pObj = CXObjectList::GetInstance().GetObj( nID );
		if( pObj && std::find( m_ChildList.begin(), m_ChildList.end(), nID ) == m_ChildList.end() )
		{
			pObj->SetParent( m_nID );
			m_ChildList.push_back( nID );
		}
	}
}

// 删除子对象ID
void	CXObject::RemoveChild( int nID, bool bRelease )
{
	ASSERT( CXObjectList::GetInstance().GetObj( nID ) );
	if( OnRemoveChild( nID, bRelease ) )
	{
		CChildList::iterator iter = std::find( m_ChildList.begin(), m_ChildList.end(), nID );
		if( iter != m_ChildList.end() )
		{
			CXObject* pObj = CXObjectList::GetInstance().GetObj( nID );
			if( pObj ) pObj->SetParent( INVALID_OBJID );
			m_ChildList.erase( iter );
		}
	}
}

void	CXObject::EnumChild( EnumChildCallback CallbackFunc, LPVOID pParam )
{
	CChildList::iterator iter = m_ChildList.begin();
	CXObjectList& objlist = CXObjectList::GetInstance();
	while( iter != m_ChildList.end() )
	{
		int nID = *iter;
		++iter;
		if( CallbackFunc( nID, objlist.GetObj( nID ), pParam ) == FALSE ) break;
	}
}

bool	CXObject::Destroy()
{
	if( !OnDestroy() ) 
	{
		return false;
	}

	CXObject* pParent = CXObjectList::GetInstance().GetObj( GetParent() );
	if( pParent )
	{
		pParent->RemoveChild( GetObjID() );
	}
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