#include "StdAfx.h"
#include "animation.h"
#include "GameScene.h"

CAnimation::CAnimation(void)
{
	m_dwResID		= INVALID_RESID;
	m_nCurFrame		= 0;
	m_nCurState		= 0;
	m_nRef			= 0;

	m_dwOldTime		= 0;
	m_dwAniSpeed	= 100;
}

CAnimation::~CAnimation(void)
{
	CResourceMng& resmng = CResourceMng::GetInstance();
	resmng.ReleaseResource( m_dwResID );
}

BOOL CAnimation::CreateAni( LPCSTR lpszPathName )
{
	return TRUE;
}

BOOL CAnimation::Update( float fDeltaTime )
{
	return TRUE;
}