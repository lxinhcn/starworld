#include "StdAfx.h"
#include ".\personai.h"
#include "Event.h"

CPersonAI::CPersonAI(void)
{
}

CPersonAI::~CPersonAI(void)
{
}

BOOL CPersonAI::Think( CGameObject* pObj, int nEvent, LPCSTR lpBuf, size_t nSize )
{
	BOOL bRet = FALSE;
	switch( nEvent )
	{
	case event_Talk:
		if( !lpBuf ) // First talk, say hello;
		{
			//TalkFirst( pObj );
		}
		else
		{
			// Talk( pObj, lpBuf, nSize );
		}
		break;
	default:
		break;
	}
	return bRet;
}
