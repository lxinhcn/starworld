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
		if( obj.isvalid() )
		{
			lua_getref( m_state, obj.m_luaobject );
			m_luaobject = luaL_ref( m_state, LUA_REGISTRYINDEX );
		}
	}

	LuaObject::~LuaObject(void)
	{
		// 解除引用
		if( isvalid() )
		{
			lua_unref( m_state, m_luaobject );
		}
	}

	LuaObject& LuaObject::operator=( LuaObject& obj )
	{
		if( obj.isvalid() )
		{
			obj.push();
			int luaobject = luaL_ref( obj.m_state, LUA_REGISTRYINDEX );

			if( isvalid() )
			{
				lua_unref( m_state, m_luaobject );
			}
			m_state		= obj.m_state;
			m_luaobject = luaobject;
		}
		return *this;
	}

	void LuaObject::push()const
	{
		if( isvalid() )
		{
			lua_getref( m_state, m_luaobject );
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

	ptrdiff_t LuaObject::tointeger()const
	{
		lua_getref( m_state, m_luaobject );			// t
		return lua_tointeger( m_state, -1 );
	}

	double LuaObject::tonumber()const
	{
		lua_getref( m_state, m_luaobject );			// t
		return (long)lua_tonumber( m_state, -1 );
	}

	bool LuaObject::isfunction()const
	{
		lua_getref( m_state, m_luaobject );			// t
		bool ret = lua_isfunction( m_state, -1 ) != 0;
		lua_pop( m_state, 1 );
		return ret;
	}

	bool LuaObject::isboolean()const
	{
		lua_getref( m_state, m_luaobject );			// t
		//const char* tn = lua_typename( m_state, -1 );
		bool ret = lua_isboolean( m_state, -1 ) != 0;
		lua_pop( m_state, 1 );
		return ret;
	}

	bool LuaObject::isstring()const
	{
		lua_getref( m_state, m_luaobject );			// t
		//const char* tn = lua_typename( m_state, -1 );
		bool ret = lua_isstring( m_state, -1 ) != 0;
		lua_pop( m_state, 1 );
		return ret;
}

	bool LuaObject::isnumber()const
	{
		lua_getref( m_state, m_luaobject );			// t
		//const char* tn = lua_typename( m_state, -1 );
		bool ret = lua_isnumber( m_state, -1 ) != 0;
		lua_pop( m_state, 1 );
		return ret;
	}

	bool LuaObject::isnil()const
	{
		lua_getref( m_state, m_luaobject );			// t
		//const char* tn = lua_typename( m_state, -1 );
		bool ret = lua_isnil( m_state, -1 ) != 0;
		lua_pop( m_state, 1 );
		return ret;
	}

	bool LuaObject::istable()const
	{
		lua_getref( m_state, m_luaobject );			// t
		//const char* tn = lua_typename( m_state, -1 );
		bool ret = lua_istable( m_state, -1 ) != 0;
		lua_pop( m_state, 1 );
		return ret;
	}

	bool LuaObject::isvalid()const
	{
		return m_state && m_luaobject!=LUA_REFNIL;
	}

}
