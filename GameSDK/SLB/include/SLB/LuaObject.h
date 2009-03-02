#pragma once
#include "lua.hpp"

struct lua_State;
namespace SLB
{
	class LuaObject
	{
	public:
		LuaObject( lua_State* L, int ref );
		LuaObject( LuaObject& obj );
		~LuaObject(void);

		LuaObject& operator=( LuaObject& obj );
		void push( lua_State *L )const;

		//---------------------------------------------------------------------//
		// describe	: 设置对象
		// return	: none
		//---------------------------------------------------------------------//
		template< class K, class V >
		void set( K key, V value, bool raw = false )
		{
			int top = lua_gettop( m_state );

			lua_getref( m_state, m_luaobject );
			// lua_pushinteger( m_state, idx );
			Private::Type< K >::push( m_state, key );
			// lua_pushinteger( m_state, value );
			Private::Type< V >::push( m_state, value );
			raw?lua_rawset( m_state, -3 ):lua_settable( m_state, -3 );

			lua_settop( m_state, top );
		}

		//---------------------------------------------------------------------//
		// describe	: 获取对象
		// return	: 值
		//---------------------------------------------------------------------//
		template< class K, class V >
		V get( K key, bool raw = false )
		{
			int top = lua_gettop( m_state );

			lua_getref( m_state, m_luaobject );			// t
			Private::Type< K >::push( m_state, key );	// t k
			raw?lua_rawget( m_state, -2 ):lua_gettable( m_state, -2 );	// t v

			const V& val = Private::Type< V >::get( m_state, -1 );
			lua_settop( m_state, top );

			return val;
		}

	protected:
		virtual void pushImplementation(lua_State *);

	private:
		lua_State*	m_state;
		int			m_luaobject;
	};
}
