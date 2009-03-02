#pragma once
#ifdef GAMEWORLD_EXPORTS
#define GAMEWORLD_API __declspec(dllexport)
#else
#define GAMEWORLD_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#include "XObject.h"
class CGameObject;
//////////////////////////////////////////////////////////////////////////
/*
	对象的AI由CXObjectAI类派生，AI接口定义一个Think接口函数，该函数接受一个事
件代码以标示对象的逻辑分支，通过解析传入的数据缓冲，对不同的环境作出反应。AI包
含一个Action动作队列。该队列由实际的游戏对象解析。并提供相应的功能。动作包含执
行者和受影响的接受方两个对象，动作目标的引入仅仅为了动作可以根据目标的不同而使
用不同的逻辑分支。以及对目标进行状态限制。
	需要注意的是，Action仅仅影响源或目标的行为。如果需要同时改变其他对象的行为，
请使用Event对象。
*/
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 角色的动作接口
class CXObjectAI;
class CMap;
class GAMEWORLD_API CAction
{
protected:
friend class CXObjectAI;
	explicit CAction( int nSrcID, int nDescID )
		: m_nSrcID( nSrcID )
		, m_nDescID( nDescID )
	{}
	virtual ~CAction(){}

protected:
	int		m_nSrcID;		// 执行动作的对象ID
	int		m_nDescID;		// 针对的对象ID。可能为无效值

	BOOL	m_bIsFinish;

public:
	// 對Map的引用和不改變Map本身並不矛盾。Action對象的職責還是對源對象進行操作。
	// 並且Action對象不允許改變除原對象外的其他數據
	virtual BOOL Execute( CMap& Map )	= 0;		// 执行动作
};

//////////////////////////////////////////////////////////////////////////
// 物体的AI接口
class GAMEWORLD_API CXObjectAI
{
public:
	CXObjectAI( int nHostID );
	CXObjectAI();

	virtual ~CXObjectAI(void);

	//////////////////////////////////////////////////////////////////////////
	/*
	//	Think 函数内部为Action的产生地。除该处以外的任何地方都不可以有Action产生。
	*/
	//////////////////////////////////////////////////////////////////////////
	//	思考
	//	pObj	:	思考的对象，可能是自己或空值
	//  nEvent	:	思考的事件
	//	lpBuf	:	根据事件编号传入的一些数据
	//	nSize	:	lpBuf的长度
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL Think( CGameObject* pObj, int nEvent, LPCSTR lpBuf, size_t nSize ) = 0;

	// 更新
	virtual void Update( float fDeltaTime ) = 0;

	void AttachHost( int nHostID ){ m_nHostID = nHostID; }
protected:

	int		m_nHostID;
};
