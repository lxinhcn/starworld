#pragma once
#include "Object.hpp"
#include "Export.hpp"
#include "SPP.hpp"
#include "lua.hpp"
#include "Type.hpp"

namespace SLB
{
	template< class C, typename V >
	class SLB_EXPORT setMember : public FuncCall
	{
	private:
		setMember( V C::* offset )
			: m_offset( offset )
		{

		}

		~setMember(void)
		{
		}

		virtual int call(lua_State *L)
		{
			InstanceBase** Instance = (InstanceBase**)lua_touserdata( L, 1 );
			C* p = (C*)( (*Instance)->isConst()?(*Instance)->get_const_ptr():(*Instance)->get_ptr() );
			if( p )
			{
				p->*m_offset = Private::Type< V >::get(L,2);
			}
			return 0;
		}

	public:
		static setMember* create( V C::* offset )
		{
			setMember* m = new setMember< C, V >( offset );
			return m;
		}

	private:
		V	C::* m_offset;
	};

	template< class C, typename V >
	class SLB_EXPORT getMember : public FuncCall
	{
	private:
		getMember( V C::* offset )
			: m_offset( offset )
		{

		}

		~getMember(void)
		{
		}

		virtual int call(lua_State *L)
		{
			int t = lua_type( L, 1 );
			InstanceBase** Instance = (InstanceBase**)lua_touserdata( L, 1 );
			if( (*Instance)->isConst() )
			{
				C* p = (C*)(*Instance)->get_const_ptr();
				if( p )
				{
					Private::Type< V& >::push( L, p->*m_offset );
				}
			}
			else
			{
				C* p = (C*)(*Instance)->get_ptr();
				if( p )
				{
					Private::Type< V& >::push( L, p->*m_offset );
				}
			}
			return 1;
		}

	public:
		static getMember* create( V C::* offset )
		{
			getMember* m = new getMember< C, V >( offset );
			return m;
		}

	private:
		V	C::* m_offset;
	};

	template< class C, typename V >
	class SLB_EXPORT setMember< C, V* > : public FuncCall
	{
	private:
		setMember( V* C::* offset )
			: m_offset( offset )
		{

		}

		~setMember(void)
		{
		}

		virtual int call(lua_State *L)
		{
			InstanceBase** Instance = (InstanceBase**)lua_touserdata( L, 1 );
			C* p = (C*)( (*Instance)->isConst()?(*Instance)->get_const_ptr():(*Instance)->get_ptr() );
			if( p )
			{
				p->*m_offset = Private::Type< V* >::get(L,2);
			}
			return 0;
		}

	public:
		static setMember* create( V* C::* offset )
		{
			setMember* m = new setMember< C, V* >( offset );
			return m;
		}

	private:
		V*	C::* m_offset;
	};

	template< class C, typename V >
	class SLB_EXPORT getMember< C, V* >: public FuncCall
	{
	private:
		getMember( V* C::* offset )
			: m_offset( offset )
		{

		}

		~getMember(void)
		{
		}

		virtual int call(lua_State *L)
		{
			int t = lua_type( L, 1 );
			InstanceBase** Instance = (InstanceBase**)lua_touserdata( L, 1 );
			if( (*Instance)->isConst() )
			{
				C* p = (C*)(*Instance)->get_const_ptr();
				if( p )
				{
					Private::Type< V* >::push( L, p->*m_offset );
				}
			}
			else
			{
				C* p = (C*)(*Instance)->get_ptr();
				if( p )
				{
					Private::Type< V* >::push( L, p->*m_offset );
				}
			}
			return 1;
		}

	public:
		static getMember* create( V* C::* offset )
		{
			getMember* m = new getMember< C, V* >( offset );
			return m;
		}

	private:
		V*		C::* m_offset;
	};

}
