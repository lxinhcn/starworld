#pragma once

#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS
#pragma warning( disable:4251 )

#include "XObject.h"
#include <list>
/*********************************************************************/
/*
    Create:     2006/01/041:2006   17:46
    FileName:   XEffectBase.h
    Author:     Albert xu
*/
/*********************************************************************/
/************************************************************************/
/* 实现了属性系统的作用系统，该类完成了对属性系统作用的底层代码。通过一组宏定义
/* 指定作用编号和属性编号的对应关系，以及作用的过程处理函数。底层提供了一个简单的
/* 过程处理函数，而其他过程处理函数可以由用户自由定义。同时，作用系统的影响链
/* 可以使两个作用具有相关性，即在某作用完成后触发另一作用继续对对象施加影响，例如
/* 增加HP的同时也增加HP最大值，即可以用作用链来触发。

/* 作用和属性的对应关系是多对一的，不同的作用效果可能都影响同一个属性值
/************************************************************************/
namespace XGC
{
	enum eStatus { status_append, status_active, status_remove, timing_count };

	// 定义指向函数的指针
	class CORE_API CXAction;
	typedef int (*ActionFunc)( xObject hSource, xObject hDest, const char* pBuf, unsigned int nSize );
	typedef int (*StatusFunc)( xObject hDest, int nFlag, long_ptr& pUserdata, const char* pBuf, unsigned int nSize );

	class CORE_API CXAction	:	public CXObject
	{
		DECLARE_DYNAMICTYPE( CXObject, TypeAction );
	public:
		CXAction( const ActionFunc Functor, cbuffer Params, float fDelay );
		~CXAction();

		//---------------------------------------------------//
		// [11/18/2010 Albert]
		// Description:	激活动作 
		//---------------------------------------------------//
		int ActiveAction( xObject hSource, xObject hTarget );

		//---------------------------------------------------//
		// [11/26/2010 Albert]
		// Description:	获取动作参数 
		//---------------------------------------------------//
		cbuffer GetParams()const{ return m_Params; }

		//---------------------------------------------------//
		// [11/26/2010 Albert]
		// Description:	获取延迟时间 
		//---------------------------------------------------//
		float GetDelay()const{ return m_fDelay; }

	protected:
		//---------------------------------------------------//
		// [11/18/2010 Albert]
		// Description:	执行动作 
		//---------------------------------------------------//
		int DoAction( xObject hSource, xObject hTarget );

	private:
		ActionFunc	m_Functor;
		cbuffer		m_Params;
		float		m_fDelay;
	};

	class CORE_API CXStatusInstance
	{
	public:
		CXStatusInstance();
		~CXStatusInstance();

		//---------------------------------------------------//
		// [12/14/2010 Albert]
		// Description:	添加作用 
		//---------------------------------------------------//
		void AppendEffect( const StatusFunc& Functor, cbuffer Params );

		//---------------------------------------------------//
		// [11/19/2010 Albert]
		// Description:	执行状态 
		//---------------------------------------------------//
		void DoStatus( xObject nIdentity, long_ptr& pUserdata, eStatus eFlag );
	private:
		struct effect
		{
			cbuffer		mParams;
			StatusFunc	mFunctor;
		};

		typedef xgc_vector< effect > EffectContainer;

		EffectContainer	m_EffectContainer;
	};

	class CORE_API CXStatus : public CXObject
	{
		DECLARE_DYNAMICTYPE( CXObject, TypeStatus );
	public:
		CXStatus( CXStatusInstance* pInstance )
			: m_pInstance( pInstance )
			, m_hUpdateTimerHandler( INVALID_TIMER_HANDLE )
		{
		}

		~CXStatus();

		//---------------------------------------------------//
		// [12/14/2010 Albert]
		// Description:	更新状态 
		//---------------------------------------------------//
		bool OnUpdate( timer_t handle );

		//---------------------------------------------------//
		// [12/15/2010 Albert]
		// Description:	状态结束 
		//---------------------------------------------------//
		bool OnStatusOver( timer_t handle );

		//---------------------------------------------------//
		// [9/13/2009 Albert]
		// Description:	安装定时器
		//---------------------------------------------------//
		void InstallTimer( float fLifetime, float fInterval, float fDelay = 0 );

		//---------------------------------------------------//
		// [11/19/2010 Albert]
		// Description:	执行状态 
		//---------------------------------------------------//
		__inline void DoStatus( eStatus eFlag )
		{
			m_pInstance->DoStatus( GetParent(), m_Userdata, eFlag );
		}

		virtual void OnDestroy()
		{
			m_pInstance->DoStatus( GetParent(), m_Userdata, status_remove );
		}
	private:
		CXStatusInstance* m_pInstance;
		timer_t m_hUpdateTimerHandler;
		timer_t m_hLifetimeHandler;

		union
		{
			unsigned int u;
			int n;
			float f;
			long_ptr m_Userdata;
		};
	};
}