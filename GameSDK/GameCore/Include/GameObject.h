#pragma once
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#include <queue>
#include "Loki/Function.h"

class CMapBlock;
class CGameMap;
class CXObjectAI;
//////////////////////////////////////////////////////////////////////////
// 基础对象类， 所有屏幕上的对象都从此类派生
class CORE_API CGameObject	:	public XObjOnlyTypeT< CGameObject, CXObject, TypeGameObject >
{
friend class CGameMap;
public:
	CGameObject();
	~CGameObject();

	inline void SetPosition( float fPosX, float fPosY, float fPosZ )
	{
		m_Position.SetValue( fPosX, fPosY, fPosZ );
	}

	inline void SetPosition( const float fPos[3] )
	{
		m_Position[0] = fPos[0];
		m_Position[1] = fPos[1];
		m_Position[2] = fPos[2];
	}

	// 直接世界坐标
	inline void GetPosition( float fPos[3] )const
	{
		fPos[0] = m_Position[0];
		fPos[1] = m_Position[1];
		fPos[2] = m_Position[2];
	}

	// 直接世界坐标
	inline void GetPosition( XVector3& Position )const
	{
		Position = m_Position;
	}

	// 直接世界坐标
	inline const XVector3& GetPosition()const
	{
		return m_Position;
	}

	inline float GetPosX()const{ return m_Position.x; }
	inline float GetPosY()const{ return m_Position.y; }
	inline float GetPosZ()const{ return m_Position.z; }

	inline float GetWidth()const{ return 1.0f; }
	inline float GetHeight()const{ return 1.0f; }
	DEBUG_CODE
	(
	void SetBlock( void* pBlock ){ m_pBlock = pBlock; }
	void* GetBlock()const{ return m_pBlock; }
	)
private:
	XVector3	m_Position;
	DEBUG_CODE( void *m_pBlock; )
};

class CORE_API CDynamicObject :	public	XObjTypeT< CDynamicObject, CGameObject, TypeDynamicObject, _DynamicObject_attrCount >
{
public:
	CDynamicObject();
	virtual ~CDynamicObject(void);

	//---------------------------------------------------//
	// [8/3/2009 Albert]
	// Description:	对象进入视野
	// pObject	:	进入视野的对象指针
	//---------------------------------------------------//
	virtual void EnterEyeshot( CDynamicObject* pObject ){}

	//---------------------------------------------------//
	// [8/3/2009 Albert]
	// Description:	对象离开视野
	// pObject	:	离开视野的对象指针
	//---------------------------------------------------//
	virtual void LeaveEyeshot( CDynamicObject* pObject ){}

	//---------------------------------------------------//
	// [9/9/2009 Albert]
	// Description:	到达目标时调用
	//---------------------------------------------------//
	virtual void OnArrived( const XVector3& vPosition ){}

	//---------------------------------------------------//
	// [9/10/2009 Albert]
	// Description:	每次移动后调用
	//---------------------------------------------------//
	virtual void OnStep( const XVector3& vPosition ){}

	//---------------------------------------------------//
	// [8/4/2009 Albert]
	// Description:	清除路径点
	//---------------------------------------------------//
	__inline void BeginPathPoint();

	//---------------------------------------------------//
	// [8/4/2009 Albert]
	// Description:	添加路点
	//---------------------------------------------------//
	__inline void AddPathPoint( XVector3 vPosition, float fSpeed );

	//---------------------------------------------------//
	// [8/5/2009 Albert]
	// Description:	完成路点添加
	//---------------------------------------------------//
	__inline void FinishPathPoint( float fDelayTime = 0.0f );

	//---------------------------------------------------//
	// [9/9/2009 Albert]
	// Description:	移动到指定位置
	//---------------------------------------------------//
	void MoveTo( XVector3 vPosition, float fSpeed, float fDelayTime = 0.0f );

	//---------------------------------------------------//
	// [9/9/2009 Albert]
	// Description:	设置移动速度
	//---------------------------------------------------//
	__inline void SetSpeed( float fSpeed )	{ SetLocalAttrib( DynamicObject_attrSpeed, fSpeed ); }

	//---------------------------------------------------//
	// [9/9/2009 Albert]
	// Description:	获取移动速度
	//---------------------------------------------------//
	__inline float GetSpeed()const { return GetLocalAttrib( DynamicObject_attrSpeed ); }

	//---------------------------------------------------//
	// [9/10/2009 Albert]
	// Description:	是否在更新中
	//---------------------------------------------------//
	__inline bool IsUpdate()const{ return m_hUpdateTimer != INVALID_TIMER_HANDLE; }

	//---------------------------------------------------//
	// [9/16/2009 Albert]
	// Description:	设置更新函数
	//---------------------------------------------------//
	__inline bool SetUpdate( TimerFunction &Fn, unsigned short repeat = 1, unsigned int timer = TIMER_SECONDS(1), unsigned int delay = 0 );

	//---------------------------------------------------//
	// [9/16/2009 Albert]
	// Description:	停止更新
	//---------------------------------------------------//
	__inline void StopUpdate();

	//---------------------------------------------------//
	// [9/16/2009 Albert]
	// Description:	获取下一个移动点
	//---------------------------------------------------//
	__inline XVector3& GetNextPosition(){ return m_vNextPosition; }

	//---------------------------------------------------//
	// [9/18/2009 Albert]
	// Description:	重置当前坐标
	//---------------------------------------------------//
	__inline XVector3& ResetPosition(){ m_vNextPosition = GetPosition(); return m_vNextPosition; }
protected:
	//---------------------------------------------------//
	// [8/5/2009 Albert]
	// Description:	定时器调用该函数,用于调整坐标实现移动
	//---------------------------------------------------//
	bool UpdateTimer( unsigned int /*handle*/, unsigned short& /*repeat*/, unsigned int& /*timer*/ );

private:
	struct PathPoint
	{
		XVector3	vPosition;	// 路点位置
		float		fSpeed;		// 该点使用的速度
	};
	typedef std::list< PathPoint, ALLOCATOR< PathPoint > >	CPointList;

	CPointList	m_PointList;		// 路点列表
	XVector3	m_vNextPosition;	// 下一个目标点
	_uint32		m_hUpdateTimer;		// 位移定时器句柄
};