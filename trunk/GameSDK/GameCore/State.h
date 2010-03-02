#pragma once

template< class E >
class CState
{
public:
	CState(void);
	virtual ~CState(void);
	//--------------------------------------------------------//
	//	created:	21:1:2010   14:33
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	进入状态前调用
	//	pEntity:	进入状态的对象指针
	//	return:		TRUE 进入状态成功， false进入状态失败
	//--------------------------------------------------------//
	virtual bool OnEnter( E *pEntity ) = 0;

	//--------------------------------------------------------//
	//	created:	21:1:2010   14:34
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	更新状态
	//	pEntity:	进入状态的对象指针
	//--------------------------------------------------------//
	virtual void OnUpdate( E *pEntity ) = 0;

	//--------------------------------------------------------//
	//	created:	21:1:2010   17:02
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	中断
	//--------------------------------------------------------//
	virtual void OnInterrupt( E* pEntity ) = 0;

	//--------------------------------------------------------//
	//	created:	21:1:2010   17:02
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	中断恢复现场
	//--------------------------------------------------------//
	virtual void OnIret( E* pEntity ) = 0;

	//--------------------------------------------------------//
	//	created:	21:1:2010   14:35
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	离开状态
	//	pEntity:	进入状态的对象指针
	//--------------------------------------------------------//
	virtual void OnLeave( E *pEntity ) = 0;
};

template< class E, int C = 8 >
class CStateTemplate
{
public:
	typedef E _entity;

	typedef CState< _entity >				_state;	// 状态
	typedef CStateTemplate< _entity, C >	_this;

	typedef Loki::Function< _uint8 ( _entity* ) >	_guardfun;// 守护条件
	typedef std::list< _guardfun >	_guardlist; // 守护条件列表。

public:
	CStateTemplate( _entity *pOwner )
		: m_pOwner( pOwner )
		, m_pCurStage( NULL )
	{
		memset( m_StatePool, 0, sizeof(m_StatePool) );
	}

	~CStateTemplate()
	{

	}

	//--------------------------------------------------------//
	//	created:	22:1:2010   14:10
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	设置状态映射表
	//--------------------------------------------------------//
	bool MapState( _uint8 nIndex, _state *pState )
	{
		if( nIndex >= _countof(m_StatePool) ) 
			return false;

		m_StatePool[nIndex] = pState;
	}

	//--------------------------------------------------------//
	//	created:	22:1:2010   14:10
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	注册状态迁移路径的守护条件
	//--------------------------------------------------------//
	bool RegistStateGuard( _uint8 nIndex, _guardfun& Func )
	{
		if( nIndex >= _countof( m_StatePool ) )
			return false;

		m_Guards[nIndex].push_back( Func );
	}

	//--------------------------------------------------------//
	//	created:	22:1:2010   14:10
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	更新状态机
	//--------------------------------------------------------//
	void Update()
	{
		if( !m_pCurStage )
			return;

		_state* pState = m_StatePool[m_pCurStage->index];
		if( pState )
		{
			pState->OnUpdate( m_pOwner );
			_guardlist::iterator i = m_Guards[m_pCurStage->index].begin();
			while( i != m_Guards[m_pCurStage->index].end() )
			{
				_uint8 nNewState = (*i)( m_pOwner );
				if( nNewState != -1 )
				{
					ChangeTo( nNewState );
					break;
				}
				++i;
			}
		}
	}

	//--------------------------------------------------------//
	//	created:	22:1:2010   14:10
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	无条件迁移状态
	//--------------------------------------------------------//
	bool ChangeTo( _uint8 nIndex )
	{
		if( nIndex >= _countof(m_StatePool) )
			return false;

		if( m_pCurStage == NULL )
			m_pCurStage = new stage;
		_state *pOldState = m_StatePool[m_pCurStage->index];
		_state *pNewState = m_StatePool[nIndex];
		if( pNewState == NULL )
			return false;

		if( pNewState->OnEnter( m_pOwner ) == false )
			return false;

		if( pOldState )
			pOldState->OnLeave( m_pOwner );

		m_pCurStage->index = nIndex;
		m_pCurStage->interrupt = false;
		return true;
	}

	//--------------------------------------------------------//
	//	created:	22:1:2010   14:11
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	中断当前状态，被中断的状态会自动保存
	//				当前状态的上下文，等到中断退出的时候被恢复。
	//--------------------------------------------------------//
	bool Interrupt( _uint8 nIndex )
	{

		if( nIndex >= _countof(m_StatePool) )
			return false;

		_state *pOldState  = m_StatePool[m_pCurStage->index];
		_state *pNewState = m_StatePool[nIndex];

		if( pNewState == NULL )
			return false;

		if( pNewState->OnInterrupt( m_pOwner ) == false )
			return false;

		pstage s = new stage;
		s->index = m_pCurStage->index;
		s->interrupt = m_pCurStage->interrupt;
		s->next = m_CurStage->next;

		m_pCurStage
		return true;
	}

	//--------------------------------------------------------//
	//	created:	22:1:2010   14:14
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	中断返回，被中断的状态要调用中断返回函数切换到原先的状态。
	//--------------------------------------------------------//
	bool Iret()
	{
		if( m_ss.empty() )
			return false;

		_uint8 nIndex = m_ss.top();
		m_ss.pop();

		if( nIndex >= _countof(m_StatePool) )
			return false;

		_state *pOldState  = m_StatePool[nIndex];
		_state *pCurState  = m_StatePool[m_pCurStage->index];

		// 通知当前状态从中断过程中返回
		if( pCurState )
			pCurState->OnIret( m_pOwner );

		m_pCurStage->index = nIndex;

		return true;
	}

private:
	typedef struct _stage
	{
		_uint8	interrupt;
		_uint8	index;

		struct _stage *next;
	}stage, *pstage;

	_entity		*m_pOwner;
	_state		*m_StatePool[C];
	_guardlist	m_Guards[C];

	pstage		m_pCurStage;
};
