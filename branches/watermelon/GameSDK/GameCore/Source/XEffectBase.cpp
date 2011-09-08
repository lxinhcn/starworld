#include "StdAfx.h"
#include ".\xeffectbase.h"

CXEffect::CXEffect( int nEffectID )
{
	m_nOwnerID		= INVALID_OBJID;		// 所有者ID
	m_nUserID		= INVALID_OBJID;		// 使用者ID
	m_nEffectID		= nEffectID;			// 效果ID
	m_nLv			= 0;
	ZeroMemory( m_fParam, sizeof( m_fParam ) );	// 效果参量

	m_bEffected		= false;	// 是否已经转移了作用
}

CXEffect::~CXEffect(void)
{
	//CNodeList::iterator iter = m_NodeList.begin();
	//while( iter !=m_NodeList.end() )
	//{
	//	delete (*iter);
	//	iter++;
	//}
	m_NodeList.clear();
}

const EFFECT_ENTRY CXEffect::_effectEntry[] =
{
	{ -1, -1, -1, 0, NULL, -1 }     // nothing here
};

const EFFECT_MAP CXEffect::_effectMAP = 
{
	NULL, 
	&_effectEntry[0],
};

const EFFECT_MAP*	CXEffect::getEffectMap() const
{
	return &CXEffect::_effectMAP;
}

const EFFECT_ENTRY*	CXEffect::FindEffectEntry( const EFFECT_ENTRY* lpEntry, int nEffectID )
{
	while( lpEntry->nID != INVALID_EFFECTID )
	{
		if( lpEntry->nID == nEffectID )	return lpEntry;
		lpEntry++;
	}
	return NULL;    // not found
}

int CXEffect::DoEffect( CXObject* pObj, bool bRemove )
{
	ASSERT_MSG( (m_bEffected && bRemove) || (!m_bEffected && !bRemove), _T("重复移除作用.") );
	// 重复进行作用,或者移除作用在没有作用前
	int nEffectID = m_nEffectID;
	// 当效果作用完成后，可能会引发另一个关联的作用效果。
	if( m_NodeList.empty() )
	{
		// 初始化作用链
		const EFFECT_MAP*	pMessageMap;
		const EFFECT_ENTRY*	lpEntry;
		while( nEffectID != INVALID_EFFECTID )
		{
			for( pMessageMap = getEffectMap(); pMessageMap != NULL; pMessageMap = pMessageMap->pBaseMap )
			{
				lpEntry = FindEffectEntry( pMessageMap->lpMapEntry, nEffectID );
				if( lpEntry && pObj->IsType( lpEntry->nType ) )
				{
					( this->*lpEntry->pDoEffect )( pObj, lpEntry->nAttribID, lpEntry->nType, lpEntry->bPersent, bRemove );
					const EFFECT_ENTRY* pEffectNode = lpEntry;//AllocEffectItem();
					//pEffectNode->nID		= lpEntry->nID;
					//pEffectNode->nAttribID	= lpEntry->nAttribID;
					//pEffectNode->nType		= lpEntry->nType;
					//pEffectNode->bPersent	= lpEntry->bPersent;
					//pEffectNode->pDoEffect	= lpEntry->pDoEffect;
					//pEffectNode->nNextEffID = lpEntry->nNextEffID;

					m_NodeList.push_back( pEffectNode );

					m_bEffected += (bRemove?-1:1);	// 标志自己的效果已经被转移了

					ASSERT_MSG( nEffectID != lpEntry->nNextEffID, _T("引起循环作用，程序可能因此死锁，至崩溃！") );
					nEffectID = lpEntry->nNextEffID;	// 下一个作用
					break;
				}
				else
				{
					nEffectID = INVALID_EFFECTID;
				}
			}
		}
		//ASSERT_MSG( !m_NodeList.empty(), "none effect be found" );
		return true;
	}

	// 已经将所有作用缓存在NodeList中，这样直接就可以执行了。不用再次去查表！
	CNodeList::const_iterator citer = m_NodeList.begin();
	while( citer !=m_NodeList.end() )
	{
		nEffectID = ( this->*( ( *citer )->pDoEffect ) )( 
			pObj,
			( *citer )->nAttribID,
			( *citer )->nType,
			( *citer )->bPersent,
			bRemove );

		m_bEffected += (bRemove?-1:1);	// 标志自己的效果已经被转移了
		citer++;
	}
	return true;
}

int	CXEffect::DoDefaultEffect( CXObject* pObj, int nAttribID, int nType, bool bPersent, bool bRemove )
{
	float fValue = bRemove?-m_fParam[0]:m_fParam[0];
	pObj->ChangeValue( nType, nAttribID, fValue, bPersent );

	return INVALID_EFFECTID;
}

//////////////////////////////////////////////////////////////////////////
// 设置参数
// nIdx		:	索引
// fValue	:	值
// return	:	是否设置成功
//////////////////////////////////////////////////////////////////////////
bool CXEffect::SetParam( int nIdx, float fValue )
{
	ASSERT( nIdx >= 0 && nIdx < EFFECT_PARAM_MAX );
	if( nIdx < 0 && nIdx >= EFFECT_PARAM_MAX )
	{
		return false;
	}

	m_fParam[nIdx] = fValue;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 设置参数
// fValue	:	值数组指针
// nFirstIdx:	起始索引
// nEndIdx	:	结束索引
// return	:	是否设置成功
// remark	:	nFirstIdx 可以大于 nEndIdx 但是fValue的值将被倒序输入。
//////////////////////////////////////////////////////////////////////////
bool CXEffect::SetParam( float* fValue, int nFirstIdx, int nEndIdx )
{
	ASSERT( nEndIdx >= 0 && nEndIdx < EFFECT_PARAM_MAX );
	ASSERT( nFirstIdx >= 0 && nFirstIdx < EFFECT_PARAM_MAX );
	if( nFirstIdx > nEndIdx ) std::swap( nFirstIdx, nEndIdx );
	if( nEndIdx < 0 || nEndIdx >= EFFECT_PARAM_MAX )
	{
		return false;
	}

	if( nFirstIdx < 0 || nFirstIdx >= EFFECT_PARAM_MAX )
	{
		return false;
	}

	for( int i = nFirstIdx; i != nEndIdx + 1; ++i )
	{
		m_fParam[i] = *fValue++;
	}
	return true;
}

CEffectFactory& CEffectFactory::GetInstance()
{
	static CEffectFactory _inst;
	return _inst;
}

//////////////////////////////////////////////////////////////////////////
//enum EFFECT_ITEM
//{
//	EFF_ITEM_HP	=	0,
//	EFF_ITEM_HPMAX,
//	EFF_ITEM_NONE,
//	EFF_ITEM_TEST1,
//	EFF_ITEM_TEST2,
//};
//
//BEGIN_EFFECT_TABLE( CXEffectItem, CXEffect )
//	EFFECT_DEFAULT( EFF_ITEM_HP,	0, TypeItem, EFF_ITEM_HPMAX )
//	EFFECT_DEFAULT( EFF_ITEM_HPMAX, 1, TypeItem, INVALID_EFFECTID )
//	EFFECT_DEFAULT_PERSENT( EFF_ITEM_TEST1, 2, TypeItem, INVALID_EFFECTID )
//	EFFECT_DEFAULT( EFF_ITEM_TEST2, 3, TypeItem, INVALID_EFFECTID )
//	EFFECT_DEFAULT( EFF_ITEM_NONE,	0, TypeMapobj, INVALID_EFFECTID )
//END_EFFECT_TABLE()

