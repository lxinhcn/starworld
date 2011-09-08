// GameCore.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "GameCore.h"

CORE_API CGameTriggerMgr*	ThisTriggerMgr()
{
	core_thread* l = reinterpret_cast< core_thread* >( TlsGetValue( dwTlsIndex ) );
	ASSERT_POINTER( l );
	return l->_trigger_mgr;
}

CORE_API timer* ThisTimer()
{
	core_thread* l = reinterpret_cast< core_thread* >( TlsGetValue( dwTlsIndex ) );
	ASSERT_POINTER( l );
	return l->_timer;
}
