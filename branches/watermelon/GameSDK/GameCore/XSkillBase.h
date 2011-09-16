#pragma once
using namespace XGC;

#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif

namespace XGC
{
	class CGameObject;
	class CORE_API XSkillBase : public CXObject
	{
	public:
		DECLARE_DYNAMICTYPE( CXObject, TypeSkillBase );
		DECLARE_ATTRIBUTE( SkillBase_attrCount );

		enum { SkillBegin, SkillCasting, SkillCooldown, SkillOver };
		XSkillBase();
		~XSkillBase(void);

		//---------------------------------------------------//
		// [12/29/2010 Albert]
		// Description:	开始使用技能 
		//---------------------------------------------------//
		void CastSkill( xObject hTarget );

		//---------------------------------------------------//
		// [12/3/2010 Albert]
		// Description:	攻击目标
		// return : 技能释放需要使用的时间
		//---------------------------------------------------//
		int Attack( xObject hTarget );

		//---------------------------------------------------//
		// [12/6/2010 Albert]
		// Description:	设置技能释放状态 
		//---------------------------------------------------//
		void MarkCasting( float fDelay );

		//---------------------------------------------------//
		// [12/11/2010 Albert]
		// Description:	设置技能冷却状态 
		//---------------------------------------------------//
		void MarkCooldown( float fDelay );

		//---------------------------------------------------//
		// [12/6/2010 Albert]
		// Description:	技能是否在吟唱 
		//---------------------------------------------------//
		bool IsCasting()const{ return m_hCastingTimerHandler != INVALID_TIMER_HANDLE; }

		//---------------------------------------------------//
		// [12/11/2010 Albert]
		// Description:	技能是否冷却中 
		//---------------------------------------------------//
		bool IsCooldown()const{ return m_hCooldownTimerHandler != INVALID_TIMER_HANDLE; }

		//---------------------------------------------------//
		// [12/27/2010 Albert]
		// Description:	技能是否结束
		//---------------------------------------------------//
		bool IsOver()const{ return m_hLifetimeTimerHandler == INVALID_TIMER_HANDLE; }

		//---------------------------------------------------//
		// [12/6/2010 Albert]
		// Description:	清除吟唱状态 
		//---------------------------------------------------//
		void ClearCasting( bool bTrigger = false );

		//---------------------------------------------------//
		// [12/6/2010 Albert]
		// Description:	清除冷却状态 
		//---------------------------------------------------//
		void ClearCooldown( bool bTrigger = false );

		//---------------------------------------------------//
		// [12/6/2010 Albert]
		// Description:	清除释放状态
		//---------------------------------------------------//
		void ClearLifetime( bool bTrigger = false );

	protected:
		//---------------------------------------------------//
		// [12/6/2010 Albert]
		// Description:	重置技能释放状态 
		//---------------------------------------------------//
		bool ResetCasting( timer_t handle );

		//---------------------------------------------------//
		// [12/11/2010 Albert]
		// Description:	重置技能冷却状态 
		//---------------------------------------------------//
		bool ResetCooldown( timer_t handle );

		//---------------------------------------------------//
		// [12/6/2010 Albert]
		// Description:	重置技能释放状态 
		//---------------------------------------------------//
		bool ResetLifetime( timer_t handle );

		//---------------------------------------------------//
		// [12/3/2010 Albert]
		// Description:	添加子物体的时候调用，这里用于判断是否是Action类型
		//---------------------------------------------------//
		virtual bool PreAddChild( CXObject* pChild );

	private:
		timer_t m_hCastingTimerHandler;
		timer_t m_hCooldownTimerHandler;
		timer_t m_hLifetimeTimerHandler;
	};
}
