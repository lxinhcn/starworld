#pragma once
#ifndef _LUA_TABLE_ITERATOR_H_
#define _LUA_TABLE_ITERATOR_H_
#include "lua.hpp"
#include "LuaObject.h"

namespace SLB
{
	class LuaTableIterator
	{
	public:
		LuaTableIterator( const LuaObject& obj );
		~LuaTableIterator(void);

		operator bool()const;

		void next();

		template< typename K >
		K getKey()
		{
			return SLB::get< K >( m_state, -1 );
		}

		template< typename V >
		V getValue()
		{
			return SLB::get< V >( m_state, -2 );
		}

	private:
		lua_State* m_state;
	};
}

#endif // _LUA_TABLE_ITERATOR_H_