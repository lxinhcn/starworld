#ifndef _ATTRIB_DEF_
#define _ATTRIB_DEF_
//////////////////////////////////////////////////////////////////////////
// Object attrib
#define		TypeGameAction				0x60000000
#define		TypeQuestObjective			0x50000000
#define		TypeQuest					0x40000000
#define		TypeGameMap					0x30000000
#define			TypeGameScene			0x31000000

#define		TypeSkill					0x20000000

#define		TypeGameObject				0x10000000
#define			TypeDynamicObject		0x11000000
#define				TypeGameActor		0x11100000
#define					TypeGamePlayer	0x11110000
#define					TypeGameNpc		0x11120001
#define					TypeGameMonster	0x11130002
#define			TypeGameItem			0x12000000
#define			TypeGameBuild			0x13000000

#define TIMER_INTERVAL	10
#define TIMER_SECONDS( s )	int(s*1000.0f/TIMER_INTERVAL)

enum EN_DYNAMICOBJECT_ATTRIBUTE
{
	DynamicObject_attrSpeed = 0,
	DynamicObject_attrCanMove,
	_DynamicObject_attrCount,
};
#endif // _ATTRIB_DEF_