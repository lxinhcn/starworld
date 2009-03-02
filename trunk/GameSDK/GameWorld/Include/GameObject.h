#pragma once
#ifdef GAMEWORLD_EXPORTS
#define GAMEWORLD_API __declspec(dllexport)
#else
#define GAMEWORLD_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#include <queue>
#include "XObject.h"
#include "AttribDef.h"
#include "DataHelper.h"
#include "Animation.h"
using namespace XGC;

enum EN_MAPOBJ_ATTR
{
	Mapobj_attrVisible = 0,
	Mapobj_attrPosX,	// 地图坐标X
	Mapobj_attrPosY,	// 地图坐标Y
	Mapobj_attrPosZ,	// 地图坐标Z
	Mapobj_attrScene,	// 场景ID
	_Mapobj_attrCount,
};

class CMapBlock;
class CXEffect;
class CXObjectAI;
class CAction;
//////////////////////////////////////////////////////////////////////////
// 基础对象类， 所有屏幕上的对象都从此类派生
class GAMEWORLD_API CGameObject	:	public XObjTypeT< CXObject, TypeMapobj, _Mapobj_attrCount >
{
protected:
	SIZE	m_siCollision;		// 碰撞矩形

	CAnimation*	m_pAnimation;	// 动画管理器, 渲染器
	CXObjectAI*	m_pAI;			// 对象事件处理接口
	LPVOID		m_pCustomParam;	// 用户参数

	//friend class CGameObject;
	typedef std::queue< CAction* >	CActionQueue;
	CActionQueue	m_ActionQueue; // 动作队列，身体才可以执行动作的嘛！原来怎么会把它放在AI里！
	
	typedef std::list< int >		CEffectList;
	CEffectList		m_EffectList;	// 作用列表

private:
	CRect	m_rcOld;	// 临时变量，不允许访问
	// CXRender	m_pRender;	// 渲染器

public:
	CGameObject();
	~CGameObject();

	inline void SetPos( float fPosX, float fPosY, float fPosZ )
	{
		SetLocalAttrib( Mapobj_attrPosX, fPosX );
		SetLocalAttrib( Mapobj_attrPosY, fPosY );
		SetLocalAttrib( Mapobj_attrPosZ, fPosZ );
	}

	// 直接世界坐标
	inline void GetPos( float& fPosX, float& fPosY, float& fPosZ )const
	{
		_variant_t PosX, PosY, PosZ;

		GetLocalAttrib( Mapobj_attrPosX, PosX );
		GetLocalAttrib( Mapobj_attrPosY, PosY );
		GetLocalAttrib( Mapobj_attrPosZ, PosZ );
		fPosX = V_R4(&PosX);
		fPosY = V_R4(&PosY);
		fPosZ = V_R4(&PosZ);
	}

	inline float	GetPosX()const{	_variant_t PosX; GetLocalAttrib( Mapobj_attrPosX, PosX ); return V_R4(&PosX); }
	inline float	GetPosY()const{	_variant_t PosY; GetLocalAttrib( Mapobj_attrPosY, PosY ); return V_R4(&PosY); }
	inline float	GetPosZ()const{ _variant_t PosZ; GetLocalAttrib( Mapobj_attrPosZ, PosZ ); return V_R4(&PosZ); }

	inline void		SetPosX( float fPosX )	{ SetLocalAttrib( Mapobj_attrPosX, fPosX ); }
	inline void		SetPosY( float fPosY )	{ SetLocalAttrib( Mapobj_attrPosX, fPosY ); }
	inline void		SetPosZ( float fPosZ )	{ SetLocalAttrib( Mapobj_attrPosX, fPosZ ); }

	void SetCollisionSize( int nWidth, int nHeight );
	const RECT& GetCollision()
	{
		m_rcOld.SetRect( 
			(int)( GetPosX() - m_siCollision.cx/2 ),
			(int)( GetPosZ() - m_siCollision.cy/2 ),
			(int)( GetPosX() - m_siCollision.cx/2 + m_siCollision.cx - 1 ),
			(int)( GetPosZ() - m_siCollision.cy/2 + m_siCollision.cy - 1 ) );
		return m_rcOld;
	}


	void		AttachAI( CXObjectAI* pAI ){ m_pAI = pAI; }
	CXObjectAI*	DetachAI(){ CXObjectAI* pTmpAI = m_pAI; m_pAI = NULL; return pTmpAI; }
	CXObjectAI*	GetObjectAI(){ return m_pAI; }
	BOOL		SetMap( CMap* pMap );

	BOOL	Think( int nObjID, int nEvent, LPCSTR lpBuf, size_t nSize );
	//////////////////////////////////////////////////////////////////////////
	// return 是否完成所有动作	[TRUE - 全部完成] [FALSE - 仍有动作需要等待]
	virtual BOOL DoAction( const CMap& Map );

	CAnimation* GetAnimation()const	{ return m_pAnimation; }
	void		SetAnimation( CAnimation* pAni ) { m_pAnimation = pAni; m_pAnimation->AddRef(); }

	/************************************************************************/
	/* 用户参数设置                                                         */
	/************************************************************************/
	void		SetCustomParam( LPVOID pParam ) { m_pCustomParam = pParam; }
	LPVOID		GetCustomParam() const			{ return m_pCustomParam; }

	/************************************************************************/
	/* 作用列表操作
	/************************************************************************/
	void		RemoveEffect( int nObjID );
	void		AddEffect( int nObjID );
	void		AddEffect( CXEffect* pEffect );
	int			MoveEffectTo( CGameObject* pObj );

	// 更新逻辑
	// Map			:	地图数据对象的引用
	// fDeltaTime	:	时间差
	virtual	DWORD	Update( const CMap& Map, float fDeltaTime );
};

//////////////////////////////////////////////////////////////////////////
// BombMan 精灵
enum EN_DYNAMIC_ATTR
{
	Dynamic_attrSpeed = 0,
	Dyanmic_attrHP,
	Dyanmic_attrHPMAX,
	Dynamic_attrDirection,
	Dynamic_attrMoveState,		// 角色移动状态
	_Dynamic_attrCount,
};

#define DynamicStand	0
#define DynamicMove		1

class GAMEWORLD_API CDynamic :	public	XObjTypeT< CGameObject, TypeDynamic, _Dynamic_attrCount >
{
public:
	CDynamic();
	virtual ~CDynamic(void);

	void	Run( bool bRun )			{ SetLocalAttrib( Dynamic_attrMoveState, bRun ); }
	void	SetSpeed( float fSpeed )	{ SetLocalAttrib( Dynamic_attrSpeed, fSpeed ); }
	float	GetSpeed()					{ _variant_t Speed( 0.0f ); GetLocalAttrib( Dynamic_attrSpeed, Speed ); return V_R4(&Speed); }

	void	SetDirection( float fAngle ){ SetLocalAttrib( Dynamic_attrDirection, fAngle ); }
	float	GetDirection()				{ _variant_t Angle( 0.0f ); GetLocalAttrib( Dynamic_attrDirection, Angle ); return V_R4(&Angle); }
};