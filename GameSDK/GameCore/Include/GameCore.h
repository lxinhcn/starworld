#ifndef _SERVERCORE_H_
#define _SERVERCORE_H_

#include "GameTriggerMgr.h"
#include "timer_manager.h"
using namespace XGC;
using namespace XGC::common;
struct core_thread
{
	CGameTriggerMgr	*_trigger_mgr;
	timer			*_timer;
	_uint16			error_t;
};

CORE_API extern _uint32 dwTlsIndex;

CORE_API CGameTriggerMgr*	ThisTriggerMgr();
CORE_API timer*		ThisTimer();

#endif //_SERVERCORE_H_