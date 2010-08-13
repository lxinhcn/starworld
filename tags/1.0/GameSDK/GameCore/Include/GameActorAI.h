#pragma once
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS

class CGameActor;
class CORE_API CGameActorAI
{
public:
	CGameActorAI( CGameActor *pActor );
	virtual ~CGameActorAI(void);

	enum EN_EVENT_CODE
	{
		event_collision,
		event_active_actor,	// NPC被玩家激活
		event_answer_actor,	// 回答npc的问题
		event_actor_born,			// 角色出生
		event_actor_enter_eyeshot,	// 角色进入视野
		event_actor_leave_eyeshot,	// 角色离开视野
		event_actor_hit,			// 角色受击事件
		event_actor_dead,			// 角色死亡
		event_actor_step,		// 角色移动
		event_actor_arrived = 64,	// 角色到达目标点
	};

	enum EN_ACTOR_HIT_SUBCODE
	{
		actor_hit_none = 0,
		actor_hit_normal = XGC_BIT(1),
		actor_hit_beatback = XGC_BIT(2),		// 角色击退事件
	};

	//////////////////////////////////////////////////////////////////////////
	//	思考
	//	pObj	:	思考的对象，可能是自己或空值
	//  nEvent	:	思考的事件
	//	lpBuf	:	根据事件编号传入的一些数据
	//	nSize	:	lpBuf的长度
	//////////////////////////////////////////////////////////////////////////
	virtual bool Think( _uint32 nObjID, int nEvent, long_ptr lParam ) = 0;

	//---------------------------------------------------//
	// [8/21/2009 Albert]
	// Description:	设置视野组ID
	//---------------------------------------------------//
	void SetEyeshotGroup( _uint32 nEyeshotGroup );

protected:
	CGameActor*	m_pMaster;
};
