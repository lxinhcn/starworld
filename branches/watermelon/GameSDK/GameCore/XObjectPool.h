#pragma once
#include "boost/type_traits.hpp"
namespace XGC
{
	class CXObject;

	//----------------------------------------------------------------------//

	/************************************************************************/
	/* CXObjectPool 类，实现了一个静态的指针列表，用来通过ID的方式转化对象指针。
	/************************************************************************/
	#define ObjectPool	CXObjectPool::GetInstance()
	class CORE_API CXObjectPool
	{
		friend class CXObject;
	protected:
		CXObjectPool( size_t nLen );
		~CXObjectPool();

		xObject	AddObject( CXObject* pObj );
		void	DelObject( xObject nID );

		struct object_handle
		{
			unsigned int	position:24;	// 位置位域
			unsigned int	round:7;		// 轮询位域
			unsigned int	flag:1;			// 对象位置标志, 1 为服务器使用, 0 为客户端使用.服务器上的对象,最高位永不会使用0;
		};

		CXObject**	m_pObjectPool;
		size_t		m_nLen;
		size_t		m_nCurID;
		size_t		m_nRound;
		size_t		m_nCount;
	public:
		static CXObjectPool& GetInstance();
		inline bool IsValidObject( xObject id );

		//--------------------------------------------------------//
		//	created:	24:11:2009   15:02
		//	filename: 	d:\Developed\StarGame\GameSDK\GameCore\Include\XObjectList.h
		//	author:		Albert.xu
		//
		//	purpose:	通过ID获取对象
		//--------------------------------------------------------//
		inline CXObject* GetObject( xObject id )
		{ 
			object_handle &h = (object_handle&)id;
			return IsValidObject( id )?m_pObjectPool[h.position]:NULL; 
		}

		//--------------------------------------------------------//
		//	created:	24:11:2009   15:03
		//	filename: 	d:\Developed\StarGame\GameSDK\GameCore\Include\XObjectList.h
		//	author:		Albert.xu
		//
		//	purpose:	通过id获取对象，同时检查类型
		//--------------------------------------------------------//
		inline CXObject* GetObject( xObject id, xType type );

		//---------------------------------------------------//
		// [12/11/2010 Albert]
		// Description:	通过id获取对象 
		//---------------------------------------------------//
		template< typename ObjectType >
		ObjectType* GetObject( xObject id )
		{
			CXObject* pObject = GetObject( id );
			if( pObject && pObject->IsTypeOf( ObjectType::GetPointType() ) )
				return static_cast< ObjectType* >( pObject );
			else
				return xgc_nullptr;
		}
	};
}