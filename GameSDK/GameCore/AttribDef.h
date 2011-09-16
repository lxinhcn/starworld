#ifndef _ATTRIB_DEF_
#define _ATTRIB_DEF_
//////////////////////////////////////////////////////////////////////////
// Object attrib
#define		TypeEffectBase				0x70000000
#define		TypeGameAction				0x60000000
#define		TypeQuestObjective			0x50000000
#define		TypeQuest					0x40000000
#define		TypeMap						0x30000000
#define		TypeSkillBase				0x20000000

#define		TypeServerObject			0x10000000
#define			TypeActorBase			0x11000000
#define			TypeItem				0x12000000
#define			TypeWatcher				0x13000000	// 守望者

#define TIMER_INTERVAL	50
#define TIMER_SECONDS( s )	int((s)*1000.0f/TIMER_INTERVAL)

enum eServerActorBase
{
	attrActorType,	// 角色类型
	attrHealth,		// 生命值
	attrMagic,		// 魔法值
	attrEnergy,		// 能量值
	attrDamage,		// 已累计技能伤害
	attrHealthMax,	// 生命值上限
	attrMagicMax,	// 魔法值上限
	attrEnergyMax,	// 能量值上限
	attrBeatSpeed,	// 击退速度
	attrFaintTime,	// 眩晕时间
	attrAbnormalTime, // 特殊状态时间
	attrActorBornTime, // 出生时间
	ActorBase_attrCount,
};

enum eServerSkillBase
{
	attrConsume,
	attrCastingTime,
	attrCooldownTime,
	attrDamageMin,
	attrDamageMax,
	attrAttackRangeMin,
	attrAttackRangeMax,
	SkillBase_attrCount,
};
#endif // _ATTRIB_DEF_