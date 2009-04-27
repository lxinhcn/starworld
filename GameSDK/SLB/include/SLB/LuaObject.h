#pragma once
#include "lua.hpp"
#include "SPP.hpp"

struct lua_State;
namespace SLB
{
	class LuaObject
	{
	public:
		LuaObject();
		LuaObject( lua_State* L, int ref );
		LuaObject( const LuaObject& obj );
		~LuaObject(void);

		LuaObject& operator=( LuaObject& obj );
		void push( lua_State *L )const;

		// 执行lua函数
		void execute(int numArgs, int numOutput, int top);

		static int errorHandler(lua_State *L);

		bool		toboolean()const;
		const char* tostring()const;
		long		tointeger()const;
		double		tonumber()const;

		bool		isboolean()const;
		bool		isstring()const;
		bool		isnumber()const;
		bool		isnil()const;
		bool		isvalid()const;
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

		//template< class R, class T1, class T2 >
		//R call( T1 arg_1, T2 arg_2, char dummy = 0 )
		//{
		//	int top = lua_gettop(m_state);
		//	lua_rawgeti(m_state, LUA_REGISTRYINDEX,m_luaobject);
		//	push<T1>( m_state, arg_1 );
		//	push<T2>( m_state, arg_2 );
		//	execute(2, 1, top);
		//	R result = SLB::get<R>(m_state, -1);
		//	lua_settop(m_state,top);
		//	return result;
		//}

		#define SLB_ARG(N) T##N arg_##N, 
		#define SLB_PUSH_ARGS(N) SLB::push<T##N>(m_state, arg_##N );

		#define SLB_REPEAT(N) \
		template<class R SPP_COMMA_IF(N) SPP_ENUM_D(N, class T)> \
		R call( SPP_REPEAT( N, SLB_ARG) char dummyARG = 0)\
		{ \
			int top = lua_gettop(m_state); \
			lua_rawgeti(m_state, LUA_REGISTRYINDEX,m_luaobject); \
			SPP_REPEAT( N, SLB_PUSH_ARGS ); \
			execute(N, 1, top); \
			R result = SLB::get<R>(m_state, -1); \
			lua_settop(m_state,top); \
			return result; \
		} \

		SPP_MAIN_REPEAT_Z(MAX,SLB_REPEAT)
		#undef SLB_REPEAT
		#undef SLB_ARG
		#undef SLB_PUSH_ARGS

	protected:
		virtual void pushImplementation(lua_State *);

	private:
		lua_State*	m_state;
		int			m_luaobject;
	};
}
