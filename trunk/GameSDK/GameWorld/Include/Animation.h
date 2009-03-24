#pragma once
// 动画管理器通过一张资源图片描述一组动画。
// 帧对应行，并通过NextFrame函数进行切帧
// 状态对应列，通过设置状态参量切换不同的状态组
// 同时，由于动画元素有可能一次出现在多个不同对象上，所以在动画组件中设置了一个引用计数器
// 当且仅当计数器为0时释放动画对象。
class CSurface;
class CAnimation
{
protected:
	uint32	m_dwResID;			// 资源ID
	int		m_nCurFrame;		// 当前帧，	对应行
	int		m_nCurState;		// 当前状态， 对应列

	uint32	m_dwAniSpeed;		// 动画速度，1/1000秒为单位。
	uint32	m_dwOldTime;		// 上帧播放时的时间。
	int		m_nRef;

protected:
	friend class CMap;
	// 今后需要把这里改成私有，并且仅仅CMap可以创建渲染器，所有的渲染器也都通过Map进行管理。
	// 这样，每次翻帧就可以通过地图的逻辑一次进行，并且地图上的多个物体可以公用一个渲染器。
	CAnimation(void);
	~CAnimation(void);

public:
	virtual BOOL	CreateAni( LPCSTR lpszPathName );
	virtual void	SetState( int nState )			{ m_nCurState = nState;				}
	virtual void	SetCurFrame( int nFrame )		{ m_nCurFrame = nFrame;				} 

	//		如果动画速度为0则不进行换帧
	virtual void	SetAniSpeed( uint32 dwAniSpeed )	{ m_dwAniSpeed = dwAniSpeed;		}
	
	virtual int		GetCurFrame()				{ return m_nCurFrame; }
	virtual void	FrameReset()				{ m_nCurFrame = 0; }
	virtual BOOL	Update( float fDeltaTime );

	int		AddRef()	{ m_nRef++; return m_nRef; }
	int		Release()	{ m_nRef--; if( m_nRef <= 0 ){	delete this; return 0;	} else return m_nRef; }
};
