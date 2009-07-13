#include "StdAfx.h"
#include "GameObject.h"
#include "XObjectAI.h"
#include "XEffectBase.h"
//////////////////////////////////////////////////////////////////////////
// BombObject
CGameObject::CGameObject()
: m_pAnimation( NULL )
, m_pAI( NULL )
{
	ZeroMemory( &m_siCollision, sizeof( m_siCollision ) );
}

CGameObject::~CGameObject()
{
	if( m_pAnimation )	m_pAnimation->Release();
	SAFE_DELETE( m_pAI );

	for( CEffectList::const_iterator citer = m_EffectList.begin(); citer != m_EffectList.end(); ++citer )
	{
		CXObject* pEffect = CXObjectList::GetInstance().GetObj( *citer );
		ASSERT( pEffect->IsKindOf( TypeEffect ) );
		pEffect->Destroy();
	}
	m_EffectList.clear();
}

void CGameObject::SetCollisionSize( int nWidth, int nHeight )
{
	m_siCollision.cx = nWidth;
	m_siCollision.cy = nHeight;
}

BOOL CGameObject::Think( int nObjID, int nEvent, _lpcstr lpBuf, size_t nSize )
{
	CGameObject* pObj = static_cast< CGameObject* >( CXObjectList::GetInstance().GetObj( nObjID ) );
	return m_pAI->Think( pObj, nEvent, lpBuf, nSize );
}

uint32 CGameObject::Update( const CMap& Map, float fDeltaTime )
{
	if( !m_pAI )	return FALSE;
	//m_pAI->DoAction( Map );
	m_pAI->Update( fDeltaTime );
	return TRUE;
}

BOOL CGameObject::DoAction( const CMap& Map )
{
	//while( !m_ActionQueue.empty() )
	//{
	//	CAction* pAction = m_ActionQueue.front();
	//	ASSERT_MSG( pAction, "错误的动作指针，添加动作时无效的指针引用。" );
	//	pAction->Execute( Map );
	//	if( !pAction->IsFinish() ) return FALSE;
	//	//delete pAction;
	//	m_ActionQueue.pop();
	//}
	return TRUE;
}

void CGameObject::RemoveEffect( int nObjID )
{
	CEffectList::iterator iter = std::find( m_EffectList.begin(), m_EffectList.end(), nObjID );
	if( iter != m_EffectList.end() )
	{
		m_EffectList.erase( iter );
	}
}

void CGameObject::AddEffect( int nObjID )
{
	CXObject* pObj = CXObjectList::GetInstance().GetObj( nObjID );
	if( pObj && pObj->IsType( TypeEffect ) )
	{
		AddEffect( static_cast< CXEffect* >( pObj ) );
	}
}

void CGameObject::AddEffect( CXEffect* pEffect )
{
	ASSERT_MSG( pEffect, _T("无效的作用指针。") );
	if( pEffect )
	{
		pEffect->SetOwner( GetObjID() );
		pEffect->SetUser( GetObjID() );
		pEffect->DoEffect( this );
		m_EffectList.push_back( pEffect->GetObjID() );
	}
}

int CGameObject::MoveEffectTo( CGameObject* pObj )
{
	if( pObj->IsKindOf( TypeMapobj ) )
	{
		for( CEffectList::const_iterator citer = m_EffectList.begin(); citer != m_EffectList.end(); ++citer )
		{
			pObj->AddEffect( *citer );
		}
		m_EffectList.clear();
		return 1;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
// BombMan

CDynamic::CDynamic()
{
	CGameObject::SetCollisionSize( 24, 16 );
}

CDynamic::~CDynamic(void)
{
}