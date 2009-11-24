#pragma once
#include "XObject.h"
namespace XGC
{
	class CXObject;

	//----------------------------------------------------------------------//

	/************************************************************************/
	/* CXObjectPool 类，实现了一个静态的指针列表，用来通过ID的方式转化对象指针。
	/************************************************************************/
	class CORE_API CXObjectPool
	{
		friend class CXObject;
	protected:
		CXObjectPool( size_t nLen );
		~CXObjectPool();

		identifier	AddObj( CXObject* pObj );
		void		DelObj( identifier nID );

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
		inline bool IsValidID( identifier id );

		//--------------------------------------------------------//
		//	created:	24:11:2009   15:02
		//	filename: 	d:\Developed\StarGame\GameSDK\GameCore\Include\XObjectList.h
		//	author:		Albert.xu
		//
		//	purpose:	通过ID获取对象
		//--------------------------------------------------------//
		inline CXObject* GetObj( identifier id )
		{ 
			object_handle &h = (object_handle&)id;
			return IsValidID( id )?m_pObjectPool[h.position]:NULL; 
		}

		//--------------------------------------------------------//
		//	created:	24:11:2009   15:03
		//	filename: 	d:\Developed\StarGame\GameSDK\GameCore\Include\XObjectList.h
		//	author:		Albert.xu
		//
		//	purpose:	通过id获取对象，同时检查类型
		//--------------------------------------------------------//
		inline CXObject* GetObj( identifier id, guid type );

		inline CXObject* operator []( identifier id )
		{ 
			object_handle &h = (object_handle&)id;
			return m_pObjectPool[h.position]; 
		}
	};
}