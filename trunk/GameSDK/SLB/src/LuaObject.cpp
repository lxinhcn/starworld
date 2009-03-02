#include "SLB\LuaObject.h"

namespace SLB
{
	LuaObject::LuaObject( lua_State* state, int ref )
		: m_state( state )
		, m_luaobject( ref )
	{
	}

	LuaObject::LuaObject( LuaObject& obj )
		: m_state( obj.m_state )
	{
		// 引用
		lua_getref( m_state, obj.m_luaobject );
		m_luaobject = lua_ref( m_state, true );
	}

	LuaObject::~LuaObject(void)
	{
		// 解除引用
		if( m_luaobject != LUA_REFNIL )
		{
			lua_unref( m_state, m_luaobject );
		}
	}

	void LuaObject::push( lua_State *L )const
	{
		// assert( L == m_state );
		lua_getref( m_state, m_luaobject );
		lua_pushvalue( m_state, -1 );
	}

	void LuaObject::pushImplementation(lua_State *)
	{

	}
}
