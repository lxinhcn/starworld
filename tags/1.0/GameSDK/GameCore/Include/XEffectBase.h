#pragma once

#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS
#pragma warning( disable:4251 )

#include "stdafx.h"
#include "XObject.h"
#include <list>
/*********************************************************************/
/*
    Create:     2006/01/041:2006   17:46
    FileName:   e:\Projects\CaseProject\CaseProject\XEffectBase.h
    Author:     Albert xu

    Levle:      0
    Company:    赛泊洛斯
*/
/*********************************************************************/
/************************************************************************/
/* 实现了属性系统的作用系统，该类完成了对属性系统作用的底层代码。通过一组宏定义
/* 指定作用编号和属性编号的对应关系，以及作用的过程处理函数。底层提供了一个简单的
/* 过程处理函数，而其他过程处理函数可以由用户自由定义。同时，作用系统的影响链
/* 可以使两个作用具有相关性，即在某作用完成后触发另一作用继续对对象施加影响，例如
/* 增加HP的同时也增加HP最大值，即可以用作用链来触发。

/* 作用和属性的对应关系是多对一的，不同的作用效果可能都影响同一个属性值
/************************************************************************/
#define DECLARE_EFFECT_MAP()\
private:\
	static	const EFFECT_ENTRY	_effectEntry[];\
protected:\
	static	const EFFECT_MAP	_effectMAP;\
	virtual const EFFECT_MAP*	getEffectMap() const;\

#define BEGIN_EFFECT_TABLE( theClass, baseClass )\
	const EFFECT_MAP* theClass::getEffectMap() const\
	{ return &theClass::_effectMAP; };\
	__declspec( selectany ) const EFFECT_MAP theClass::_effectMAP = \
	{ &baseClass::_effectMAP, &theClass::_effectEntry[0] };\
	__declspec( selectany ) const EFFECT_ENTRY theClass::_effectEntry[] = \
	{\

#define EFFECT_DECLEAR( EffectID, AttribID, AttrType, IsPersent, pDoEffect, nNextEffectID )\
	{ EffectID, AttribID, AttrType, IsPersent,\
	( DoEffectFunc )( static_cast< bool ( CXEffect::* )( CXObject*, int, int, bool, bool ) >( &pDoEffect ) ), \
	nNextEffectID },\

#define EFFECT_DEFAULT( EffectID, AttribID, AttrType, nNextEffectID )\
	{ EffectID, AttribID, AttrType, false,\
	( DoEffectFunc )( static_cast< bool ( CXEffect::* )( CXObject*, int, int, bool, bool ) >( &DoDefaultEffect ) ), \
	nNextEffectID },\

#define EFFECT_DEFAULT_PERSENT( EffectID, AttribID, AttrType, nNextEffectID )\
	{ EffectID, AttribID, AttrType, true,\
	( DoEffectFunc )( static_cast< bool ( CXEffect::* )( CXObject*, int, int, bool, bool ) >( &DoDefaultEffect ) ), \
	nNextEffectID },\

#define END_EFFECT_TABLE()\
	{ -1, -1, -1, 0, NULL, -1 } };\

// 定义指向函数的指针
class CORE_API CXEffect;
typedef int ( CXEffect::* DoEffectFunc )( CXObject*, int, int, bool, bool );

struct EFFECT_ENTRY
{
	int				nID;		// effect id		效果编号
	int				nAttribID;	// effect attrib id	作用编号
	int				nType;		// effect type		属性类型
	bool			bPersent;	// effect persent	是否按百分比计算
	DoEffectFunc	pDoEffect;	// effect func		效果执行函数
	int				nNextEffID;	// 关联效果ID
};

// 消息映射表结构
struct EFFECT_MAP
{
	const EFFECT_MAP*	pBaseMap;
	const EFFECT_ENTRY*	lpMapEntry;
};

enum	EFFECT_OWNER_TYPE
{
	EN_OWNER_OBJECT,
	EN_OWNER_ITEM,
	EN_OWNER_STATE,
};

#define EFFECT_PARAM_MAX 6
#define INVALID_EFFECTID -1
class CORE_API CXEffect	:	public XObjTypeT< CXObject, TypeEffect >
{
DECLARE_EFFECT_MAP()
private:
	int		m_nOwnerID;		// 所有者ID
	int		m_nUserID;		// 使用者ID
	int		m_nEffectID;	// 效果ID
	float	m_fParam[EFFECT_PARAM_MAX];	// 效果参量

	BYTE	m_nLv;			// 效果等级
	BYTE	m_nOwnerType;	// 所有者类型 -对象， -状态，

protected:
	typedef std::list< const EFFECT_ENTRY* >	CNodeList;	// 作用节点列表

	CNodeList	m_NodeList;
	int			m_bEffected;	// 是否已经转移了作用

	const EFFECT_ENTRY*	FindEffectEntry( const EFFECT_ENTRY* lpEntry, int nEffectID );
	EFFECT_ENTRY* AllocEffectItem(){ return new EFFECT_ENTRY; }

public:
	explicit CXEffect( int nEffectID );
	~CXEffect(void);

	void	SetOwner( int nID ) { m_nOwnerID = nID; }
	int		GetOwner()const { return m_nOwnerID; }

	void	SetUser( int nID ) { m_nUserID = nID; }
	int		GetUser()const { return m_nUserID; }

	//////////////////////////////////////////////////////////////////////////
	// 设置参数
	// nIdx		:	索引
	// fValue	:	值
	// return	:	是否设置成功
	//////////////////////////////////////////////////////////////////////////
	bool	SetParam( int nIdx, float fValue );

	//////////////////////////////////////////////////////////////////////////
	// 设置参数
	// fValue	:	值数组指针
	// nFirstIdx:	起始索引
	// nEndIdx	:	结束索引
	// return	:	是否设置成功
	// remark	:	nFirstIdx 可以大于 nEndIdx 但是fValue的值将被倒序输入。
	//////////////////////////////////////////////////////////////////////////
	bool	SetParam( float* fValue, int nFirstIdx, int nEndIdx );

	// 执行作用
	int		DoEffect( CXObject* pObj, bool bRemove = false );

	// 默认的属性计算函数
	int		DoDefaultEffect( CXObject* pObj, int nAttribID, int nType, bool bPersent, bool bRemove );
};

#define REGIST_EFFECT( TYPE, LABLE )	CEffectRegister< TYPE > g_##__FILE__##LABLE( #LABLE )
#define CREATE_EFFECT( LABLE, EFFID )	CEffectFactory::GetInstance().Creator( (#LABLE), (EFFID) )
#define CREATE_EFFECT_PARAM( LABLE, EFFID, PARAM )	CEffectFactory::GetInstance().Creator( (#LABLE), (EFFID), (PARAM) )
#define CREATE_EFFECT_PARAMARRAY( LABLE, EFFID, PARAM, COUNT )	CEffectFactory::GetInstance().Creator( (#LABLE), (EFFID), (PARAM), (COUNT) )
class CORE_API CEffectFactory
{
protected:
	CEffectFactory(void){};
	~CEffectFactory(void){};

public:
	// 创建函数
	typedef CXEffect* (*pfnCreator)( int nEffectID, float* fParam, int nCount );

	// 单件模式
	static CEffectFactory& GetInstance();

	// 注册一个类型
	void Regist( LPCTSTR pszLable, pfnCreator pfunc )
	{
		CEffectFactoryMap::const_iterator citer = m_FactoryMap.find( pszLable );
		if( citer != m_FactoryMap.end() )	return;

		m_FactoryMap[pszLable] = pfunc;
	}

	CXEffect* Creator( LPCTSTR pszLable, int nEffectID, float* fParam = NULL, int nCount = 0 )
	{
		CEffectFactoryMap::const_iterator citer = m_FactoryMap.find( pszLable );
		if( citer != m_FactoryMap.end() )
		{
			return citer->second( nEffectID, fParam, nCount );
		}
		return NULL;
	}

	CXEffect* Creator( LPCTSTR pszLable, int nEffectID, float fParam )
	{
		CEffectFactoryMap::const_iterator citer = m_FactoryMap.find( pszLable );
		if( citer != m_FactoryMap.end() )
		{
			return citer->second( nEffectID, &fParam, 1 );
		}
		return NULL;
	}

private:
	typedef std::map< _string, pfnCreator >	CEffectFactoryMap;
	CEffectFactoryMap	m_FactoryMap;
};

template< typename TEffectType >
class CEffectRegister
{
public:
	explicit CEffectRegister( LPCTSTR pszLable )
	{
		CEffectFactory::GetInstance().Regist( pszLable, CreateEffect );
	}

	static CXEffect* CreateEffect( int nEffectID, float* fParam, int nCount )
	{
		TEffectType* pEffect = new TEffectType( nEffectID );
		ASSERT( !fParam || ( fParam && nCount > 0 && nCount <= EFFECT_PARAM_MAX ) );
		if( fParam && nCount > 0 && nCount <= EFFECT_PARAM_MAX )
		{
			pEffect->SetParam( fParam, 0, nCount - 1 );
		}
		return pEffect;
	}
};

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 一个作用系统的演示
/************************************************************************/
//class CXEffectItem	:	public CXEffect
//{
//DECLARE_EFFECT_MAP()
//public:
//	CXEffectItem( int nEffectID ):CXEffect( nEffectID ){};
//	~CXEffectItem(){};
//};

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* 状态系统，一个作用集合类的封装
/************************************************************************/
template< typename T >
class CXStatus
{
protected:
	int		m_nOwnerID;
	int		m_nUserID;
	int		m_nStatusID;
	int		m_nLv;

	typedef std::list< CXEffect* >	CNodeList;	// 作用节点列表

	CNodeList	m_NodeList;
	bool		m_bStated;	// 是否已经转移了作用

public:
	CXStatus( int nStatueID );
	virtual ~CXStatus();

	CXEffect*	AddEffect( int nEffectID );

	int		DoStatus( CXObject* pObj, bool bRemove = false );
	int		DoDefaultStatus( CXObject* pObj, bool bRemove );
};

//////////////////////////////////////////////////////////////////////////
// CXStatus
template< typename T>
CXStatus< typename T >::CXStatus( int nStatueID )
{
	//T* p;static_cast< CXEffect* >( p );
	m_nOwnerID	= INVALID_OBJID;
	m_nUserID	= INVALID_OBJID;
	m_nStatusID = nStatueID;
	m_nLv		= 0;
}

template< typename T>
CXStatus< typename T >::~CXStatus()
{
	//CNodeList::const_iterator citer = m_NodeList.begin();
	while( !m_NodeList.empty() )
	{
		CNodeList::iterator iter = m_NodeList.begin();
		delete (*iter);
		m_NodeList.pop_front();
	}
	m_NodeList.clear();
}

template< typename T>
CXEffect* CXStatus< typename T >::AddEffect( int nEffectID )
{
	CXEffect* pEffect = new T( nEffectID );
	if( !pEffect )	return NULL;

	pEffect->m_nOwnerID = m_nOwnerID;
	pEffect->m_nUserID	= m_nUserID;
	pEffect->m_nLv		= m_nLv;
	pEffect->m_nOwnerType = EN_OWNER_STATE;
	m_NodeList.push_back( pEffect );
	return pEffect;
}

template< typename T>
int CXStatus< typename T >::DoStatus( CXObject* pObj, bool bRemove /* = false  */ )
{
	CNodeList::const_iterator citer = m_NodeList.begin();
	while( citer != m_NodeList.end() )
	{
		(*citer)->DoEffect( pObj, bRemove );
		citer++;
	}
	return true;
}
