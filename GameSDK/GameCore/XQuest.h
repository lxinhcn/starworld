#pragma once
#ifndef _XQUEST_H_
#define _XQUEST_H_

namespace XGC
{
	class CXQuest;
	class CXQuestObjective	:	public CXObject
	{
		friend class CXQuest;
	public:
		DECLARE_DYNAMICTYPE(CXObject, TypeQuestObjective);
		enum objective_type
		{
			type_kill,	// 杀死怪物
			type_item,	// 得到物品
		};
		CXQuestObjective( _string strName, _uint16 nFinishCount );
		~CXQuestObjective(void);

		int OnKill( xObject hKiller, xObject hDead );
		int OnItem( xObject hSource, long_ptr lParam );

	protected:
		// 增加计数,若计数和完成计数相同则任务目标达成.
		bool Increment();

	private:
		//objective_type	m_ObjectiveType;	// 目标类型
		_string			m_strName;			// 对象名字
		_uint16			m_nSerial;			// 序号
		_uint16			m_nCount;			// 当前计数
		_uint16			m_nFinishCount;		// 完成计数
	};

	class CXQuest :	public CXObject
	{
		friend class CXQuestObjective;
	public:
		CXQuest(void);
		~CXQuest(void);

		DECLARE_DYNAMICTYPE( CXObject, TypeQuest );
	protected:
		//---------------------------------------------------//
		// [9/15/2009 Albert]
		// Description:	添加任务目标时调用
		//---------------------------------------------------//
		virtual void OnAddChild( CXObject* pChild );

		//---------------------------------------------------//
		// [9/15/2009 Albert]
		// Description:	任务完成时调用
		//---------------------------------------------------//
		virtual void OnQuestFinished() = 0;

		//---------------------------------------------------//
		// [9/15/2009 Albert]
		// Description:	通知任务目标完成
		//---------------------------------------------------//
		void ObjectiveNotify( _uint16 nSerial );

	private:
		_uint16	m_FinishMask;	// 完成掩码
		_uint16 m_Mask;			// 当前掩码
	};
}
#endif // _XQUEST_H_