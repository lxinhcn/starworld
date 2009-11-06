#ifndef _XOBJECT_
#define _XOBJECT_

#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS
#pragma warning( disable:4251 )

#include <map>
#include <set>
#include <loki/Function.h>
#include <loki/Factory.h>
#include <loki/TypeTraits.h>

#include "XAttribute.h"
/************************************************************************/
/* 列表系统，属性系统，以及对象基础类的实现                             */
/************************************************************************/

#define INVALID_OBJID -1
#define INVALID_ATTRIBID -1
#define INVALID_TYPE_VALUE -1
#define INVALID_ATTRIBUTE			(*(CAttribute*)(NULL))
#define INVALID_ATTRIBUTE_VALUE		(*(CXVariant*)(NULL))
#define OBJ_MAX	0xffffff
#define OBJ_MIN	0xffff

#define OBJST_NORMAL	0 
#define OBJST_DESTROY	1

#define GET_LOCAL_VALUE( CLASS, CLASS_PTR, INDEX )		static_cast< CLASS* >( CLASS_PTR )->GetLocalAttrib( INDEX )
#define SET_LOCAL_VALUE( CLASS, CLASS_PTR, INDEX, V )	static_cast< CLASS* >( CLASS_PTR )->SetLocalAttrib( INDEX, V )
#define INIT_LOCAL_VALUE( CLASS, CLASS_PTR, INDEX, V )	static_cast< CLASS* >( CLASS_PTR )->InitLocalAttrib( INDEX, V )

#define		TypeObjectbase				0
#define		TypeAction					0x00000001
#define		TypeStatus					0x00000002
/************************************************************************/


namespace XGC
{
	class CXObject;

	//----------------------------------------------------------------------//

	typedef _uint32 identifier;
	/************************************************************************/
	/* CXObjectList 类，实现了一个静态的指针列表，用来通过ID的方式转化对象指针。
	/************************************************************************/
	class CORE_API CXObjectList
	{
	friend class CXObject;
	protected:
		CXObjectList( size_t nLen );
		~CXObjectList();

		identifier	AddObj( CXObject* pObj );
		void		DelObj( identifier nID );

		struct object_handle
		{
			unsigned int	position:24;	// 位置位域
			unsigned int	round:7;		// 轮询位域
			unsigned int	flag:1;			// 对象位置标志, 1 为服务器使用, 0 为客户端使用.服务器上的对象,最高位永不会使用0;
		};

		CXObject**	m_pObjList;
		size_t		m_nLen;
		size_t		m_nCurID;
		size_t		m_nRound;
		size_t		m_nCount;
	public:
		static CXObjectList& GetInstance();
		inline bool IsValidID( object_handle& h );

		inline CXObject* GetObj( identifier nID )
		{ 
			object_handle& h = (object_handle&)nID; 
			return IsValidID( h )?m_pObjList[h.position]:NULL; 
		}

		inline CXObject* GetObj( identifier nID, _uint32 type );

		inline CXObject* operator []( identifier nID ){ object_handle h = *(object_handle*)&nID; return m_pObjList[h.position]; }
	};

	/************************************************************************/
	/* CXObjectT 模版实现了属性系统，该模版在每个类层次上实现一个属性列表，但在存取
	/* 时需要明确存取的属性是位于哪个类层次上的，这通过一个类的类值来指定（暂无其他方法）。
	/************************************************************************/
	template < typename ThisClassT, typename BaseClassT, unsigned int ClassType, int nAttribCount >
	class XObjTypeT	:	public BaseClassT
	{
	public:
		typedef XObjTypeT BaseClass;
		typedef typename Loki::Int2Type< ClassType >	ClassIdentify;
	protected:
	#define XOBJECT_REPEAT(N) \
		template< CLIB_ENUM_D(N, class T) > \
		XObjTypeT( CLIB_PARAM_D(N,T,&P) )\
			: BaseClassT( CLIB_ENUM_D(N,P) )\
		{\
			ASSERT_MSG( IsTypeOf( TypeObjectbase ), _T("错误的继承类型。") );\
			ZeroMemory( _attrib, sizeof( _attrib ) );\
		}\
	
	// CLIB_MAIN_REPEAT(10,XOBJECT_REPEAT)
	CLIB_MAIN_REPEAT(10,XOBJECT_REPEAT)
	#undef SLB_REPEAT
	#undef XOBJECT_REPEAT

	XObjTypeT()
	{
		ASSERT_MSG( IsTypeOf( TypeObjectbase ), _T("错误的继承类型。") );
		ZeroMemory( _attrib, sizeof( _attrib ) );
	}

	template<>
	XObjTypeT( const ThisClassT& src )
		: BaseClassT( src )
	{
		ASSERT_MSG( IsTypeOf( TypeObjectbase ), _T("错误的继承类型。") );
		memcpy( (void*)_attrib, (void*)src._attrib, sizeof(_attrib) );
	}

	private:
		// 里面的各个元素,在第一时间确定所属类型,之后不能更改类型.
		CAttrib _attrib[nAttribCount];

	public:
		static	_uint32	GetPointType()				{ return ClassType; }
		virtual _uint32	GetType()const				{ return ClassType; }
		virtual bool	IsTypeOf( _uint32 dwType )const	{ if( GetPointType() == dwType ) return true; else return BaseClassT::IsTypeOf( dwType );}
		static	bool	IsPointOf( _uint32 dwType )		{ if( GetPointType() == dwType ) return true; else return BaseClassT::IsPointOf( dwType );}

		virtual CAttribute GetAttribute( int nType, int nIndex, bool bNotifyListener = false )
		{
			if( GetPointType() == nType )
			{
				return GetLocalAttribute( nIndex, bNotifyListener );
			}
			return BaseClassT::GetAttribute( nType, nIndex, bNotifyListener );
		}

		virtual CXVariant GetAttrib( int nType, int nIndex )const
		{
			if( GetPointType() == nType )
			{
				return GetLocalAttrib( nIndex );
			}
			return BaseClassT::GetAttrib( nType, nIndex );
		}

		virtual void SetAttrib( int nType, int nIndex, CXVariant Value, bool bNotifyListener = false )
		{
			if( GetPointType() == nType )
			{
				SetLocalAttrib( nIndex, Value, bNotifyListener );
			}
			else
			{
				BaseClassT::SetAttrib( nType, nIndex, Value, bNotifyListener );
			}
		}

		__inline CAttribute GetLocalAttribute( int nIndex, bool bNotifyListener = false )
		{
			ASSERT_MSG( nIndex >= 0 && nIndex < nAttribCount, _T("属性值超出范围") );
			if( nIndex < 0 || nIndex >= nAttribCount )	return INVALID_ATTRIBUTE;
			if( bNotifyListener )
				return CAttribute( _attrib[nIndex], m_Listener, GetPointType(), nIndex );
			else
				return CAttribute( _attrib[nIndex] );
		}

		// 取类的本地属性
		__inline CXVariant GetLocalAttrib( int nIndex )const
		{
			ASSERT_MSG( nIndex >= 0 && nIndex < nAttribCount, _T("属性值超出范围") );
			if( nIndex < 0 || nIndex >= nAttribCount )	return INVALID_ATTRIBUTE_VALUE;
			return _attrib[nIndex].GetValue();
		}

		// 设置本地属性
		__inline void SetLocalAttrib( int nIndex, CXVariant Value, bool bNotifyListener = false )
		{
			ASSERT_MSG( nIndex >= 0 && nIndex < nAttribCount, _T("属性值超出范围") );
			if( nIndex < 0 || nIndex >= nAttribCount )	return;
			_attrib[nIndex].SetValue( Value );
			if( bNotifyListener && m_Listener )
			{
				m_Listener( GetPointType(), nIndex, Value );
			}
		}

		__inline void InitLocalAttrib( int nIndex, CXVariant Value )
		{
			ASSERT_MSG( nIndex >= 0 && nIndex < nAttribCount, _T("属性值超出范围") );
			if( nIndex < 0 || nIndex >= nAttribCount )	return;
			_attrib[nIndex].InitValue( Value );
		}
	};

	/************************************************************************/
	/* CXObjectT 模版实现了属性系统，该模版在每个类层次上实现一个属性列表，但在存取
	/* 时需要明确存取的属性是位于哪个类层次上的，这通过一个类的类值来指定（暂无其他方法）。
	/************************************************************************/
	template < typename ThisClassT, typename BaseClassT, unsigned int ClassType >
	class XObjOnlyTypeT : public BaseClassT
	{
	public:
		typedef XObjOnlyTypeT BaseClass;
		typedef typename Loki::Int2Type< ClassType >	ClassIdentify;
	protected:
	#define XOBJECT_REPEAT(N) \
	template< CLIB_ENUM_D(N, class T) > \
	XObjOnlyTypeT( CLIB_PARAM_D(N,T,&P) )\
		: BaseClassT( CLIB_ENUM_D(N,P) )\
	{\
		ASSERT_MSG( IsTypeOf( TypeObjectbase ), _T("错误的继承类型。") );\
	}\

	CLIB_MAIN_REPEAT(10,XOBJECT_REPEAT)
	#undef SLB_REPEAT
	#undef XOBJECT_REPEAT

	XObjOnlyTypeT()
	{
		ASSERT_MSG( IsTypeOf( TypeObjectbase ), _T("错误的继承类型。") );
	}

	template<>
	XObjOnlyTypeT( const ThisClassT& src )
		: BaseClassT( src )
	{
		ASSERT_MSG( IsTypeOf( TypeObjectbase ), _T("错误的继承类型。") );
	}

	public:
		static	_uint32	GetPointType()				{ return ClassType; }
		virtual _uint32	GetType()const				{ return ClassType; }
		virtual bool	IsTypeOf( _uint32 dwType )const	{ if( GetPointType() == dwType ) return true; else return BaseClassT::IsTypeOf( dwType );}
		static	bool	IsPointOf( _uint32 dwType )		{ if( GetPointType() == dwType ) return true; else return BaseClassT::IsPointOf( dwType );}
	};

	/************************************************************************/
	/* CXObject 类，所有带有对象属性的基础类。这里定义了一组属性系统的接口，并提
	/* 供了底层实现。
	/* 同时，CXObject类实现了自动列表，当对象生成后，将自己插入对象列表中，并初始化
	/* 当前ID
	/************************************************************************/
	/*	:	public XObjTypeT< CXObjectBase, EN_OBJECT, 1 > */
	class CORE_API CXObject
	{
	protected:
		CXObject();
		CXObject( bool bIsParent, bool bIsTypeList );
		virtual	~CXObject();

	protected:
		const _byte	m_bIsParent;	// 是否为子物体的父
		const _byte	m_bIsTypeList;	// 是否分类子物体

		_byte m_nStatus;	// 当前状态

		ListenFunction	m_Listener;
		bool IsDestroy()const{ return m_nStatus&OBJST_DESTROY; }
	private:
		identifier	m_nID;
		identifier	m_nParentID;	// 父物体ID

		typedef std::set< identifier >			CChildList;
		typedef std::map< int, CChildList >		CTypeList;
		typedef std::map< _string, _string >	CStrAttribMap;

		CChildList		m_ChildList;	// 子物体列表
		CTypeList		m_TypeList;		// 子物体分类表
		CStrAttribMap	m_strAttrib;

	public:
		const	identifier	GetObjID()const			{ return m_nID; }
		static	_uint32		GetPointType()			{ return TypeObjectbase; }
		static	bool		IsPointOf( _uint32 dwType )		{ return TypeObjectbase == dwType;}
		virtual _uint32		GetType()const			{ return TypeObjectbase; }
		virtual bool		IsTypeOf( _uint32 dwType )const	{ return TypeObjectbase == dwType;}

		virtual CXVariant	GetAttrib( int nType, int nIndex )const	{ ASSERT( false ); return INVALID_ATTRIBUTE_VALUE; }
		virtual void		SetAttrib( int nType, int nIndex, CXVariant Value, bool bNotifyListener = false ){ ASSERT( false ); }
		virtual CAttribute	GetAttribute( int nType, int nIndex, bool bNotifyListener = false ){ ASSERT( false ); return INVALID_ATTRIBUTE; }

		// 字符串属性表
		_lpctstr GetStrAttrib( _lpctstr szName )
		{
			CStrAttribMap::const_iterator citer = m_strAttrib.find( szName );
			if( citer != m_strAttrib.end() )
			{
				const _string& str = citer->second;	
				return str.c_str();
			}
			return NULL;
		}

		bool SetStrAttrib( _lpctstr szName, _lpctstr strValue )
		{
			if( strValue == NULL || szName == NULL )	
				return false;

			m_strAttrib[szName] = strValue;	
			return true;
		}

		// 字符串属性表
		bool GetStrAttribA( _lpcstr szName, _string& strValue )
		{
			CStrAttribMap::const_iterator citer = m_strAttrib.find( XA2TSTR( szName ) );
			if( citer != m_strAttrib.end() )
			{
				strValue = citer->second;
				return true;
			}
			return false;
		}

		bool SetStrAttribA( _lpcstr szName, _lpcstr strValue )
		{
			if( szName == NULL )
				return false;

			if( strValue == NULL )
				m_strAttrib.erase( XA2TSTR(szName) );

			m_strAttrib[XA2TSTR(szName)] = XA2TSTR( strValue );
			return true;
		}

		//---------------------------------------------------//
		// [9/17/2009 Albert]
		// Description:	设置监听函数
		//---------------------------------------------------//
		void SetAttributeListener( ListenFunction &Fn )
		{
			m_Listener = Fn;
		}

		/************************************************************************/
		/* 层级关系函数
		/************************************************************************/
		// 设置父对象
		void	SetParent( identifier nID ) { m_nParentID = nID; }
		// 得到父对象ID
		identifier	GetParent()const { return m_nParentID; }
		// 添加子对象ID
		__inline 
		bool	AddChild( identifier nID, bool bChangeParent = true );
		bool	AddChild( CXObject* pObj, bool bChangeParent = true );
		// 删除子对象ID
		__inline 
		void	RemoveChild( identifier nID, bool bRelease = false );
		void	RemoveChild( CXObject* pObj, bool bRelease = false );
		// 枚举子对象
		template< class _Func >
		_uint32	EnumChild( _Func &CallbackFunc, int nTypeFilter )
		{
			CChildList& child = ( nTypeFilter != INVALID_TYPE_VALUE )?m_TypeList[nTypeFilter]:m_ChildList;

			CChildList::const_iterator cb = child.begin();
			CChildList::const_iterator ce = child.end();
			for( ; cb != ce; ++cb )
			{
				if( CallbackFunc( *cb ) )
					return *cb;
			}

			return INVALID_OBJID;
		}

		/************************************************************************/
		// 销毁对象
		bool	Destroy();
		// 销毁所有子对象
		void	DestroyAllChild();

		//---------------------------------------------------//
		// [9/21/2009 Albert]
		// Description:	销毁所有子对象
		//---------------------------------------------------//
		void	DestroyAllChild( int nType );

		// 获取子对象数量
		size_t	GetChildrenCount()const{ return m_ChildList.size(); }
		size_t	GetChildrenCount( int nType )const
		{ 
			CTypeList::const_iterator i = m_TypeList.find( nType ); 
			return i == m_TypeList.end()?0:i->second.size();
		}

	private:
		/************************************************************************/
		/* 设置的一些事件响应虚函数。
		/************************************************************************/

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	增加子节点前调用
		// return :		true - 确认增加子节点, false - 子节点被否决,添加节点失败.
		//---------------------------------------------------//
		virtual bool PreAddChild( CXObject* pChild ){ return true; }

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	增加子节点前调用
		// return :		true - 确认移除子节点, false - 子节点被否决,移除子节点失败.
		//---------------------------------------------------//
		virtual bool PreRemoveChild( CXObject* pChild, bool bRelease ){ return true; }

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	加入子节点后调用
		//---------------------------------------------------//
		virtual void OnAddChild( CXObject* pChild ){}

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	删除子节点后调用,此时对象尚未被删除
		//---------------------------------------------------//
		virtual void OnRemoveChild( CXObject* pChild, bool bRelease ){}

		//---------------------------------------------------//
		// [8/3/2009 Albert]
		// Description:	对象被删除时调用,此时对象尚未被上除
		//---------------------------------------------------//
		virtual void OnDestroy(){}
	};

}
#endif //_XOBJECT_