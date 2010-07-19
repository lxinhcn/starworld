#pragma once
#include "GameObject.h"
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS

#define BLOCK_ADD		1
#define BLOCK_DEL		2
#define BLOCK_EXCANGE	3
//////////////////////////////////////////////////////////////////////////
// MapBlock 格子管理
class CORE_API CMapBlock
{
public:
	bool	m_bAllow;			// 是否允许通过
	float	m_fHeight;			// 高度

	typedef std::set< _uint32, LESS< _uint32 >, ALLOCATOR< _uint32 > >	CObjList;
	CObjList m_ObjList;

	CMapBlock()
	{
	}

	~CMapBlock()
	{
	};

	DEBUG_CODE(	bool CheckExist( _uint32 nID )const{ return m_ObjList.find( nID ) != m_ObjList.end(); })
	//////////////////////////////////////////////////////////////////////////
	// ObjectManage
	// 添加对象引用

	void AddObject( _uint32 nID )
	{
		m_ObjList.insert( nID );
	}

	size_t DelObject( _uint32 nObjID )
	{
		return m_ObjList.erase( nObjID );
	}

	template< class func >
	void for_each_object( func &pfnCall )
	{
		std::for_each( m_ObjList.begin(), m_ObjList.end(), pfnCall );
	}

	bool Empty()const{ return m_ObjList.empty(); }
};

class CORE_API CGameMap	: public CXObject
{
private:
	// 地图
	CMapBlock*	m_pBlockArray;

protected:
	xgcSize	m_siMapSize;
	xgcSize	m_siBlockSize;
	xgcPoint m_ptTransfrom;
	
	float m_InvBW, m_InvBH;
public:
	typedef std::list< CMapBlock*, ALLOCATOR< CMapBlock* > >	CBlockList;
	typedef std::set< _uint32, LESS< _uint32 >, ALLOCATOR< _uint32 > >			CTargetSet;
	typedef std::function< bool( CMapBlock* ) > fnBlockCallback;

protected:
	CGameMap()
		: m_siBlockSize( xgcSize( 0, 0 ) )
		, m_siMapSize( xgcSize( 0, 0 ) )	// 地砖横向个数
		, m_pBlockArray( NULL )
		, m_InvBW( 0.0f )
		, m_InvBH( 0.0f )
	{
		// 地图格子信息
	}

	CGameMap( bool bIsParent, bool bIsTypeList )
		: CXObject( bIsParent, bIsTypeList )
		, m_siBlockSize( xgcSize( 0, 0 ) )
		, m_siMapSize( xgcSize( 0, 0 ) )	// 地砖横向个数
		, m_pBlockArray( NULL )
		, m_InvBW( 0.0f )
		, m_InvBH( 0.0f )
	{
		// 地图格子信息
	}

	virtual ~CGameMap()
	{
		DestroyMap();
	}

	// 初始化地图
	void InitializeMap( _uint32 nBlockWidth, _uint32 nBlockHeight, SIZE siBlockSize, POINT ptTransfrom );
	void DestroyMap();

public:
	DECLARE_DYNAMICTYPE( CXObject, TypeGameMap );

	inline	_uint32	GetMapWidth()const		{ return m_siMapSize.cx; }
	inline	_uint32	GetMapHeight()const		{ return m_siMapSize.cy; }
	inline	_uint32	GetBlockWidth()const	{ return m_siBlockSize.cx; }
	inline	_uint32	GetBlockHeight()const	{ return m_siBlockSize.cy; }
	inline	_uint32	GetWorldWidth()const	{ return m_siMapSize.cx*m_siBlockSize.cx; }
	inline	_uint32	GetWorldHeight()const	{ return m_siMapSize.cy*m_siBlockSize.cy; }

	inline	CMapBlock*	GetBlock( int x, int y )const;

	xgcRect	MapToWorld( const xgcRect& rc )const{ return rc + m_ptTransfrom; }
	xgcRect WorldToMap( const xgcRect& rc )const{ return rc - m_ptTransfrom; }

	xgcPoint MapToWorld( const xgcPoint& pt )const{ return pt + m_ptTransfrom; }
	xgcPoint WorldToMap( const xgcPoint& pt )const{ return pt - m_ptTransfrom; }

	//---------------------------------------------------//
	// [9/10/2009 Albert]
	// Description:	获取矩形范围内的格子列表
	//---------------------------------------------------//
	int	GetBlockList( const xgcRect& rcBlockArea, CBlockList& ret )const;

	//---------------------------------------------------//
	// [9/3/2009 Albert]
	// Description:	获取直线上的格子列表
	//---------------------------------------------------//
	int GetBlockList( xgcPoint ptBegin, xgcPoint ptEnd, int nWidth, CBlockList& ret )const;

	//---------------------------------------------------//
	// [9/3/2009 Albert]
	// Description:	获取区域内的所有对象ID
	//---------------------------------------------------//
	static
	int	GetTargets( const CBlockList& bl, CTargetSet& ret );


	//---------------------------------------------------//
	// [8/7/2009 Albert]
	// Description:	移动对象
	//---------------------------------------------------//
	bool DynamicMoveTo( CDynamicObject* Obj, XVector3 &vPosition, bool bCollisionTest = true );

	//---------------------------------------------------//
	// [8/7/2009 Albert]
	// Description:	获取对象占用的格子范围
	//---------------------------------------------------//
	xgcRect	GetBlockArea( const xgcRect& rcCollision )const;

	//---------------------------------------------------//
	// [8/7/2009 Albert]
	// Description:	获取对象占用的格子范围
	//---------------------------------------------------//
	xgcRect	GetBlockArea( float fPosX, float fPosY, float fRadius )const;
protected:

	//---------------------------------------------------//
	// [8/6/2009 Albert]
	// Description:	添加子节点前被调用
	//---------------------------------------------------//
	virtual bool PreAddChild( CXObject* pObj );	// 将对象放入地图

	//---------------------------------------------------//
	// [8/6/2009 Albert]
	// Description:	删除子节点前被调用
	//---------------------------------------------------//
	virtual bool PreRemoveChild( CXObject* pObj, bool bRelease );

	//---------------------------------------------------//
	// [8/3/2009 Albert]
	// Description:	碰撞检测
	//---------------------------------------------------//
	virtual bool TestCollision( CMapBlock* pBlock, CGameObject *pObj );

	//---------------------------------------------------//
	// [8/3/2009 Albert]
	// Description:	角色移动后调用
	// pObj		:	移动的对象
	// nPosX, nPosY, nPosZ:	目标点坐标
	//---------------------------------------------------//
	virtual void OnDynamicMove( CDynamicObject *pObj, XVector3 &vPosition );

	//---------------------------------------------------//
	// [8/3/2009 Albert]
	// Description:	对象所占格子发生改变时调用
	//---------------------------------------------------//
	virtual void ExchangeBlock( CDynamicObject* pObj, const xgcPoint *pOldBlock, const xgcPoint *pNewBlock );
};
