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
	typedef CState< E >			_ESTATE;	// 状态
	typedef CStateTemplate< E >	_THIS;

	typedef std::stack< _uint8 >			_STATESTACK; // 状态队列
	typedef Loki::Function< _uint8 ( E* ) >	_GUARDFUNC;// 守护条件
	typedef std::list< _GUARDFUNC >			_GUARDLIST; // 守护条件列表。

public:
	CStateTemplate( E *pOwner )
		: m_pOwner( pOwner )
		, m_nCurIndex( 0 )
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
	bool MapState( _uint8 nIndex, _ESTATE *pState )
	{
		if( nIndex >= _countof(m_StatePool) ) 
			return false;

		m_StatePool[nIndex].state = pState;
	}

	//--------------------------------------------------------//
	//	created:	22:1:2010   14:10
	//	filename: 	State
	//	author:		Albert.xu
	//
	//	purpose:	注册状态迁移路径的守护条件
	//--------------------------------------------------------//
	bool RegistStateGuard( _uint8 nIndex, _GUARDFUNC& Func )
	{
		if( nIndex >= _countof(m_StatePool) ) 
			return false;

		_STATE& State = m_StatePool[nIndex];
		if( !State.state )
			return false;

		State.guards.push_back( Func );
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
		_STATE& State = m_StatePool[m_nCurIndex];
		if( State.state )
		{
			State.state->OnUpdate( m_pOwner );
			_GUARDLIST::iterator i = State.guards.begin();
			while( i != State.guards.end() )
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

		_STATE& _OldState = m_StatePool[m_nCurIndex];
		_STATE& _NewState = m_StatePool[nIndex];
		if( _NewState.state == NULL )
			return false;

		if( _NewState.state->OnEnter( m_pOwner ) == false )
			return false;

		if( _OldState.state )
			_OldState.state->OnLeave( m_pOwner );

		m_nCurIndex = nIndex;
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
		m_ss.push( nIndex );

		if( nIndex >= _countof(m_StatePool) )
			return false;

		_STATE& _OldState = m_StatePool[m_nCurIndex];
		_STATE& _NewState = m_StatePool[nIndex];

		if( _NewState.state == NULL )
			return false;

		if( _NewState.state->OnEnter( m_pOwner ) == false )
			return false;

		if( _OldState.state )
			_OldState.state->OnInterrupt( m_pOwner );

		m_nCurIndex = nIndex;
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
		_uint8 nIndex = m_ss.top();
		m_ss.pop();

		if( nIndex >= _countof(m_StatePool) )
			return false;

		_STATE& _OldState = m_StatePool[m_nCurIndex];
		_STATE& _NewState = m_StatePool[nIndex];

		if( _NewState.state == NULL )
			return false;

		if( _NewState.state->OnIret( m_pOwner ) == false )
			return false;

		if( _OldState.state )
			_OldState.state->OnLeave( m_pOwner );

		m_nCurIndex = nIndex;

		return true;
	}

private:
	typedef struct _STATE_
	{
		_ESTATE*	state;
		_GUARDLIST	guards;
	}_STATE, *_PSTATE;

	E*		m_pOwner;
	_STATE	m_StatePool[C];
	_GUARDLIST	m_gl;
	_STATESTACK	m_ss;

	_uint8	m_nCurIndex;
};
