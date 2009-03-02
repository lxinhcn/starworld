#pragma once
#ifdef GAMEWORLD_EXPORTS
#define GAMEWORLD_API __declspec(dllexport)
#else
#define GAMEWORLD_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#include "XObjectAI.h"
#include "GameObject.h"

//// 动作定义
//class CMap;
//enum EN_ACTION
//{
//	action_None	= 0,
//	action_Say,				// 内容[Message]
//	action_ChangeDir,		// 更改方向[Dir]
//	action_WaitUserInput,	// 等待用户输入
//	action_Moveto,			// [场景，格子[x,y]]
//	action_Lookfor,			// 寻找周围区域
//};
//
//class GAMEWORLD_API CActionChangeDir :	public CAction
//{
//public:
//	CActionChangeDir( float fDirection, int nHostID );
//	~CActionChangeDir(void);
//
//	virtual BOOL Execute( const CMap& Map );		// 执行动作
//private:
//	float	m_fDir;
//};
//
//class GAMEWORLD_API CActionWaitUserInput	:	public CAction
//{
//public:
//	CActionWaitUserInput();
//	virtual	BOOL Execute( const CMap& Map );
//
//	static void SetUserInput( int nInput ){ m_nUserInput = nInput; }
//private:
//	static int	m_nUserInput;
//};
//
//class GAMEWORLD_API CActionLookfor	:	public	CAction
//{
//public:
//	CActionLookfor( int nSrcObjID );
//	virtual BOOL Execute( const CMap& Map );
//};
