#include "StdAfx.h"
#include "XUtilityFunctor.h"
#include "XActorBase.h"
#include "MoveState.h"
#include "IdleState.h"
#include "AttackState.h"
#include "GameMap.h"

//---------------------------------------------------//
// [11/18/2010 Albert]
// Description:	判断是否为有效的对象 
//---------------------------------------------------//
bool CheckObjectType( xObject hObject, xType TypeId )
{
	CXObject* pObject = ObjectPool.GetObject( hObject );
	if( pObject )
	{
		return pObject->IsTypeOf( TypeId );
	}

	return false;
}

CAttribute GetAttribute( xObject hObject, xType gType )
{
	CXObject* pObject = ObjectPool.GetObject( hObject );
	ASSERT_POINTER( pObject );
	return pObject->GetAttribute( gType, true );
}

//---------------------------------------------------//
// [11/18/2010 Albert]
// Description:	判断是否为有效的对象 
//---------------------------------------------------//
bool IsValidObject( xObject hObject )
{
	return ObjectPool.IsValidObject( hObject );
}

//---------------------------------------------------//
// [11/18/2010 Albert]
// Description:	获取父对象 
//---------------------------------------------------//
xObject GetParentObject( xObject hObject )
{
	CXObject* pObject = ObjectPool.GetObject( hObject );
	ASSERT_POINTER( pObject );
	return pObject->GetParent();
}

//---------------------------------------------------//
// [11/18/2010 Albert]
// Description:	添加子对象 
//---------------------------------------------------//
bool AddChild( xObject hParent, xObject hChild )
{
	CXObject* pObject = ObjectPool.GetObject( hParent );
	ASSERT_POINTER( pObject );
	return pObject->AddChild( hChild );
}

//---------------------------------------------------//
// [11/18/2010 Albert]
// Description:	删除子对象 
//---------------------------------------------------//
void RemoveChild( xObject hParent, xObject hChild )
{
	CXObject* pObject = ObjectPool.GetObject( hParent );
	ASSERT_POINTER( pObject );
	pObject->RemoveChild( hChild );
}

//---------------------------------------------------//
// [11/18/2010 Albert]
// Description:	 查询子对象
//---------------------------------------------------//
bool QueryChild( xObject hParent, xObject hChild )
{
	CXObject* pObject = ObjectPool.GetObject( hParent );
	ASSERT_POINTER( pObject );
	return pObject->QueryChild( hChild );
}

//---------------------------------------------------//
// [11/18/2010 Albert]
// Description:	销毁对象 
//---------------------------------------------------//
void DestroyObject( xObject hObject )
{
	CXObject* pObject = ObjectPool.GetObject( hObject );
	ASSERT_POINTER( pObject );
	pObject->Destroy();
}

//---------------------------------------------------//
// [12/15/2010 Albert]
// Description:	获取字符串属性
//---------------------------------------------------//
_lpctstr GetStrAttrib( xObject hObject, _lpctstr lpszKey )
{
	CXObject* pObject = ObjectPool.GetObject( hObject );
	ASSERT_POINTER( pObject );

	return pObject->GetStrAttrib( lpszKey );
}

//---------------------------------------------------//
// [12/15/2010 Albert]
// Description:	设置字符串属性 
//---------------------------------------------------//
void SetStrAttrib( xObject hObject, _lpctstr lpszKey, _lpctstr lpszValue )
{
	CXObject* pObject = ObjectPool.GetObject( hObject );
	ASSERT_POINTER( pObject );

	pObject->SetStrAttrib( lpszKey, lpszValue );
}

//---------------------------------------------------//
// [12/15/2010 Albert]
// Description:	获取整型属性
//---------------------------------------------------//
int GetIntegerAttrib( xObject hObject, _lpctstr lpszKey, int nDefault )
{
	CXObject* pObject = ObjectPool.GetObject( hObject );
	ASSERT_POINTER( pObject );

	CXVariant v;
	if( pObject->GetValueAttrib( lpszKey, v ) )
		return v;
	else
		return nDefault;
}

//---------------------------------------------------//
// [12/15/2010 Albert]
// Description:	设置整型属性 
//---------------------------------------------------//
void SetIntegerAttrib( xObject hObject, _lpctstr lpszKey, int nValue )
{
	CXObject* pObject = ObjectPool.GetObject( hObject );
	ASSERT_POINTER( pObject );

	pObject->SetValueAttrib( lpszKey, nValue );
}

//---------------------------------------------------//
// [12/15/2010 Albert]
// Description:	获取整型属性
//---------------------------------------------------//
float GetFloatAttrib( xObject hObject, _lpctstr lpszKey, float fDefault )
{
	CXObject* pObject = ObjectPool.GetObject( hObject );
	ASSERT_POINTER( pObject );

	CXVariant v;
	if( pObject->GetValueAttrib( lpszKey, v ) )
		return v;
	else
		return fDefault;
}

//---------------------------------------------------//
// [12/15/2010 Albert]
// Description:	设置整型属性 
//---------------------------------------------------//
void SetFloatAttrib( xObject hObject, _lpctstr lpszKey, float fValue )
{
	CXObject* pObject = ObjectPool.GetObject( hObject );
	ASSERT_POINTER( pObject );

	pObject->SetValueAttrib( lpszKey, fValue );
}

//---------------------------------------------------//
// [12/14/2010 Albert]
// Description:	设置触发器Reciver 
//---------------------------------------------------//
void SetTriggerReciver( xObject hObject, int nEvent, void (*Reciver)( xObject, xObject, long_ptr, int_ptr ) )
{
	CGameObject* pObject = ObjectPool.GetObject< CGameObject >( hObject );
	ASSERT_POINTER( pObject );
	pObject->RegisteTrigger( nEvent, Reciver );
}

//---------------------------------------------------//
// [12/23/2010 Albert]
// Description:	触发事件 
//---------------------------------------------------//
void TriggerEvent( xType nType, xObject hSource, xObject hTarget, long_ptr lParam, int_ptr wParam )
{
	CGameObject* pObject = ObjectPool.GetObject< CGameObject >( hTarget );
	if( pObject )
	{
		pObject->Trigger( nType, hSource, lParam, wParam );
	}
}

//---------------------------------------------------//
// [1/7/2011 Albert]
// Description:	设置角色状态 
//---------------------------------------------------//
void SetActorStatus( xObject hActor, int eStatus, float fDelayTime, int nMode )
{
	XActorBase* pObject = ObjectPool.GetObject< XActorBase >( hActor );
	if( pObject )
	{
		pObject->SetActorStatus( (XActorBase::ActorStatus)eStatus, fDelayTime, nMode );
	}
}

//---------------------------------------------------//
// [1/13/2011 Albert]
// Description:	获取角色状态 
//---------------------------------------------------//
CORE_API int GetActorStatus( xObject hActor )
{
	XActorBase* pObject = ObjectPool.GetObject< XActorBase >( hActor );
	if( pObject )
	{
		return pObject->GetActorStatus();
	}

	return -1;
}

//---------------------------------------------------//
// [12/1/2010 Albert]
// Description:	获取对象的当前坐标 
//---------------------------------------------------//
const XVector3& GetPosition( xObject hObject )
{
	CGameObject* pObject = ObjectPool.GetObject< CGameObject >( hObject );
	ASSERT_RETURN( pObject, XVector3::ZERO );
	return pObject->GetPosition();
}

//---------------------------------------------------//
// [1/7/2011 Albert]
// Description:	获取当前方向 
//---------------------------------------------------//
const XVector3& GetDirection( xObject hObject )
{
	CGameObject* pObject = ObjectPool.GetObject< CGameObject >( hObject );
	ASSERT_RETURN( pObject, XVector3::ZERO );
	return pObject->GetDirection();
}

//---------------------------------------------------//
// [1/7/2011 Albert]
// Description:	动态移动对象到指定位置
//---------------------------------------------------//
bool MoveObjectTo( xObject hObject, XVector3& vPosition, bool bTestCollision )
{
	CGameObject* pObject = ObjectPool.GetObject< CGameObject >( hObject );
	ASSERT_RETURN( pObject, false );
	CServerMap* pMap = ObjectPool.GetObject< CServerMap >( pObject->GetParent() );
	if( pMap )
		return pMap->DynamicMoveTo( pObject, vPosition, bTestCollision );

	return false;
}

//---------------------------------------------------//
// [11/18/2010 Albert]
// Description:	获取一个范围内的随机数 
//---------------------------------------------------//
int RandomRange( int nMin, int nMax )
{
	if( nMin == nMax )
		return nMin;

	if( nMin > nMax )
		std::swap( nMin, nMax );

	return nMin + rand()%(nMax-nMin);
}

//---------------------------------------------------//
// [12/1/2010 Albert]
// Description:	 获取一个范围内的随机数 
//---------------------------------------------------//
float RandomRange( float fMin, float fMax )
{
	if( fMin > fMax )
		std::swap( fMin, fMax );

	float f = 0.0f;
	
	do 
	{
		f = 1.0f * rand() / rand();
	} while( f > 1.0f );
	
	return fMin + f * (fMax-fMin);
}

//---------------------------------------------------//
// [11/19/2010 Albert]
// Description:	角色受到伤害 
//---------------------------------------------------//
void AttackTarget( xObject hAttacker, xObject hInjured, int nDamage )
{
	CXObject* pObject = ObjectPool.GetObject( hInjured );
	if( pObject && pObject->IsTypeOf( TypeActorBase ) )
	{
		XActorBase* pActorBase = static_cast< XActorBase* >( pObject );
		pActorBase->UnderAttack( hAttacker, nDamage );
	}
}