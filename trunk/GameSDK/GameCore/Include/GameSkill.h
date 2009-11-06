#pragma once
#include "excel.h"
using namespace XGC;

#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif

struct config;
class CGameActor;
class CORE_API CGameSkill : public XObjOnlyTypeT< CGameSkill, CXObject, TypeSkill >
{
public:
	CGameSkill( _uint32 hOwner, _int16 nSerial );
	~CGameSkill(void);

	_int16 GetSerial()const{ return m_nSerial; }

	void IncreChildrenSerial(){ ++m_nChildrenSerial; }
	void ResetChildrenSerial(){ m_nChildrenSerial = 0; }
	_int16 GetChildrenSerial()const{ return m_nChildrenSerial; }

	CGameActor* GetOwner()const;
private:
	_int16 m_nSerial;			// 编号
	_int16 m_nChildrenSerial;	// 使用序号
	_uint32 m_hOwner;			// 技能所有者的句柄
};

