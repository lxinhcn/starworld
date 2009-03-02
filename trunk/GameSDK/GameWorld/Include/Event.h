#pragma once
#include "GameObject.h"
#include "XObjectAI.h"
//////////////////////////////////////////////////////////////////////////
// class CEvent
//////////////////////////////////////////////////////////////////////////
/*
// Event 对象专注各种事件的实现，比如地图中的传送门为CMapEvent处理的事件对象，
// 该事件将触发的源传送到一个指定地点。又比如Npc身上的对话事件为CObjEvent处理的
// 事件对象，该事件将对触发源产生一个Say:Action同时传回一个Replay:Event事件

// Event的职责是转交数据给相映的处理函数，其本身并不对数据进行操作。
*/
//////////////////////////////////////////////////////////////////////////
// 事件定义
enum EN_EVENT
{
	event_None		= 0,
	event_UserInput,
	event_MapEvent,		// 地图事件
	event_Talk,			// 对话事件
	event_beCollision,	// 被碰撞事件[]
	event_Collision,	// 碰撞事件[]
};

//enum EN_EVENT_TYPE
//{
//	type_event_map,
//};
//
//class CMap;
//class CGameEvent
//{
//public:
//	CEvent( int nType ): m_nType( nType ){}
//protected:
//	int m_nType;
//};
//
//class CEventPool
//{
//public:
//	CEventPool();
//	~CEventPool();
//
//	static BOOL			Initialize( int nTypeCount );
//	static CEventPool&	GetInstance( int nType );
//
//private:
//	typedef std::queue< CGameEvent* > CEventQueue;
//	CEventQueue	m_evnet_queue;
//};
//
//class CMapEvent	:	public	CGameEvent
//{
//public:
//	CMapEvnet(): CGameEvent( type_event_map ){}
//
//private:
//	virtual OnEvent();
//
//private:
//	CMap*	m_pMap;
//};