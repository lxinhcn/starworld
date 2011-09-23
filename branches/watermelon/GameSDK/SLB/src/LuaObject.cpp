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

	int LuaObject::errorHandler(lua_State *L)
	{
		std::ostringstream out; // Use lua pushfstring and so on...
		lua_Debug debug;

		out << "SLB Exception: "
			<< std::endl << "-------------------------------------------------------"
			<< std::endl;
		out << "Lua Error:" << std::endl << "\t" 
			<<  lua_tostring(L, -1) << std::endl
			<< "Traceback:" << std::endl;
		for ( int level = 0; lua_getstack(L, level, &debug ); level++)
		{
			if (lua_getinfo(L, "Sln", &debug) )
			{
				//TODO use debug.name and debug.namewhat
				//make this more friendly
				out << "\t [ " << level << " (" << debug.what << ") ] ";
				if (debug.currentline > 0 )
				{
					out << debug.short_src << ":" << debug.currentline; 
					if (debug.name)
						out << " @ " << debug.name << "(" << debug.namewhat << ")";
				}
				out << std::endl;
			}
			else
			{
				out << "[ERROR using Lua DEBUG INTERFACE]" << std::endl;
			}
		}

		lua_pushstring(L, out.str().c_str()) ;
		return 1;
	}

	void LuaObject::execute(int numArgs, int numOutput, int top)
	{
		int base = lua_gettop(m_state) - numArgs;
		lua_pushcfunction(m_state, LuaObject::errorHandler);
		lua_insert(m_state, base);

		if(lua_pcall(m_state, numArgs, numOutput, base)) 
		{
			std::runtime_error exception( lua_tostring(m_state, -1) );
			lua_remove(m_state, base);
			lua_settop(m_state,top); // TODO: Remove this.
			throw exception;
		}
		lua_remove(m_state, base);
	}

}
