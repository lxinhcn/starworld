#pragma once
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS

namespace XGC
{
	class CGameObject;
	class CORE_API CObjectControlerBase
	{
	public:
		CObjectControlerBase();
		CObjectControlerBase( CGameObject *pObject );
		virtual ~CObjectControlerBase(void);

		//---------------------------------------------------//
		// [11/24/2010 Albert]
		// Description:	连接控制控制端 
		//---------------------------------------------------//
		void Attach( CGameObject* pObject );

		//---------------------------------------------------//
		// [11/24/2010 Albert]
		// Description:	断开控制端
		//---------------------------------------------------//
		CGameObject* Detach();

		//---------------------------------------------------//
		// [12/1/2010 Albert]
		// Description:	暂停更新逻辑 
		//---------------------------------------------------//
		void PauseUpdate();

		//---------------------------------------------------//
		// [12/1/2010 Albert]
		// Description:	恢复更新逻辑 
		//---------------------------------------------------//
		void ResumeUpdate();

		//---------------------------------------------------//
		// [11/24/2010 Albert]
		// Description:	设置更新 
		//---------------------------------------------------//
		void SetUpdate( const _tfunction &fn, int nRepeat, int nInterval, int nDelay );

		//---------------------------------------------------//
		// [9/10/2009 Albert]
		// Description:	是否在更新中
		//---------------------------------------------------//
		__inline bool IsUpdate()const{ return m_hUpdateTimer != INVALID_TIMER_HANDLE; }

		//---------------------------------------------------//
		// [9/16/2009 Albert]
		// Description:	停止更新
		//---------------------------------------------------//
		unsigned int StopUpdate();

	private:
		virtual void OnStopUpdate( unsigned int nReleaseTime ){};

	protected:
		CGameObject*	m_pHost;
		timer_t			m_hUpdateTimer;		// 定时器句柄
	};

	class CORE_API CObjectMoveControler	:	public CObjectControlerBase
	{
	public:
		CObjectMoveControler( CGameObject* pObject = xgc_nullptr, bool bCollision = false );
		~CObjectMoveControler();

		enum { MoveBegin, MoveArrived, MoveStop, MoveStep, };

		//---------------------------------------------------//
		// [12/16/2010 Albert]
		// Description:	移动到目标位置 
		//---------------------------------------------------//
		void MoveTo( XVector3 vPosition, float fSpeed, float fDelay = 0.0f );

		//---------------------------------------------------//
		// [8/4/2009 Albert]
		// Description:	清除路径点
		//---------------------------------------------------//
		virtual void OnStopUpdate( unsigned int nReleaseTime );

		//---------------------------------------------------//
		// [12/10/2010 Albert]
		// Description:	获取移动速度 
		//---------------------------------------------------//
		float GetSpeed()const{ return m_fSpeed; }

		//---------------------------------------------------//
		// [12/10/2010 Albert]
		// Description:	获取速度向量 
		//---------------------------------------------------//
		const XVector3& GetVectorSpeed()const{ return m_vSpeed; }

		//---------------------------------------------------//
		// [12/11/2010 Albert]
		// Description:	获取目标点 
		//---------------------------------------------------//
		void GetTargetPosition( float vTargetPosition[3] )const
		{ 
			vTargetPosition[0] = m_vTargetPosition[0]; 
			vTargetPosition[1] = m_vTargetPosition[1]; 
			vTargetPosition[2] = m_vTargetPosition[2]; 
		}

		//---------------------------------------------------//
		// [12/11/2010 Albert]
		// Description:	获取下一个移动点 
		//---------------------------------------------------//
		void GetNextPosition( float vNextPosition[3] )const
		{ 
			vNextPosition[0] = m_vNextPosition[0]; 
			vNextPosition[1] = m_vNextPosition[1]; 
			vNextPosition[2] = m_vNextPosition[2]; 
		}

	protected:
		//---------------------------------------------------//
		// [8/5/2009 Albert]
		// Description:	定时器调用该函数,用于调整坐标实现移动
		//---------------------------------------------------//
		bool UpdatePosition( unsigned int /*handle*/ );

		//---------------------------------------------------//
		// [11/22/2010 Albert]
		// Description:	计算下一次更新的坐标点 
		//---------------------------------------------------//
		float CalcNextPosition();
	private:
		XVector3	m_vNextPosition;	// 当前点
		XVector3	m_vTargetPosition;	// 目标点
		XVector3	m_vSpeed;
		float		m_fSpeed;
		bool		m_bCollision;
	};

	class CORE_API CObjectIdleControler	:	public CObjectControlerBase
	{
	public:
		CObjectIdleControler( CGameObject* pObject = xgc_nullptr );
		~CObjectIdleControler();

		enum { IdleTimeout, };
		void Idle( float fTime = 0.0f );

	protected:
		//---------------------------------------------------//
		// [8/5/2009 Albert]
		// Description:	定时器调用该函数,用于调整坐标实现移动
		//---------------------------------------------------//
		bool UpdateIdle( unsigned int handle );

	private:
		float		m_fIdleTime;
	};

}
