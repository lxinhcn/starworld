#include "SLB/LuaTableIterator.h"

namespace SLB
{
	LuaTableIterator::LuaTableIterator( const LuaObject& obj )
	: m_state( obj.m_state )
	{
		obj.push();
		lua_pushnil(obj.m_state);
	}

	LuaTableIterator::~LuaTableIterator(void)
	{
		lua_pop(m_state,1);
	}

	LuaTableIterator::operator bool()const
	{
		bool b = lua_next(m_state, -2) != 0;
		lua_pushvalue(m_state,-2);

		return b;
	}

	void LuaTableIterator::next()
	{
		lua_pop(m_state,2);
	}
}