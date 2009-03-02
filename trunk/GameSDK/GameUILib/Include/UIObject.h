#ifndef _UIOBJECT_
#define _UIOBJECT_
/************************************************************************/
/* 列表系统，属性系统，以及对象基础类的实现                               */
/************************************************************************/
#define INVALID_OBJID -1
#define INVALID_ATTRIBID -1
#define OBJ_MAX	65535

#define		TypeObjectbase					0

#define		TypeUI							0x30000000
#define			TypeElement					0x31000000
#define				TypeButton				0x31100000
#define				TypeEditBox				0x31200000
#define				TypeScroll				0x31300000
#define				TypeProgress			0x31400000
#define				TypePicture				0x31500000
#define					TypeMultiPicture	0x31510000
#define					TypeRenderTarget	0x31520000
#define					TypeStatic			0x31530000
#define					TypeWindow			0x31540000
#define						TypeDialog		0x31551000
#define					TypeListbox			0x31560000

#include <Windows.h>

/************************************************************************/
/* CXObjectT 模版实现了动态类型系统，
/************************************************************************/
template < typename TBase, int TYPE >
class UIObjTypeT	:	public TBase
{
protected:
	UIObjTypeT()
	{
		//ASSERT( IsTypeOf( TypeObjectbase ), "错误的继承类型。" );
	}

public:
	static	int		GetPointType()					{ return TYPE; }
	virtual int		GetType()const					{ return TYPE; }
	virtual BOOL	IsKindOf( DWORD dwType )const	{ if( this == NULL ) return FALSE;if( GetPointType() == dwType ) return TRUE; else return TBase::IsKindOf( dwType );}
	static	BOOL	IsTypeOf( DWORD dwType )		{ if( GetPointType() == dwType ) return TRUE; else return TBase::IsTypeOf( dwType );}
};

/*	:	public XObjTypeT< CXObjectBase, EN_OBJECT, 1 > */
class CUIObject
{
protected:
	CUIObject();
	virtual ~CUIObject();

public:
	static	int		GetPointType()					{ return TypeObjectbase;		}
	virtual int		GetType()const					{ return TypeObjectbase;		}
	virtual BOOL	IsKindOf( DWORD dwType )const	{ return TypeObjectbase == dwType;}
	static	BOOL	IsTypeOf( DWORD dwType )		{ return TypeObjectbase == dwType;}
};
#endif //_UIOBJECT_