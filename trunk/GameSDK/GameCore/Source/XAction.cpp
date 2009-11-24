#include "StdAfx.h"
#include "XAction.h"
namespace XGC
{
	const action_entry_ptr	FindEffectEntry( const action_map_ptr lpEntryMap, int nActionID )
	{
		size_t n = 0, count = lpEntryMap->lpMapEntryLast - lpEntryMap->lpMapEntry;

		const action_entry_ptr lpEntry = lpEntryMap->lpMapEntry;
		while( n < count )
		{
			if( lpEntry[n].nID == nActionID )
				return lpEntry + n;

			++n;
		}

		return NULL;
	}

	CXAction::CXAction()
		: m_entry_ptr( NULL )
		, m_nActionID( INVALID_ACTION_ID )	// 效果ID
		, m_bEffected( false )	// 是否已经转移了作用
	{
		memset( m_Param, 0, sizeof( m_Param ) );
	}

	CXAction::CXAction( int nActionID, const action_entry_ptr entry_ptr )
		: m_entry_ptr( entry_ptr )
		, m_nActionID( nActionID )			// 效果ID
		, m_bEffected( false )	// 是否已经转移了作用
	{
		ZeroMemory( m_Param, sizeof( m_Param ) );	// 效果参量
	}

	CXAction::CXAction( const CXAction& rsh )
		: m_entry_ptr( rsh.m_entry_ptr )
		, m_nActionID( rsh.m_nActionID )	// 效果ID
		, m_bEffected( false )	// 是否已经转移了作用
	{
		memcpy( m_Param, rsh.m_Param, sizeof( m_Param ) );
	}

	CXAction::~CXAction(void)
	{
	}

	const action_entry_ptr CXAction::getActionEntry()const
	{
		return m_entry_ptr;
	}

	int CXAction::DoAction( CXObject* pObj, bool bRemove )
	{
		// ASSERT_MSG( (m_bEffected && bRemove) || (!m_bEffected && !bRemove), _T("重复移除作用.") );
		if( (m_bEffected && !bRemove) || ( !m_bEffected && bRemove) )
			return false;

		// 在自己身上的作用才标记效果转移
		int flag = (GetParent() == pObj->GetObjID())?1:0;

		// 初始化作用链
		if( m_entry_ptr && pObj->IsTypeOf( m_entry_ptr->nDestType ) && m_entry_ptr->pDoAction )
		{
			//( this->*lpEntry->pDoEffect )( pObj, lpEntry->nAttribID, lpEntry->nType, lpEntry->bPersent, bRemove );
			int ret = (*m_entry_ptr->pDoAction)( this, pObj, m_entry_ptr->ConstParam, m_entry_ptr->nDestType, bRemove );
			//const action_entry_ptr pEffectNode = lpEntry;//AllocEffectItem();
			//m_NodeList.push_back( pEffectNode );
			m_bEffected += (bRemove?-flag:flag);	// 标志自己的效果已经被转移了
			return ret;
		}
		//ASSERT_MSG( !m_NodeList.empty(), "none effect be found" );
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置参数
	// nIdx		:	索引
	// fValue	:	值
	// return	:	是否设置成功
	//////////////////////////////////////////////////////////////////////////
	bool CXAction::SetParam( int nIdx, CXVariant fValue )
	{
		ASSERT( nIdx >= 0 && nIdx < ACTION_PARAM_MAX );
		if( nIdx < 0 || nIdx >= ACTION_PARAM_MAX )
		{
			return false;
		}

		m_Param[nIdx] = fValue;

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置参数
	// nIdx		:	索引
	// fValue	:	值
	// return	:	是否设置成功
	//////////////////////////////////////////////////////////////////////////
	bool CXAction::SetOwnerParam( int nIdx, CXVariant fValue )
	{
		ASSERT( nIdx >= 0 && nIdx < ACTION_PARAM_MAX );
		if( nIdx < 0 || nIdx >= ACTION_PARAM_MAX )
		{
			return false;
		}

		m_ParamEx[nIdx] = fValue;

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置参数
	// fValue	:	值数组指针
	// nFirstIdx:	起始索引
	// nEndIdx	:	结束索引
	// return	:	是否设置成功
	// remark	:	nFirstIdx 可以大于 nEndIdx 但是fValue的值将被倒序输入。
	//////////////////////////////////////////////////////////////////////////
	bool CXAction::SetParam( const CXVariant *fValue, int nFirstIdx, int nEndIdx )
	{
		if( nFirstIdx > nEndIdx ) std::swap( nFirstIdx, nEndIdx );
		ASSERT( nFirstIdx >= 0 && nEndIdx < ACTION_PARAM_MAX );
		if( nFirstIdx < 0 || nEndIdx >= ACTION_PARAM_MAX )
		{
			return false;
		}

		for( int i = nFirstIdx; i != nEndIdx + 1; ++i )
		{
			m_Param[i] = *fValue++;
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// 设置参数
	// fValue	:	值数组指针
	// nFirstIdx:	起始索引
	// nEndIdx	:	结束索引
	// return	:	是否设置成功
	// remark	:	nFirstIdx 可以大于 nEndIdx 但是fValue的值将被倒序输入。
	//////////////////////////////////////////////////////////////////////////
	bool CXAction::SetOwnerParam( const CXVariant *fValue, int nFirstIdx, int nEndIdx )
	{
		if( nFirstIdx > nEndIdx ) std::swap( nFirstIdx, nEndIdx );
		ASSERT( nFirstIdx >= 0 && nEndIdx < ACTION_PARAM_MAX );
		if( nFirstIdx < 0 || nEndIdx >= ACTION_PARAM_MAX )
		{
			return false;
		}

		for( int i = nFirstIdx; i != nEndIdx + 1; ++i )
		{
			m_ParamEx[i] = *fValue++;
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// Status class
	//////////////////////////////////////////////////////////////////////////
	CXStatus::CXStatus( int nActionID, const action_entry_ptr emap )
		: BaseClass( nActionID, emap )
		, m_hTimerHandler( INVALID_TIMER_HANDLE )
	{

	}

	CXStatus::~CXStatus()
	{
		if( m_hTimerHandler != INVALID_TIMER_HANDLE )
		{
			ThisTimer()->remove_event( m_hTimerHandler );
			m_hTimerHandler = INVALID_TIMER_HANDLE;
		}
	}

	//---------------------------------------------------//
	// [9/13/2009 Albert]
	// Description:	安装定时器
	//---------------------------------------------------//
	void CXStatus::InstallTimer( _uint32 nCount, float fInterval, float fDelay )
	{
		// 插入状态作用定时器
		m_hTimerHandler = ThisTimer()->insert_event( 
			TimerFunction( this, &CXStatus::OnTimer ), 
			nCount, 
			TIMER_SECONDS( fInterval ),
			TIMER_SECONDS( fDelay ) );
	}

	bool CXStatus::OnTimer( unsigned int handle, unsigned short &repeat, unsigned int &timer )
	{
		// CLogger::GetInstance(_T("Log"))->WriteLog( _T("角色[%08x]状态计数 Count = %d"), GetParent(), repeat );
		CXObject *pObj = CXObjectPool::GetInstance().GetObj( GetParent() );
		if( pObj )
		{
			m_bEffected = 0;
			DoAction( pObj, repeat == 0 );
		}

		if( repeat == 0 )
		{
			Destroy();
		}
		return false;
	}
}
