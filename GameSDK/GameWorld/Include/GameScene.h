#pragma once
#include "GameObject.h"
#include "Animation.h"
#include "Event.h"
#include "DataHelper.h"

#ifdef GAMEWORLD_EXPORTS
#define GAMEWORLD_API __declspec(dllexport)
#else
#define GAMEWORLD_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#define		PICTURE_COUNT	5
//////////////////////////////////////////////////////////////////////////
// MapBlock 格子管理
class GAMEWORLD_API CMapBlock
{
public:
	BOOL	m_bAllow;		// 是否允许通过
	int		m_nHeight;		// 高度
	int		m_nIntension;	// 强度
	POINT	m_ptPos;		// 格子坐标
	CAnimation*	m_pAni;		// 建筑动画控制器
	typedef std::list< DWORD >	CObjList;
	CObjList		m_ObjList;

	CMapBlock()
	{
		m_pAni		= NULL;
	}

	~CMapBlock()
	{
		if( m_pAni )
		{
			m_pAni->Release();
			m_pAni = NULL;
		}
	};

	// 设置格子索引
	void SetPos( int nPosX, int nPosY )
	{
		m_ptPos.x = nPosX;
		m_ptPos.y = nPosY;
	}

	// 得到格子索引
	void GetPos( int& nPosX, int& nPosY )
	{
		nPosX = m_ptPos.x;
		nPosY = m_ptPos.y;
	}

	//////////////////////////////////////////////////////////////////////////
	// ObjectManage
	// 添加对象引用
	void AddObject( CGameObject* pObj )
	{
		if( !pObj )	return;
		ASSERT_MSG( pObj->IsType( TypeMapobj ), _T("非法的对象类型。") );
		m_ObjList.push_front( pObj->GetObjID() );
	}

	void AddObject( int nID )
	{
		CXObject* pObj = CXObjectList::GetInstance().GetObj( nID );
		if( !pObj || !pObj->IsType( TypeMapobj ) )	return;
		ASSERT_MSG( pObj->IsType( TypeMapobj ), _T("非法的对象类型。") );
		m_ObjList.push_front( nID );
	}

	void DelObject( int nObjID )
	{
		CObjList::iterator iter = m_ObjList.begin();
		while( iter != m_ObjList.end() )
		{
			if( (*iter) == nObjID )
			{
				m_ObjList.erase( iter );
				break;
			}
			iter++;
		}
	}

	void DelObject( CGameObject* pObj )
	{
		DelObject( pObj->GetObjID() );
	}
	//////////////////////////////////////////////////////////////////////////
	
	void SetAnimation( CAnimation* pAni )
	{
		ASSERT_MSG( !m_pAni, _T("非法的重复设置。") );
		if( m_pAni )	m_pAni->Release();
		m_pAni = pAni; 
		m_pAni->AddRef();
	}

};

class GAMEWORLD_API CMap	: public XObjTypeT< CXObject, TypeScene >
{
private:
	// 地图
	// 一个表面里包含了所有的地形贴图

	CMapBlock*	m_pBlockArray;

	//typedef std::map< int, CGameObject* >	CObjMap;
	typedef std::list< CAnimation* >		CAnimationList;

	//CObjMap			m_ObjMap;			// ID映射表,该表中的ID仅存在一个对象
	CAnimationList	m_AnimationList;	// 渲染器列表

protected:
	DWORD	m_dwMapWidth;
	DWORD	m_dwMapHeight;
	SIZE	m_siBlockSize;
	int		m_nRoleID;			// 主角ID, 可被控制对象的ID;

public:
	typedef std::list< CMapBlock* >		CBlockList;

public:
	CMap()
	{
		// 地图格子信息
		m_dwMapWidth			= 0;	// 地砖横向个数
		m_dwMapHeight			= 0;	// 地砖纵向个数
		m_pBlockArray			= NULL;
		m_nRoleID				= INVALID_OBJID;
		m_pBlockArray			= NULL;
		ZeroMemory( &m_siBlockSize, sizeof( m_siBlockSize ) );
	}

	virtual ~CMap()
	{
		CAnimationList::iterator iter = m_AnimationList.begin();
		while( iter != m_AnimationList.end() )
		{
			int nRef = (*iter)->Release();
			ASSERT_MSG( !nRef, _T("未释放的渲染器，可能会造成内存泄露。") );
			++iter;
		}

		if( m_pBlockArray )
		{
			delete[] m_pBlockArray;
			m_pBlockArray = NULL;
		}
	}

	// 初始化地图
	VOID InitializeMap( DWORD dwMapWidth, DWORD dwMapHeight, int nRoleID = INVALID_OBJID );

	inline	DWORD	GetMapWidth()const		{ return m_dwMapWidth; }
	inline	DWORD	GetMapHeight()const		{ return m_dwMapHeight; }
	inline	DWORD	GetBlockWidth()const	{ return m_siBlockSize.cx; }
	inline	DWORD	GetBlockHeight()const	{ return m_siBlockSize.cy; }
	inline	DWORD	GetWorldWidth()const	{ return m_dwMapWidth* m_siBlockSize.cx; }
	inline	DWORD	GetWorldHeight()const	{ return m_dwMapHeight*m_siBlockSize.cy; }

	inline	CMapBlock*	GetBlock( DWORD x, DWORD y )const;

	// 设置主角ID
	inline	VOID	SetRoleID( int nID )	{ m_nRoleID = nID; }
	inline	int		GetRoleID()const		{ return m_nRoleID; }

	BOOL	GetCollisionBlock( const RECT& rc, CBlockList& retList )const;
	DWORD	Update( float fDeltaTime );

	virtual BOOL	CreateMap( LPCSTR lpszMapFilename ) = 0;

protected:
	inline	void	SetBlockSize( SIZE siBlockSize ){	m_siBlockSize = siBlockSize; }

	BOOL	PutObjToMap( int nObjID );			// 将对象放入地图
	BOOL	RemoveObjFromMap( int nObjID );		// 将对象从地图移出

	BOOL	MoveObject( CDynamic& Obj, float fDirection, float fSpeed );
	BOOL	TestCollision( CMapBlock* pBlock, const CRect& CollisionRect, int nObjID );

	void	AddAnimation( CAnimation* pAni )	{ pAni->AddRef(); m_AnimationList.push_back( pAni ); }

	// 角色移动时调用，返回false则移动取消
	virtual bool OnDynamicMove( CDynamic& pObj, float& fNewX, float& fNewY, float& fNewZ ){ return true; }

	static	bool OnEnumChild( int nID, CXObject* pObj, LPVOID pParam );
	virtual bool OnAddChild(int nID );
	virtual bool OnRemoveChild( int nID, bool bRelease );

};