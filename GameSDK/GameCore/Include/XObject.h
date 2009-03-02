#ifndef _XOBJECT_
#define _XOBJECT_

#include <comutil.h>
#ifdef _DEBUG
#pragma comment( lib, "comsuppwd.lib" )
#else
#pragma comment( lib, "comsuppw.lib" )
#endif
#ifdef CORE_EXPORTS
#define CORE_API __declspec(dllexport)
#else
#define CORE_API __declspec(dllimport)
#endif	//CORE_EXPORTS
#pragma warning( disable:4251 )

#ifdef _UNICODE
typedef std::wstring _string;
#else
typedef std::string _string;
#endif

/************************************************************************/
/* 列表系统，属性系统，以及对象基础类的实现                               */
/************************************************************************/
#define INVALID_OBJID -1
#define INVALID_ATTRIBID -1
#define OBJ_MAX	65535

#define		TypeObjectbase				0

#define		TypeScene					0x30000000

#define		TypeEvent					0x20000000
#define			TypeMapevent			0x21000000
#define			TypeObjevent			0x22000000

#define		TypeMapobj					0x10000000
#define			TypeDynamic				0x11000000
#define				TypeSprite			0x11100000
#define			TypeItem				0x12000000
#define			TypeBuild				0x13000000

#define		TypeEffect					0x00000001
/************************************************************************/

#include <list>
#include <map>
class CXObject;

//----------------------------------------------------------------------//

/************************************************************************/
/* CXObjectList 类，实现了一个静态的指针列表，用来通过ID的方式转化对象指针。
/************************************************************************/
class CORE_API CXObjectList
{
friend class CXObject;
protected:
	CXObjectList( int nLen );
	~CXObjectList();

	int		AddObj( CXObject* pObj );
	void	DelObj( int nID );

	CXObject**	m_pObjList;
	int			m_nLen;
	int			m_nCurID;
	int			m_nCount;
public:
	static CXObjectList& GetInstance();
	bool IsValidID( int nID ) { return nID != INVALID_OBJID && nID >= 0 && nID < m_nLen; }

	CXObject*	GetObj( int nID ){ return IsValidID( nID )?m_pObjList[nID]:NULL; }
	CXObject*	operator []( int nID ){ return m_pObjList[nID]; }
};

struct AttribValueListener
{
	virtual BOOL OnAttribChange( _variant_t OldValue, _variant_t NewValue ) = 0;
};

class CAttrib
{
public:
	CAttrib()
	: Value( 0 )
	, NowValue( 0 )
	, mListener( NULL )
	{
	}

	~CAttrib()
	{

	}

	void SetListener( AttribValueListener* pListener )
	{
		mListener = pListener;
	}

	void ChangeValue( BOOL bPersent, const _variant_t& Val )
	{
		// DoDefaultEffect 函数的调用在这里会出问题。小心，回头再改吧！
		switch( V_VT(&Val) )
		{
		case VT_I4:
			{
				int nTmpValue = 0;

				if( bPersent )
					nTmpValue = V_INT(&NowValue) + (int)( V_INT(&Value) * V_INT(&Val) / 100.0f );
				else
					nTmpValue = V_INT(&NowValue) + V_INT(&Val);

				if( ( mListener == NULL ) || ( mListener && mListener->OnAttribChange( NowValue, nTmpValue ) ) )
				{
					NowValue = nTmpValue;
				}
			}
			break;
		case VT_R4:
			{
				float fTmpValue = 0.0f;

				if( bPersent )
					fTmpValue = V_R4(&NowValue) + V_R4(&Value) * V_R4(&Val) / 100.0f;
				else
					fTmpValue = V_R4(&NowValue) + V_R4(&Val);

				if( ( mListener == NULL ) || ( mListener && mListener->OnAttribChange( NowValue, fTmpValue ) ) )
				{
					NowValue = fTmpValue;
				}
			}
			break;
		case VT_R8:
			{
				double fTmpValue = 0.0f;

				if( bPersent )
					fTmpValue = V_R8(&NowValue) + V_R8(&Value) * V_R8(&Val) / 100.0f;
				else
					fTmpValue = V_R8(&NowValue) + V_R8(&Val);

				if( ( mListener == NULL ) || ( mListener && mListener->OnAttribChange( NowValue, fTmpValue ) ) )
				{
					NowValue = fTmpValue;
				}
			}
			break;
		}
	}

	void	SetValue( const _variant_t& Val )
	{ 
		if( Val == NowValue )
		{
			return;
		}
		if( ( mListener == NULL ) || ( mListener && mListener->OnAttribChange( NowValue, Val ) ) )
		{
			Value = Val;
			NowValue = Val; 
		}
	}

	const _variant_t&	GetValue()const { return NowValue; }

private:
	AttribValueListener* mListener;
	_variant_t Value;		// 基准值
	_variant_t NowValue;	// 当前值,经过换算的值
};

/************************************************************************/
/* CXObjectT 模版实现了属性系统，该模版在每个类层次上实现一个属性列表，但在存取
/* 时需要明确存取的属性是位于哪个类层次上的，这通过一个类的类值来指定（暂无其他方法）。
/************************************************************************/
template < typename TBase, int TYPE, int nAttribCount = 1 >
class XObjTypeT	:	public TBase
{
protected:
	XObjTypeT()
	{
		ASSERT_MSG( IsTypeOf( TypeObjectbase ), _T("错误的继承类型。") );
		ZeroMemory( _attrib, sizeof( _attrib ) );
	}

private:
	// 里面的各个元素,在第一时间确定所属类型,之后不能更改类型.
	CAttrib _attrib[nAttribCount];

	typedef std::map< _string, _string >	CStrAttribMap;
	typedef std::map< _string, LPVOID >		CBufAttribMap;

	CStrAttribMap	_strAttrib;
	CBufAttribMap	_bufAttrib;

public:
	static	int		GetPointType()				{ return TYPE; }
	virtual int		GetType()const				{ return TYPE; }
	virtual BOOL	IsType( DWORD dwType )const	{ if( GetPointType() == dwType ) return TRUE; else return TBase::IsType( dwType );}
	static	BOOL	IsTypeOf( DWORD dwType )	{ if( GetPointType() == dwType ) return TRUE; else return TBase::IsTypeOf( dwType );}

	virtual BOOL SetAttribListener( int nType, int nIndex, AttribValueListener* pListener )
	{
		if( GetPointType() == nType )
		{
			return SetLocalAttribListener( nIndex, pListener );
		}
		if( GetPointType() == TBase::GetPointType() )	return FALSE;
		return TBase::SetAttribListener( nType, nIndex, pListener );
	}

	virtual BOOL GetAttrib( int nType, int nIndex, _variant_t& Value )
	{
		if( GetPointType() == nType )
		{
			return GetLocalAttrib( nIndex, Value );
		}
		if( GetPointType() == TBase::GetPointType() )	return FALSE;
		return TBase::GetAttrib( nType, nIndex, Value );
	}

	virtual BOOL SetAttrib( int nType, int nIndex, _variant_t Value )
	{
		if( GetPointType() == nType )
		{
			return SetLocalAttrib( nIndex, Value );
		}
		if( GetPointType() == TBase::GetPointType() )	return FALSE;
		return TBase::SetAttrib( nType, nIndex, Value );
	}

	virtual BOOL ChangeValue( int nType, int nIndex, _variant_t Value, bool bPersent )
	{
		if( GetPointType() == nType )
		{
			return ChangeLocalValue( nIndex, Value, bPersent );
		}
		if( GetPointType() == TBase::GetPointType() )	return FALSE;
		return TBase::ChangeValue( nType, nIndex, Value, bPersent );
	}

	BOOL SetLocalAttribListener( int nIndex, AttribValueListener* pListener )
	{
		ASSERT_MSG( nIndex >= 0 && nIndex < nAttribCount, _T("属性值超出范围") );
		if( nIndex < 0 || nIndex >= nAttribCount )	return FALSE;
		_attrib[nIndex].SetListener( pListener );
		return TRUE;
	}

	// 取类的本地属性
	BOOL GetLocalAttrib( int nIndex, _variant_t& Value )const
	{
		ASSERT_MSG( nIndex >= 0 && nIndex < nAttribCount, _T("属性值超出范围") );
		if( nIndex < 0 || nIndex >= nAttribCount )	return FALSE;
		Value = _attrib[nIndex].GetValue();
		return TRUE;
	}

	// 设置本地属性
	BOOL SetLocalAttrib( int nIndex, const _variant_t& Value )
	{
		ASSERT_MSG( nIndex >= 0 && nIndex < nAttribCount, _T("属性值超出范围") );
		if( nIndex < 0 || nIndex >= nAttribCount )	return FALSE;
		_attrib[nIndex].SetValue( Value );
		return TRUE;
	}

	BOOL ChangeLocalValue( int nIndex, const _variant_t& Value, bool bPersent )
	{
		ASSERT_MSG( nIndex >= 0 && nIndex < nAttribCount, _T("属性值超出范围") );
		if( nIndex < 0 || nIndex >= nAttribCount )	return FALSE;
		_attrib[nIndex].ChangeValue( bPersent, Value );
		return TRUE;
	}

	// 字符串属性表
	virtual LPCTSTR GetStrAttrib( int nType, LPCTSTR szName )
	{
		if( GetPointType() == nType )
		{
			CStrAttribMap::const_iterator citer = _strAttrib.find( szName );
			if( citer != _strAttrib.end() )
			{
				const _string& str = citer->second;	return str.c_str();
			}
		}
		if( GetPointType() == TBase::GetPointType() )	return NULL;
		return TBase::GetStrAttrib( nType, szName );
	}

	virtual BOOL SetStrAttrib( int nType, LPCTSTR szName, LPCTSTR strValue )
	{
		if( strValue == NULL || szName == NULL )	return FALSE;
		if( GetPointType() == nType )
		{
			_strAttrib[szName] = strValue;	return TRUE;
		}
		if( GetPointType() == TBase::GetPointType() )	return FALSE;
		return TBase::SetStrAttrib( nType, szName, strValue );
	}

	// 缓冲区属性表
	virtual LPVOID GetBufAttrib( int nType, LPCTSTR szName )
	{
		if( GetPointType() == nType )
		{
			CBufAttribMap::const_iterator citer = _bufAttrib.find( szName );
			if( citer != _bufAttrib.end() )
			{
				LPVOID p = citer->second;	return p;
			}
			return NULL;
		}
		if( GetPointType() == TBase::GetPointType() )	return NULL;
		return TBase::GetBufAttrib( nType, szName );
	}

	virtual BOOL SetBufAttrib( int nType, LPCTSTR szName, LPVOID lpData )
	{
		if( GetPointType() == nType )
		{
			_bufAttrib[szName] = lpData;
			return TRUE;
		}
		if( GetPointType() == TBase::GetPointType() )	return NULL;
		return TBase::SetBufAttrib( nType, szName, lpData );
	}

};

/************************************************************************/
/* CXObject 类，所有带有对象属性的基础类。这里定义了一组属性系统的接口，并提
/* 供了底层实现。
/* 同时，CXObject类实现了自动列表，当对象生成后，将自己插入对象列表中，并初始化
/* 当前ID
/************************************************************************/
/*	:	public XObjTypeT< CXObjectBase, EN_OBJECT, 1 > */
typedef bool (*EnumChildCallback)( int nID, CXObject* pObj, LPVOID pParam );
class CORE_API CXObject
{
protected:
	CXObject();
	virtual	~CXObject();

private:
	int m_nID;
	int	m_nParentID;	// 父物体ID

	typedef std::list< int >	CChildList;

	CChildList	m_ChildList;	// 子物体列表
	_string		m_strName;		// 对象名字

public:
	void			SetName( LPCTSTR lpszName )	{ m_strName = lpszName;			}
	LPCTSTR			GetName()const				{ return m_strName.c_str();		}

	const	int		GetObjID()const				{ return m_nID;					}
	static	int		GetPointType()				{ return TypeObjectbase;		}
	virtual int		GetType()const				{ return TypeObjectbase;		}
	virtual BOOL	IsType( DWORD dwType )const	{ return TypeObjectbase == dwType;}
	static	BOOL	IsTypeOf( DWORD dwType )	{ return TypeObjectbase == dwType;}

	BOOL	IsKindOf( DWORD dwType )			{ if( this == NULL ) return FALSE;else return IsType( dwType ); }

	virtual BOOL	SetAttribListener( int nType, int nIndex, AttribValueListener* pListener )	{ return FALSE; }
	virtual BOOL	GetAttrib( int nType, int nIndex, _variant_t& Value )const					{ return FALSE; }
	virtual BOOL	SetAttrib( int nType, int nIndex, _variant_t Value )						{ return FALSE; }
	virtual BOOL	ChangeValue( int nType, int nIndex, _variant_t Value, bool bPersent )		{ return FALSE; }

	virtual LPCTSTR	GetStrAttrib( int nType, LPCTSTR szName )const				{ return NULL; }
	virtual BOOL	SetStrAttrib( int nType, LPCTSTR szName, LPCTSTR strValue )	{ return FALSE;}

	virtual LPVOID	GetBufAttrib( int nType, LPCTSTR szName )const				{ return NULL; }
	virtual BOOL	SetBufAttrib( int nType, LPCTSTR szName, LPVOID lpData )		{ return FALSE; }

	/************************************************************************/
	/* 层级关系函数
	/************************************************************************/
	// 设置父对象
	void	SetParent( int nID ) { m_nParentID = nID; }
	// 得到父对象ID
	int		GetParent()const { return m_nParentID; }
	// 添加子对象ID
	void	AddChild( int nID );
	// 删除子对象ID
	void	RemoveChild( int nID, bool bRelease = false );
	// 枚举子对象
	void	EnumChild( EnumChildCallback CallbackFunc, LPVOID pParam = NULL );
	/************************************************************************/
	// 销毁对象
	bool	Destroy();
	// 销毁所有子对象
	void	DestroyAllChild();
protected:
	/************************************************************************/
	/* 设置的一些事件响应虚函数。
	/************************************************************************/
	virtual bool OnParentChange( int nOldID, int nNewID ){ return TRUE; }
	virtual bool OnAddChild( int nID ){ return TRUE; }
	virtual bool OnRemoveChild( int nID, bool bRelease ){ return TRUE; }
	virtual bool OnDestroy(){ return TRUE; }
	// 属性变更时调用，用于通知属性变更
	// nType	:	属性的类型层次
	// nIndex	:	属性的索引
	// nNewValue:	属性的值
	// return	:	-ture 允许更改	-false 阻止更改
	// 暂时无用
	//virtual bool OnAttribChange( int nType, int nIndex, int nNewValue ){ return true; }
};

/************************************************************************/
/* 一个属性系统的演示，通过模版派生，可以很容易的继承属性系统，避免了重复代码
/* 模版参数指定了派生基类， 类型值，和整型属性个数
/************************************************************************/
//class CXItem	:	public XObjTypeT< CXObject, TypeItem, 20 >
//{
//public:
//	CXItem(){};
//	~CXItem(){};
//};

#endif //_XOBJECT_