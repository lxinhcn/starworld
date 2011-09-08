#pragma once
#ifndef _LUABINDCLASS_H
#define _LUABINDCLASS_H
#include "singleton.h"
#include "SLB\LuaObject.h"
#include "XUI_System.h"

#define LUA_CATCH(...)	catch( std::runtime_error& err ){ OutputDebugStringA( err.what() ); __VA_ARGS__; }

class LuaDebuger;
namespace XGC
{
	namespace ui
	{
		class XUI_Wnd;
		class XUI_Lua	:	public SLB::Script
		{
			friend struct CreateStatic< XUI_Lua >;
			friend class XUI_System;
		public:
			operator lua_State*(){ return getState(); }
			bool SetupDebuger();

		private:
			XUI_Lua(void);
			~XUI_Lua(void);

			void Initialize();

			LuaDebuger	*m_pLuaDebuger; 
		};
		inline unsigned int GetLongevity( XUI_Lua* ){ return 21; }
		typedef SingletonHolder< XUI_Lua, CreateStatic, SingletonWithLongevity >	Lua;

		template< class T >
		void show_members( const T* item, int indent )
		{
			SLB::ClassInfo* c = SLB::Manager::getInstance().getClass( typeid( *item ) );
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
						for( int n = 0; n < indent; ++n ) _cprintf( "  " );
						_cprintf( "|->" );
						SLB::LuaCall< const char*( const T*, const char*, int ) > op( Lua::Instance(), "class_attrib" );
						const char* attrib = op( item, *i, indent );
						if( attrib )
						{
							_cprintf( " %s = %s", *i, attrib );
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
			SLB::ClassInfo* c = SLB::Manager::getInstance().getClass( typeid( *pElement ) );
			if( c )
			{
				SLB::LuaCall< void( const T*, const char*, TiXmlElement* ) > op( Lua::Instance(), "save" );
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
			SLB::ClassInfo* c = SLB::Manager::getInstance().getClass( typeid( *pElement ) );
			if( c )
			{
				SLB::LuaCall< void( const T*, const char*, TiXmlElement* ) > op( Lua::Instance(), "load" );
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

		//////////////////////////////////////////////////////////////////////////
		// 设置定时器
		//////////////////////////////////////////////////////////////////////////
		unsigned int LuaSetTimer( SLB::LuaObject& function, unsigned short repeat, unsigned short timer );

		//////////////////////////////////////////////////////////////////////////
		// 删除定时器
		//////////////////////////////////////////////////////////////////////////
		void LuaKillTimer( unsigned int handle );

		//////////////////////////////////////////////////////////////////////////
		// 创建UI控件
		//////////////////////////////////////////////////////////////////////////
		XUI_Wnd*	CreateUI( _lpcstr lpszLable );
	}
}
#endif //_LUABINDCLASS_H