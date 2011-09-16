// GameCore.cpp : 定义 DLL 应用程序的入口点。
//

#include "stdafx.h"
#include "GameCore.h"

namespace XGC
{
	struct CCoreTimer	: public timer
	{
		friend struct CreateUsingNew< CCoreTimer >;
	};

	typedef SingletonHolder< CCoreTimer, CreateUsingNew, DeletableSingleton >	CoreTimer;
	bool InitGameCore()
	{
		return CoreTimer::Instance().initialize( 0xffff, 0xffff );
	}

	void FiniGameCore()
	{
		DeletableSingleton< CoreTimer >::GracefulDelete();
		DeletableSingleton< AttributeNameMap >::GracefulDelete();
	}

	timer& ThisTimer()
	{
		return static_cast< timer& >( CoreTimer::Instance() );
	}
}
