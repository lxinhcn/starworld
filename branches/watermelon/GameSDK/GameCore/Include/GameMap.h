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
	int	m_nMapWidth, m_nMapHeight;
	int m_nBlockSizeX, m_nBlockSizeY;
	int m_nTransfromX, m_nTransfromY;
	
	float m_InvBW, m_InvBH;
public:
	typedef std::list< CMapBlock*, ALLOCATOR< CMapBlock* > > CBlockList;
	typedef std::set< _uint32, LESS< _uint32 >, ALLOCATOR< _uint32 > > CTargetSet;
	typedef std::function< bool( CMapBlock* ) > fnBlockCallback;

protected:
	CGameMap()
		: m_nBlockSizeX( 0 )
		, m_nBlockSizeY( 0 )
		, m_nMapWidth( 0 )	// 地砖横向个数
		, m_nMapHeight( 0 )	// 地砖纵向个数
		, m_pBlockArray( NULL )
		, m_InvBW( 0.0f )
		, m_InvBH( 0.0f )
	{
		// 地图格子信息
	}

	CGameMap( bool bIsParent, bool bIsTypeList )
		: CXObject( bIsParent, bIsTypeList )
		, m_nBlockSizeX( 0 )
		, m_nBlockSizeY( 0 )
		, m_nMapWidth( 0 )	// 地砖横向个数
		, m_nMapHeight( 0 )	// 地砖纵向个数
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
	void InitializeMap( int nMapWidth, int nMapHeight, int nBlockWidth, int nBlockHeight, int nTransfromX, int nTransfromY );
	void DestroyMap();

public:
	DECLARE_DYNAMICTYPE( CXObject, TypeGameMap );

	inline	int	GetMapWidth()const		{ return m_nMapWidth; }
	inline	int	GetMapHeight()const		{ return m_nMapHeight; }
	inline	int	GetBlockWidth()const	{ return m_nBlockSizeX; }
	inline	int	GetBlockHeight()const	{ return m_nBlockSizeY; }
	inline	int	GetWorldWidth()const	{ return m_nMapWidth*m_nBlockSizeX; }
	inline	int	GetWorldHeight()const	{ return m_nMapHeight*m_nBlockSizeY; }

	inline	CMapBlock*	GetBlock( int x, int y )const;

	RECT MapToWorld( RECT rc )const{ rc.left += m_nTransfromX; rc.right += m_nTransfromX; rc.top += m_nTransfromY; rc.bottom += m_nTransfromY; return rc;}
	RECT WorldToMap( RECT rc )const{ rc.left -= m_nTransfromY; rc.right -= m_nTransfromX; rc.top -= m_nTransfromY; rc.bottom -= m_nTransfromY; return rc;}

	POINT MapToWorld( POINT pt )const{ pt.x += m_nTransfromX; pt.y += m_nTransfromY; return pt; }
	POINT WorldToMap( POINT pt )const{ pt.x -= m_nTransfromX; pt.y -= m_nTransfromY; return pt; }

	//---------------------------------------------------//
	// [8/5/2010 Albert]
	// Description:	获取矩形范围内的格子列表
	//---------------------------------------------------//
	int GetBlockList( int x0, int y0, int x1, int y1, CBlockList& ret )const;

	//---------------------------------------------------//
	// [8/5/2010 Albert]
	// Description:	获取矩形范围内的格子列表
	//---------------------------------------------------//
	int GetBlockList( POINT ptBegin, POINT ptEnd, CBlockList& ret )const;

	//---------------------------------------------------//
	// [9/10/2009 Albert]
	// Description:	获取矩形范围内的格子列表
	//---------------------------------------------------//
	int	GetBlockList( RECT rcBlockArea, CBlockList& ret )const;

	//---------------------------------------------------//
	// [9/3/2009 Albert]
	// Description:	获取直线上的格子列表
	//---------------------------------------------------//
	int GetBlockList( POINT ptBegin, POINT ptEnd, int nWidth, CBlockList& ret )const;

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
	RECT GetBlockArea( RECT rcCollision )const;

	//---------------------------------------------------//
	// [8/7/2009 Albert]
	// Description:	获取对象占用的格子范围
	//---------------------------------------------------//
	RECT GetBlockArea( float fPosX, float fPosY, float fRadius )const;
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
	virtual void ExchangeBlock( CDynamicObject *pObj, int x0, int y0, int x1, int y1 );
};
