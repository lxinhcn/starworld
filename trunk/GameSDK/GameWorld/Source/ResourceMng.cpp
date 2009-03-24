#include "StdAfx.h"
#include "..\Include\ResourceMng.h"

struct CResourceMng::Impl
{
	typedef std::vector< PRESOURCE >	CResourceVec;
	typedef std::queue< int >			CFreeQueue;		// 空闲索引
	CResourceVec	m_ResourceVec;	// 单元
	CFreeQueue		m_FreeQueue;	// 空闲单元索引队列
};

CResourceMng::CResourceMng()
: m_Impl( new Impl )
{
}

CResourceMng::~CResourceMng()
{
	delete m_Impl;
}

CResourceMng& CResourceMng::GetInstance()
{
	static CResourceMng resmng;
	return resmng;
}

BOOL	CResourceMng::IsResIDValid( uint32 dwResID )
{
	return dwResID < m_Impl->m_ResourceVec.size();
}

int		CResourceMng::AddResource( RES_TYPE enType, void* pRes )
{
	PRESOURCE pResource = NULL;
	if( m_Impl->m_FreeQueue.size() )
	{
		int nIndex = m_Impl->m_FreeQueue.front();
		pResource = m_Impl->m_ResourceVec[nIndex];
		m_Impl->m_FreeQueue.pop();
	}
	else
	{
		pResource = new RESOURCE;
	}
	if( pResource )
	{
		pResource->nID = ( int )m_Impl->m_ResourceVec.size();
		pResource->nType = enType;
		pResource->pResource = pRes;
		m_Impl->m_ResourceVec.push_back( pResource );
		return pResource->nID;
	}
	return INVALID_RESID;
}

BOOL	CResourceMng::DeleteResource( uint32 dwResID )
{
	if( !IsResIDValid( dwResID ) )	return FALSE;

	PRESOURCE pResource = m_Impl->m_ResourceVec[dwResID];
	if( pResource )
	{
		delete pResource;
		pResource = NULL;
		m_Impl->m_ResourceVec[dwResID] = NULL;
		m_Impl->m_FreeQueue.push( dwResID );
	}
	return TRUE;
}

void*	CResourceMng::GetResource( uint32 dwResID )
{
	PRESOURCE pResource = m_Impl->m_ResourceVec[dwResID];
	ASSERT_MSG( pResource, _T("无效的指针参数") );
	if( !pResource )	return NULL;

	pResource->nRef++;
	return pResource->pResource;
}

void	CResourceMng::ReleaseResource( uint32 dwResID )
{
	PRESOURCE pResource = m_Impl->m_ResourceVec[dwResID];
	ASSERT_MSG( pResource, _T("无效的指针参数") );
	if( !pResource )	return;
	pResource->nRef--;
	if( pResource->nRef <= 0 )
	{
		DeleteResource( dwResID );
	}
}