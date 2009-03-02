#include "stdafx.h"
#include "autolist.h"

CAutoList::CObjectProxy& CAutoList::CObjectProxy::operator=( CXObject* pObj )
{ 
	++order;
	if( NULL != pObj )
	{
		obj_point = pObj;
	}
	else
	{
		obj_point = NULL;
	}
	return *this;
}

CAutoList::CAutoList( int len )
{
	m_objlist = new CObjectProxy[len];
	if( !m_objlist )	throw( "cannot alloc enough memorys!" );

	ZeroMemory( m_objlist, sizeof( CXObject* )*len );
	m_len		= len;
	m_count		= 0;
	m_curindex	= 0;
}

CAutoList::~CAutoList()
{
	if( m_objlist )	delete[] m_objlist;
	m_objlist = NULL;
}


CAutoList& CAutoList::instance()
{
	static CAutoList objlist( OBJ_MAX );
	return objlist;
}

CAutoList::_Pos CAutoList::push( CXObject* pObj )
{
	if( m_count >= m_len )	return _Pos( INVALID_OBJID );
	CCriticalLock lock( m_lock );
	while( m_objlist[m_curindex].obj_point != NULL )
	{
		m_curindex++;
		if( m_curindex >= m_len )	m_curindex = 0;
	}

	m_objlist[m_curindex] = pObj;
	++m_count;
	_Pos pos( m_curindex, m_objlist[m_curindex].order );
	return pos;
}

void CAutoList::erase( const CAutoList::_Pos& Pos )
{
	CCriticalLock lock( m_lock );
	CAutoList::_Pos ThisPos = m_objlist[Pos.id]->GetObjID();
	ASSERT( ThisPos == Pos/*, "不等的ID，删除对象时出错。"*/ );
	if( ThisPos != Pos )	return;

	m_objlist[Pos.id] = NULL;
	--m_count;
}

//////////////////////////////////////////////////////////////////////////
// CXObject
CXObject::CXObject()
{
	// 自动列表
	m_self = CAutoList::instance().push( this );
}

CXObject::~CXObject()
{
	CAutoList::instance().erase( m_self );
}