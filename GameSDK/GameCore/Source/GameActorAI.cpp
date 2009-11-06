#include "StdAfx.h"
#include "GameActorAI.h"
#include "GameActor.h"

CGameActorAI::CGameActorAI( CGameActor *pMaster )
: m_pMaster( pMaster )
{
	ASSERT_POINTER( m_pMaster );
	if( m_pMaster )
	{
		m_pMaster->AttachAI( this );
	}
}

CGameActorAI::~CGameActorAI(void)
{
	m_pMaster = NULL;
}

//---------------------------------------------------//
// [8/21/2009 Albert]
// Description:	ÉèÖÃÊÓÒ°×éID
//---------------------------------------------------//
void CGameActorAI::SetEyeshotGroup( _uint32 nEyeshotGroup )
{
	if( m_pMaster )
		m_pMaster->m_hEyeshotGroup = nEyeshotGroup;
}
