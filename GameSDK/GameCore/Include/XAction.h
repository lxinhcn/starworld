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
	// 定义指向函数的指针
	class CORE_API CXAction;
	typedef int (*DoActionFunc_c)( const CXAction*, CXObject*, CXVariant[3], int, bool );

	typedef struct ACTION_ENTRY
	{
		unsigned int	nID;			// id			效果编号
		unsigned int	nDestType;		// type			目标类型
		CXVariant		ConstParam[3];	// param		静态参数
		DoActionFunc_c	pDoAction;		// func			动作执行函数
	} action_entry, *action_entry_ptr;

	// 消息映射表结构
	typedef struct ACTION_MAP
	{
		const ACTION_MAP	*pBaseMap;
		action_entry_ptr	lpMapEntry;
		action_entry_ptr	lpMapEntryLast;
	} action_map, *action_map_ptr;

	CORE_API const action_entry_ptr	FindEffectEntry( const action_map_ptr lpEntryMap, int nActionID );

	#define ACTION_PARAM_MAX 3
	#define INVALID_ACTION_ID -1

	class CORE_API CXAction	:	public XObjOnlyTypeT< CXAction, CXObject, TypeGameAction >
	{
	//DECLARE_EFFECT_MAP()
	private:
		_uint32	m_nActionID;	// 效果ID

		//_byte	m_nLv;			// 效果等级
		//_byte	m_nOwnerType;	// 所有者类型 -对象， -状态，

		const action_entry_ptr m_entry_ptr;	// 模板入口
	protected:
		int			m_bEffected;					// 是否已经转移了作用
		CXVariant	m_Param[ACTION_PARAM_MAX];		// 效果参量
		CXVariant	m_ParamEx[ACTION_PARAM_MAX];	// 效果参量

		CXAction();
	public:
		CXAction( int nActionID, const action_entry_ptr emap );
		CXAction( const CXAction& rsh );
		~CXAction(void);

		//////////////////////////////////////////////////////////////////////////
		// 设置参数
		// nIdx		:	索引
		// fValue	:	值
		// return	:	是否设置成功
		//////////////////////////////////////////////////////////////////////////
		bool	SetParam( int nIdx, CXVariant fValue );

		//////////////////////////////////////////////////////////////////////////
		// 设置参数
		// nIdx		:	索引
		// fValue	:	值
		// return	:	是否设置成功
		//////////////////////////////////////////////////////////////////////////
		bool	SetOwnerParam( int nIdx, CXVariant fValue );

		//////////////////////////////////////////////////////////////////////////
		// 获取参数
		// nIdx		:	索引
		// fValue	:	值
		// return	:	是否设置成功
		//////////////////////////////////////////////////////////////////////////
		CXVariant	GetParam( int nIdx )const{ ASSERT( nIdx >=0 && nIdx < ACTION_PARAM_MAX ); return m_Param[nIdx]; }

		//////////////////////////////////////////////////////////////////////////
		// 获取参数
		// nIdx		:	索引
		// fValue	:	值
		// return	:	是否设置成功
		//////////////////////////////////////////////////////////////////////////
		CXVariant	GetOwnerParam( int nIdx )const{ ASSERT( nIdx >=0 && nIdx < ACTION_PARAM_MAX ); return m_ParamEx[nIdx]; }

		//////////////////////////////////////////////////////////////////////////
		// 设置参数
		// fValue	:	值数组指针
		// nFirstIdx:	起始索引
		// nEndIdx	:	结束索引
		// return	:	是否设置成功
		// remark	:	nFirstIdx 可以大于 nEndIdx 但是fValue的值将被倒序输入。
		//////////////////////////////////////////////////////////////////////////
		bool	SetParam( const CXVariant *fValue, int nFirstIdx, int nEndIdx );

		//////////////////////////////////////////////////////////////////////////
		// 设置参数
		// fValue	:	值数组指针
		// nFirstIdx:	起始索引
		// nEndIdx	:	结束索引
		// return	:	是否设置成功
		// remark	:	nFirstIdx 可以大于 nEndIdx 但是fValue的值将被倒序输入。
		//////////////////////////////////////////////////////////////////////////
		bool	SetOwnerParam( const CXVariant *fValue, int nFirstIdx, int nEndIdx );

		//---------------------------------------------------//
		// [9/19/2009 Albert]
		// Description:	重置状态转移标志
		//---------------------------------------------------//
		void	ResetFlag(){ m_bEffected = 0; }

		// 执行作用
		int		DoAction( CXObject* pObj, bool bRemove = false );

	protected:
		const action_entry_ptr getActionEntry()const;
	};

	class CORE_API CXStatus :	public  XObjOnlyTypeT< CXStatus, CXAction, TypeStatus >
	{
	private:
		_uint32	m_hTimerHandler;
	public:
		explicit CXStatus( int nActionID, const action_entry_ptr emap );
		explicit CXStatus( const CXStatus& rsh );
		~CXStatus();

		//---------------------------------------------------//
		// [9/13/2009 Albert]
		// Description:	安装定时器
		//---------------------------------------------------//
		void InstallTimer( _uint32 nCount, float fInterval, float fDelay = 0 );

	protected:
		//---------------------------------------------------//
		// [9/13/2009 Albert]
		// Description:	定时器事件
		//---------------------------------------------------//
		bool OnTimer( unsigned int handle, unsigned short &repeat, unsigned int &timer );
	};
}