#pragma once
#include "lua.hpp"
#include "SPP.hpp"
#include <sstream>

struct lua_State;
namespace SLB
{
	class LuaCallBase;
	class LuaObject
	{
		friend class LuaTableIterator;
	public:
		LuaObject();
		LuaObject( lua_State* L, int ref );
		LuaObject( const LuaObject& obj );
		~LuaObject(void);

		LuaObject& operator=( LuaObject& obj );
		void push()const;

		bool		toboolean()const;
		const char* tostring()const;
		ptrdiff_t 	tointeger()const;
		double		tonumber()const;

		operator bool(){ return toboolean(); }
		operator const char*()const{ return tostring(); }
		operator long()const{ return (long)tointeger(); }
		operator int()const{ return (int)tointeger(); }
		operator unsigned long()const{ return (unsigned long)tointeger(); }
		operator unsigned int()const{ return (unsigned int)tointeger(); }

		bool		istable()const;
		bool		isfunction()const;
		bool		isboolean()const;
		bool		isstring()const;
		bool		isnumber()const;
		bool		isnil()const;
		bool		isvalid()const;

		//---------------------------------------------------------------------//
		// Description	: 设置对象
		// return	: none
		//---------------------------------------------------------------------//
		template< class V, class K >
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
		// Description	: 获取对象
		// return	: 值
		//---------------------------------------------------------------------//
		template< class V, class K >
		V get( K key, bool raw = false )
		{
			int top = lua_gettop( m_state );

			lua_getref( m_state, m_luaobject );			// t
			Private::Type< K >::push( m_state, key );	// t k
			raw?lua_rawget( m_state, -2 ):lua_gettable( m_state, -2 );	// t v

			const V val = Private::Type< V >::get( m_state, -1 );
			lua_settop( m_state, top );

			return val;
		}

	protected:
		static int errorHandler(lua_State *L);
		void execute(int numArgs, int numOutput, int top);

	public:
		#define SLB_ARG(N) T##N arg_##N, 
		#define SLB_PUSH_ARGS(N) SLB::push<T##N>(m_state, arg_##N );

		#define SLB_REPEAT(N) \
		template< class R SPP_COMMA_IF(N) SPP_ENUM_D(N, class T) >\
		R call( SPP_REPEAT( N, SLB_ARG) char dummyARG = 0 )\
		{\
			int top = lua_gettop(m_state);\
			lua_rawgeti(m_state, LUA_REGISTRYINDEX,m_luaobject);\
			SPP_REPEAT( N, SLB_PUSH_ARGS );\
			execute(N, 1, top);\
			R result = SLB::get<R>(m_state, -1);\
			lua_settop(m_state,top);\
			return result;\
		}\

		SPP_MAIN_REPEAT_Z(MAX,SLB_REPEAT)
		#undef SLB_REPEAT

		#undef SLB_ARG
		#undef SLB_PUSH_ARGS
	protected:
		virtual void pushImplementation(lua_State *);

	private:
		friend class LuaCallBase;
		lua_State*	m_state;
		int			m_luaobject;
	};
}
