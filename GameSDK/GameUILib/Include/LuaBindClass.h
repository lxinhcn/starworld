#pragma once
#ifndef _LUABINDCLASS_H
#define _LUABINDCLASS_H
#include <loki\Singleton.h>
#include "SLB\LuaObject.h"

#define LUA_CATCH(...)	catch( std::runtime_error& err ){ OutputDebugStringA( err.what() ); __VA_ARGS__; }

class LuaDebuger;
namespace UILib
{
	class XUI_Wnd;
	class LuaBindClass	:	public SLB::Script
	{
		friend struct Loki::CreateStatic< LuaBindClass >;
		friend class CGuiSystem;
	public:
		operator lua_State*(){ return getState(); }
		bool SetupDebuger();

	private:
		LuaBindClass(void);
		~LuaBindClass(void);

		void Initialize();

		LuaDebuger	*m_pLuaDebuger; 
	};
	typedef Loki::SingletonHolder< LuaBindClass, Loki::CreateStatic >	Lua;

	template< class T >
	void show_members( const T* item, int indent )
	{
		ClassInfo* c = Manager::getInstance().getClass( typeid( *item ) );
		if( c )
		{
			std::list< const char* > l;
			c->getMembers( l );
			std::list< const char* >::iterator i = l.begin();
			while( i != l.end() )
			{
				try
				{
					_putch( '\n' );
					for( int n = 0; n < indent; ++n ) printf( "  " );
					printf( "|->");
					SLB::LuaCall< const char*( const T*, const char*, int ) > op( Lua::Instance(), "class_attrib" );
					const char* attrib = op( item, *i, indent );
					if( attrib )
					{
						printf( " %s = %s", *i, attrib );
					}
				}
				catch( std::runtime_error& err )
				{
					OutputDebugStringA( err.what() );
				}
				++i;
			}
		}
	}

	template< class T >
	bool save_file( const T* pElement, TiXmlElement* pNode )
	{
		ClassInfo* c = Manager::getInstance().getClass( typeid( *pElement ) );
		if( c )
		{
			LuaCall< void( const T*, const char*, TiXmlElement* ) > op( Lua::Instance(), "save" );
			std::list< const char* > l;
			c->getMembers( l );
			std::list< const char* >::iterator i = l.begin();
			while( i != l.end() )
			{
				try
				{
					op( pElement, *i, pNode );
				}
				catch( std::runtime_error& err )
				{
					OutputDebugStringA( err.what() );
					return false;
				}
				++i;
			}
		}
		return true;
	}

	template< class T >
	bool load_file( const T* pElement, TiXmlElement* pNode )
	{
		ClassInfo* c = Manager::getInstance().getClass( typeid( *pElement ) );
		if( c )
		{
			LuaCall< void( const T*, const char*, TiXmlElement* ) > op( Lua::Instance(), "load" );
			std::list< const char* > l;
			c->getMembers( l );
			std::list< const char* >::iterator i = l.begin();
			while( i != l.end() )
			{
				try
				{
					TiXmlElement* pChild = pNode->FirstChildElement( *i );
					if( pChild )
					{
						op( pElement, *i, pChild );
					}
				}
				catch( std::runtime_error& err )
				{
					OutputDebugStringA( err.what() );
					return false;
				}
				++i;
			}
		}
		return true;
	}

	unsigned int LuaSetTimer( SLB::LuaObject& function, unsigned short repeat, unsigned short timer );
}
#endif //_LUABINDCLASS_H