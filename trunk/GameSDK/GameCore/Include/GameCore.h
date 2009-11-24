#ifndef _SERVERCORE_H_
#define _SERVERCORE_H_

#include "GameTriggerMgr.h"
#include "timer_manager.h"
using namespace XGC;
struct core_thread
{
	CGameTriggerMgr	*trigger_mgr;
	CTimerManager	*timer;
	_uint16			error_t;
};

CORE_API extern _uint32 dwTlsIndex;

CORE_API CGameTriggerMgr*	ThisTriggerMgr();
CORE_API CTimerManager*		ThisTimer();

#endif //_SERVERCORE_H_