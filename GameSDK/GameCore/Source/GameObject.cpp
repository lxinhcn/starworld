#include "StdAfx.h"
#include "GameObject.h"
#include "GameMap.h"
BEGIN_ATTRIBUTE_TABLE(CDynamicObject, CGameObject)
	DEFINE_ATTRIBUTE( _T("MoveFlag"), DynamicObject_attrCanMove, 0 )
END_ATTRIBUTE_TABLE()

IMPLEMENT_ATTRIBUTE(CDynamicObject, CGameObject)

CGameObject::CGameObject()
{
}

CGameObject::~CGameObject()
{
}

CDynamicObject::CDynamicObject()
: m_hUpdateTimer( INVALID_TIMER_HANDLE )
{

}

CDynamicObject::~CDynamicObject(void)
{
	if( m_hUpdateTimer != INVALID_TIMER_HANDLE )
	{
		ThisTimer()->remove_event( m_hUpdateTimer );
		m_hUpdateTimer = INVALID_TIMER_HANDLE;
	}
}

//---------------------------------------------------//
// [8/4/2009 Albert]
// Description:	清除路径点
//---------------------------------------------------//
void CDynamicObject::BeginPathPoint()
{
	if( m_hUpdateTimer != INVALID_TIMER_HANDLE )
	{
		unsigned int release_time = ThisTimer()->remove_event( m_hUpdateTimer );
		m_hUpdateTimer = INVALID_TIMER_HANDLE;

		if( release_time > 0 )
		{
			// 计算向量
			XVector3 Vec = m_vNextPosition - GetPosition();
			Vec.NormalizeFast();
			// 剩余了多少距离
			Vec *= release_time/1000.0f;
			// 下此更新坐标和剩余距离的差值则为当前的位置
			m_vNextPosition -= Vec;

			CGameMap* pScene = static_cast< CGameMap* >( CXObjectPool::GetInstance().GetObj( GetParent(), TypeGameMap ) );
			if( pScene )
			{
				if( pScene->DynamicMoveTo( this, m_vNextPosition, false ) )
				{
					OnStep( m_vNextPosition );
				}
			}
		}
	}
	m_PointList.clear();
}

//---------------------------------------------------//
// [8/4/2009 Albert]
// Description:	添加路点
//---------------------------------------------------//
void CDynamicObject::AddPathPoint( XVector3 vPosition, float fSpeed )
{
	vPosition.y = 0.0f;
	PathPoint p = { vPosition, fSpeed };
	m_PointList.push_back( p );
}

//---------------------------------------------------//
// [8/4/2009 Albert]
// Description:	完成添加路点
//---------------------------------------------------//
void CDynamicObject::FinishPathPoint( float fDelayTime )
{
	// timer 参数为 -1 则表示立即被执行
	SetUpdate(
		_tfunction( bind( &CDynamicObject::UpdateTimer, this, _1, _2, _3 ) ),
		1, 
		TIMER_SECONDS( 1.0f ), 
		TIMER_SECONDS( fDelayTime ) );
}

//---------------------------------------------------//
// [9/9/2009 Albert]
// Description:	移动到指定位置
//---------------------------------------------------//
void CDynamicObject::MoveTo( XVector3 vPosition, float fSpeed, float fDelayTime )
{
	if( m_hUpdateTimer != INVALID_TIMER_HANDLE )
	{
		ThisTimer()->remove_event( m_hUpdateTimer );
		m_hUpdateTimer = INVALID_TIMER_HANDLE;
	}
	m_PointList.clear();

	PathPoint p = { vPosition, fSpeed };
	m_PointList.push_back( p );

	SetUpdate(
		_tfunction( bind( &CDynamicObject::UpdateTimer, this, _1, _2, _3 ) ),
		1, 
		TIMER_SECONDS( 1.0f ), 
		TIMER_SECONDS( fDelayTime ) );
}

//---------------------------------------------------//
// [8/5/2009 Albert]
// Description:	移动定时器
//---------------------------------------------------//
bool CDynamicObject::UpdateTimer( unsigned int handle, unsigned short& repeat, unsigned int& timer )
{
	repeat = 1;
	timer = TIMER_SECONDS(1.0f);

	// 先移动到当前所在的位置.
	CGameMap* pScene = static_cast< CGameMap* >( CXObjectPool::GetInstance().GetObj( GetParent(), TypeGameMap ) );
	if( pScene )
	{
		if( pScene->DynamicMoveTo( this, m_vNextPosition, false ) )
		{
			//CLogger::GetInstance(_T("Log"))->WriteLog( _T("角色[%08x]移动到路点[%f,%f,%f]")
			//	, GetObjID()
			//	, m_vNextPosition[0], m_vNextPosition[1], m_vNextPosition[2] );
			OnStep( m_vNextPosition );
		}
		else
		{
			OnArrived( m_vNextPosition );
			return true;
		}
	}
	else
	{
		m_hUpdateTimer = INVALID_TIMER_HANDLE;
		return true;
	}

	// 计算下一个点
	if( m_PointList.empty() ) 
	{
		m_hUpdateTimer = INVALID_TIMER_HANDLE;
		OnArrived( m_vNextPosition );
		return true;
	}

	const PathPoint& target = m_PointList.front();

	XVector3 Vec = target.vPosition - m_vNextPosition;

	float distance = Vec.SqurLength();
	float fSpeed = target.fSpeed;
	if( distance < fSpeed*fSpeed )
	{
		// 当前坐标到目标点的距离小于速度值
		m_vNextPosition = target.vPosition;

		float t = XMath::Sqrt( distance )/fSpeed;
		timer = TIMER_SECONDS(t);
		m_PointList.pop_front();
	}
	else
	{
		Vec.NormalizeFast();
		Vec *= fSpeed;
		m_vNextPosition += Vec;
	}

	return false;
}


//---------------------------------------------------//
// [9/16/2009 Albert]
// Description:	设置更新函数
//---------------------------------------------------//
bool CDynamicObject::SetUpdate( _tfunction &Fn, unsigned short repeat, unsigned int timer, unsigned int delay )
{
	StopUpdate();
	ResetPosition();
	// delay 参数为 -1 则表示立即被执行
	m_hUpdateTimer = ThisTimer()->insert_event( Fn, repeat, timer, delay );

	return m_hUpdateTimer != INVALID_TIMER_HANDLE;
}

//---------------------------------------------------//
// [9/16/2009 Albert]
// Description:	停止更新
//---------------------------------------------------//
void CDynamicObject::StopUpdate()
{
	if( m_hUpdateTimer != INVALID_TIMER_HANDLE )
	{
		ThisTimer()->remove_event( m_hUpdateTimer );
		m_hUpdateTimer = INVALID_TIMER_HANDLE;
	}
}