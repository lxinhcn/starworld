#include "SLB\LuaObject.h"
#include <sstream>
#include <SLB/Debug.hpp>

namespace SLB
{
	LuaObject::LuaObject()
		: m_state( NULL )
		, m_luaobject( LUA_REFNIL )
	{
	}

	LuaObject::LuaObject( lua_State* state, int ref )
		: m_state( state )
		, m_luaobject( ref )
	{
	}

	LuaObject::LuaObject( const LuaObject& obj )
		: m_state( obj.m_state )
	{
		// 引用
		lua_getref( m_state, obj.m_luaobject );
		m_luaobject = luaL_ref( m_state, LUA_REGISTRYINDEX );
	}

	LuaObject::~LuaObject(void)
	{
		// 解除引用
		if( m_luaobject != LUA_REFNIL )
		{
			lua_unref( m_state, m_luaobject );
		}
	}

	LuaObject& LuaObject::operator=( LuaObject& obj )
	{
		int luaobject = luaL_ref( obj.m_state, LUA_REGISTRYINDEX );

		if( m_state && m_luaobject != LUA_REFNIL )
		{
			lua_unref( m_state, m_luaobject );
		}
		m_state		= obj.m_state;
		m_luaobject = luaobject;
		return *this;
	}

	void LuaObject::push( lua_State *L )const
	{
		if( m_state && m_state == L )
		{
			lua_getref( m_state, m_luaobject );
			lua_pushvalue( m_state, -1 );
		}
		else
		{
			lua_pushnil( L );
		}
	}

	void LuaObject::pushImplementation(lua_State *)
	{
	}

	bool LuaObject::toboolean()const
	{
		lua_getref( m_state, m_luaobject );			// t
		return lua_toboolean( m_state, -1 ) != 0;
	}

	const char* LuaObject::tostring()const
	{
		lua_getref( m_state, m_luaobject );			// t
		return lua_tostring( m_state, -1 );
	}

	long LuaObject::tointeger()const
	{
		lua_getref( m_state, m_luaobject );			// t
		return (long)lua_tointeger( m_state, -1 );
	}

	double LuaObject::tonumber()const
	{
		lua_getref( m_state, m_luaobject );			// t
		return (long)lua_tonumber( m_state, -1 );
	}

	bool LuaObject::isboolean()const
	{
		lua_getref( m_state, m_luaobject );			// t
		return lua_isboolean( m_state, -1 );
	}

	bool LuaObject::isstring()const
	{
		lua_getref( m_state, m_luaobject );			// t
		return lua_isstring( m_state, -1 ) != 0;
	}

	bool LuaObject::isnumber()const
	{
		lua_getref( m_state, m_luaobject );			// t
		return lua_isnumber( m_state, -1 ) != 0;
	}

	bool LuaObject::isnil()const
	{
		lua_getref( m_state, m_luaobject );			// t
		return lua_isnil( m_state, -1 );
	}

	bool LuaObject::isvalid()const
	{
		return m_state && m_luaobject!=LUA_REFNIL;
	}
}
