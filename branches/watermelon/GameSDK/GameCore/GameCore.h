#ifndef _SERVERCORE_H_
#define _SERVERCORE_H_

#include "delegate.hpp"
#include "AttribDef.h"
#include "DataHelper.h"
#include "XAttribute.h"
#include "XObject.h"
#include "XObjectPool.h"
#include "XAction.h"
#include "XMath.h"
#include "XVector2.h"
#include "XVector3.h"
#include "GameObject.h"
#include "XActorBase.h"
#include "GameMap.h"
#include "XSkillBase.h"

#if !defined( _USRDLL ) && !defined( _LIB )
	#pragma comment( lib, "ServerCore.lib" )
#endif

namespace XGC
{
	//---------------------------------------------------//
	// [11/16/2010 Albert]
	// Description:	初始化服务器内核 
	//---------------------------------------------------//
	CORE_API bool InitGameCore();

	//---------------------------------------------------//
	// [11/16/2010 Albert]
	// Description:	结束服务器内核 
	//---------------------------------------------------//
	CORE_API void FiniGameCore();

	CORE_API timer& ThisTimer();
}

#endif //_SERVERCORE_H_

