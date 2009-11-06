#include "StdAfx.h"
#include "GameSkill.h"
#include "GameActor.h"
//////////////////////////////////////////////////////////////////////////
CGameSkill::CGameSkill( _uint32 hOwner, _int16 nSerial )
: m_nChildrenSerial( 0 )
, m_nSerial( nSerial )
, m_hOwner( hOwner )
{
}

CGameSkill::~CGameSkill(void)
{
}

CGameActor* CGameSkill::GetOwner()const
{
	return static_cast< CGameActor* >( CXObjectList::GetInstance().GetObj( m_hOwner, TypeGameActor ) );
}