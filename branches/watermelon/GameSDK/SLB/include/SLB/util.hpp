/*
    SLB - Simple Lua Binder
    Copyright (C) 2007 Jose L. Hidalgo Valiño (PpluX)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	Jose L. Hidalgo (www.pplux.com)
	pplux@pplux.com
*/

#ifndef __SLB_UTIL__
#define __SLB_UTIL__
#include "Class.hpp"

#define L_abs_index(L, i)     ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : \
		                    lua_gettop(L) + (i) + 1)

namespace SLB
{
	template< class T >
	struct luaArray
	{
		luaArray( int size )
			: _array( new T[size] )
			, _size( size )
		{

		}

		~luaArray()
		{
			delete[] _array;
		}

		void Set( size_t idx, const T& v )
		{
			if( idx >= _size )
				return;

			_array[idx] = v;
		}

		T* Get( size_t idx )
		{
			if( idx >= _size )
				return NULL;

			return _array + idx;
		}

		operator const T*()const
		{
			return _array;
		}

		operator T*()
		{
			return _array;
		}

		T* _array;
		size_t _size;
	};

	template< class T >
	void RegisterArray( const char* name )
	{
		Class< luaArray< T > >( name )
			.constructor< size_t >()
			.set( "set", &luaArray< T >::Set )
			.set( "get", &luaArray< T >::Get )
			.object__index( &luaArray< T >::Get )
			.object__newindex( &luaArray< T >::Set )
			;
	}
}

#endif
