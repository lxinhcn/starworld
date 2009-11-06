#pragma once

#include "GameObject.h"
class CGameActorAI;
class CORE_API CGameActor : public XObjOnlyTypeT< CGameActor, CDynamicObject, TypeGameActor >
{
friend class CGameActorAI;
public:
	CGameActor(void);
	~CGameActor(void);

	enum ActorStatus { status_live, status_dead, };

	bool Think( int nObjID, int nEvent, long_ptr lParam );

	//---------------------------------------------------//
	// [8/3/2009 Albert]
	// Description:	对象进入视野
	// pObject	:	进入视野的对象指针
	//---------------------------------------------------//
	virtual void EnterEyeshot( CDynamicObject* pObject );

	//---------------------------------------------------//
	// [8/3/2009 Albert]
	// Description:	对象离开视野
	// pObject	:	离开视野的对象指针
	//---------------------------------------------------//
	virtual void LeaveEyeshot( CDynamicObject* pObject );

	//---------------------------------------------------//
	// [9/9/2009 Albert]
	// Description:	到达目标时调用
	//---------------------------------------------------//
	virtual void OnArrived( const XVector3& vPosition );

	//---------------------------------------------------//
	// [9/10/2009 Albert]
	// Description:	每次移动后调用
	//---------------------------------------------------//
	virtual void OnStep( const XVector3& vPosition );

	//---------------------------------------------------//
	// [8/21/2009 Albert]
	// Description:	角色死亡
	//---------------------------------------------------//
	void Dead( _uint32 hAttacker );

	//---------------------------------------------------//
	// [8/21/2009 Albert]
	// Description:	获取角色的AI接口
	//---------------------------------------------------//
	CGameActorAI*	GetActorAI()const{ return m_pAI; }

	//---------------------------------------------------//
	// [8/24/2009 Albert]
	// Description:	获取角色状态
	//---------------------------------------------------//
	ActorStatus GetActorStatus()const{ return m_ActorStatus; }

	//---------------------------------------------------//
	// [8/24/2009 Albert]
	// Description:	设置角色状态
	//---------------------------------------------------//
	void SetActorStatus( ActorStatus status ){ m_ActorStatus = status; }

	//---------------------------------------------------//
	// [8/21/2009 Albert]
	// Description:	获取视野组
	//---------------------------------------------------//
	_uint32	GetEyeshotGroup()const{ return m_hEyeshotGroup; }

protected:
	virtual void OnDead( _uint32 hAttacker );
	//---------------------------------------------------//
	// [8/10/2009 Albert]
	// Description:	AI相关
	//---------------------------------------------------//
	void AttachAI( CGameActorAI* pAI ){ ASSERT( m_pAI == NULL ); m_pAI = pAI; }
	CGameActorAI*	DetachAI(){ CGameActorAI* pTmpAI = m_pAI; m_pAI = NULL; return pTmpAI; }

protected:
	_uint32		m_hEyeshotGroup;	// 视野组
	_uint32		m_hCurrentSkill;
	ActorStatus	m_ActorStatus;		// 角色状态
private:
	CGameActorAI*	m_pAI;			// 对象事件处理接口
};
