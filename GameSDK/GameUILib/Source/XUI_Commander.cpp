#include "XUI_Header.h"
#include "XUI_Commander.h"
#include "XUI_Factory.h"
#include "XUI_Wnd.h"
#include "XUI_Window.h"
#include "XUI_System.h"
#include "XUI_LuaBinder.h"
namespace XGC
{
	namespace ui
	{
		XUI_Commander::XUI_Commander()
		{
			m_pCurElement = XUI::Instance().GetRoot();
			RegistCommand( _T("help"),		&XUI_Commander::cmd_help,		_T("help")	);
			RegistCommand( _T("create"),	&XUI_Commander::cmd_create,		_T("create control :create [lable]")	);
			RegistCommand( _T("delete"),	&XUI_Commander::cmd_delete,		_T("delete control :delete (index|control name)")	);
			RegistCommand( _T("attrib"),	&XUI_Commander::cmd_attrib,		_T("check control attrib :attrib (index|control name)")	);
			RegistCommand( _T("save"),		&XUI_Commander::cmd_save,		_T("save ui to file :save filename")	);
			RegistCommand( _T("load"),		&XUI_Commander::cmd_load,		_T("load ui from file :load filename")	);
			RegistCommand( _T("root"),		&XUI_Commander::cmd_root,		_T("back to root path:root")	);
			RegistCommand( _T("child"),		&XUI_Commander::cmd_child,		_T("选择子控件 child [index|control name]")	);
			RegistCommand( _T("cd"),		&XUI_Commander::cmd_child,		_T("选择子控件 child [index|control name]")	);
			RegistCommand( _T("parent"),	&XUI_Commander::cmd_parent,		_T("返回到父控件 parent")	);
			RegistCommand( _T("tree"),		&XUI_Commander::cmd_tree,		_T("查看控件树 tree (index|control name)")	);
			RegistCommand( _T("dir"),		&XUI_Commander::cmd_tree,		_T("查看控件树 tree (index|control name)")	);
			RegistCommand( _T("lua"),		&XUI_Commander::cmd_lua,			_T("执行lua脚本 lua [script]")	);
		}

		XUI_Commander::~XUI_Commander()
		{

		}

		void XUI_Commander::RegistCommand( _lpctstr lpszCommand, Command func, _lpctstr lpszHelpString  )
		{
			m_cmdMap.insert( std::make_pair( lpszCommand, cmd( func, lpszHelpString ) ) );
		}

		bool EnumAllChild( XUI_Wnd* pXUI_Wnd, LPVOID lpParam )
		{
			int& count = *(int*)lpParam;
			_tprintf( _T("\t%[%03d]\t%16s < %s >\n"), count++, pXUI_Wnd->GetName().c_str(), typeid(pXUI_Wnd).raw_name() );
			return false;
		}

		bool FindChildByIndex( XUI_Wnd* pXUI_Wnd, LPVOID lpParam )
		{
			_uint32& count = *(_uint32*)lpParam;
			if( LOWORD( count ) == HIWORD( count ) )
				return true;
			count = MAKELONG( LOWORD( count ) + 1, HIWORD( count ) );
			return false;
		}

		bool FindChildByName( XUI_Wnd* pXUI_Wnd, LPVOID lpParam )
		{
			_lpctstr lpszName = (_lpctstr)lpParam;
			if( pXUI_Wnd->GetName() == lpszName )
				return true;
			return false;
		}

		bool BuildChildTree( XUI_Wnd* pXUI_Wnd, LPVOID lpParam )
		{
			_uint32& count = *(_uint32*)lpParam;
			for( WORD i = 0; i < LOWORD( count ); ++i ) _tprintf( _T("     ") );
			_tprintf( _T("|--[%03d] %s < %s >\n"), HIWORD(count), pXUI_Wnd->GetName().c_str(), typeid(pXUI_Wnd).raw_name() );

			_uint32 child = MAKELONG( LOWORD( count ) + 1, 0 );
			pXUI_Wnd->ForAllChild( BuildChildTree, (LPVOID)&child );
			count = MAKELONG( LOWORD( count ), HIWORD( count ) +1 );
			return false;
		}

		XUI_Wnd* XUI_Commander::GetElementByPath( _lpctstr lpszPath )
		{
			_tchar path[1024];
			_tcsncpy_s( path, _countof(path), lpszPath, _TRUNCATE );
			LPTSTR section, next;

			_tchar sep[] = _T("/\\");

			XUI_Wnd* pCurElement = m_pCurElement;
			section = _tcstok_s( path, sep, &next );
			while( section )
			{
				if( _tcsicmp( _T("parent"), section ) == 0 ||
					_tcsicmp( _T(".."), section ) == 0 )
					pCurElement = pCurElement->GetParent();
				else if( section[0] == _T('~') )
					pCurElement = XUI::Instance().GetRoot();
				else
				{
					_string child( section );
					while( _istdigit(*section) ) ++section;

					XUI_Wnd* pXUI_Wnd = NULL;
					if( *section == 0 )
					{
						// using index select child
						_uint32 p = MAKELONG(0,_ttoi(child.c_str()));
						pXUI_Wnd = pCurElement->ForAllChild( FindChildByIndex, (LPVOID)&p );
					}
					else
					{
						// using name select child
						pXUI_Wnd = pCurElement->ForAllChild( FindChildByName, (LPVOID)child.c_str() );
					}

					if( !pXUI_Wnd ) return pCurElement;
					pCurElement = pXUI_Wnd;
				}

				section = _tcstok_s( NULL, sep, &next );
			}
			return pCurElement;
		}

		bool XUI_Commander::ProcessCommand( _lpctstr lpszCommand )
		{
			_lpctstr pCmd = lpszCommand;
			while( *pCmd && _istalnum( *pCmd ) ) ++pCmd;

			_string strCmd( lpszCommand, pCmd - lpszCommand );
			CCommandMap::iterator iter = m_cmdMap.find( strCmd );
			if( iter != m_cmdMap.end() )
			{
				_tchar szParam[1024];
				UINT nPos = 0;
				Params param;
				bool bString = false;
				while( *pCmd && !_istalnum( *pCmd ) && 
					*pCmd != _T('[') &&
					*pCmd != _T('.') &&
					*pCmd != _T('_') && 
					*pCmd != _T('/') && 
					*pCmd != _T('~') && 
					*pCmd != _T('\\')
					) ++pCmd;
				while( *pCmd )
				{
					if( *pCmd == _T('[') )
					{
						++pCmd;
						bString = true;
					}
					szParam[nPos++] = *pCmd++;
					if( *pCmd == 0 || ( bString?(_T(']')==*pCmd):
						( 
						*pCmd != _T('.') &&
						*pCmd != _T('_') && 
						*pCmd != _T('/') && 
						*pCmd != _T('~') && 
						*pCmd != _T('\\') &&
						!_istalnum( *pCmd ) 
						) ) )
					{
						szParam[nPos] = 0;
						param.push_back( szParam );
						nPos = 0;
						bString = false;
						while( *pCmd && !_istalnum( *pCmd ) && *pCmd != _T('[') ) ++pCmd;
					}
				}

				(this->* iter->second.func)( param );
			}
			else
			{
				try
				{
					Lua::Instance().set( "this", m_pCurElement );
					Lua::Instance().doString( lpszCommand );
				}
				catch( std::runtime_error& err )
				{
					puts( err.what() );
				}
			}
			return true;
		}

		bool XUI_Commander::Load( _lpcstr lpszFilename )
		{
			if( XUI::Instance().LoadFromFile( lpszFilename ) == false )
			{
				_tprintf( _T("load file failed.\n") );
			}
			else
			{
				m_pCurElement = XUI::Instance().GetRoot();
				return true;
			}
			return false;
		}

		bool XUI_Commander::Save( _lpcstr lpszFilename )
		{
			if( XUI::Instance().SaveToFile( lpszFilename ) )
			{
				_tprintf( _T("save file successful.\n" ) );
				return true;
			}
			return false;
		}

		bool XUI_Commander::cmd_help( Params& param )
		{
			_tprintf( _T("UICommander help:\n") );
			CCommandMap::iterator iter = m_cmdMap.begin();
			while( m_cmdMap.end() != iter )
			{
				_tprintf( _T("\t%s\t%s\n"), iter->first.c_str(), iter->second.helpString );
				++iter;
			}
			return true;
		}

		bool XUI_Commander::cmd_create( Params& param )
		{
			size_t param_count = param.size();
			if( param_count < 1 ) return true;

			_lptstr p = _tcsdup( param[0].c_str() );
			_tcsupr_s( p, param[0].size() + 1 );

			XUI_Wnd* pCurElement = XUI_Factory::GetInstance().Creator( XT2A(p) );
			if( !pCurElement )
			{
				_tprintf( _T("create %s failed.\n"), p );
				return false;
			}
			_tprintf( _T("create %s success.\n"), p );

			if( m_pCurElement == NULL ) m_pCurElement = XUI::Instance().GetRoot();
			if( m_pCurElement )
			{
				if( param_count < 2 )
				{
					m_pCurElement->AddChild( pCurElement );
				}
				else
				{
					XUI_Wnd* pParent = GetElementByPath( param[1].c_str() );
					if( pParent )
					{
						pParent->AddChild( pCurElement );
					}
				}
			}
			m_pCurElement = pCurElement;
			return true;
		}

		bool XUI_Commander::cmd_delete( Params& param )
		{
			XUI_Window* pDesktop = XUI::Instance().GetRoot();
			if( param.size() == 0 )
			{
				if( pDesktop == m_pCurElement )
				{
					_cputts( _T("Connot delete Desktop.") );
					return false;
				}

				XUI_Wnd* pParent = m_pCurElement->GetParent();
				if( pParent )
				{
					_tprintf( _T("\t%s <%s> was destroy."), m_pCurElement->GetName().c_str(), typeid(m_pCurElement).raw_name() );
					m_pCurElement->Release();
					m_pCurElement = pParent;
				}
			}
			else if( param.size() == 1 )
			{
				XUI_Wnd* pXUI_Wnd = GetElementByPath( param[0].c_str() );
				if( pXUI_Wnd && pXUI_Wnd != pDesktop )
				{
					XUI_Wnd* pParent = pXUI_Wnd;
					do
					{
						// delete parent node
						if( pParent == m_pCurElement ) break;
						pParent = pParent->GetParent();
					}while( pParent != pDesktop );

					if( pParent == m_pCurElement ) m_pCurElement = pXUI_Wnd->GetParent();
					pXUI_Wnd->Release();
				}
			}
			return true;
		}

		bool XUI_Commander::cmd_attrib( Params& param )
		{
			if( !m_pCurElement ) return false;

			m_pCurElement->show_members( 0 );
			return true;
		}

		bool XUI_Commander::cmd_save( Params& param )
		{
			if( param.size() != 1 ) return false;
			XUI::Instance().SaveToFile( XT2A(param[0]) );
			_tprintf( _T("save file successful.\n" ) );
			return true;
		}

		bool XUI_Commander::cmd_load( Params& param )
		{
			if( param.size() != 1 ) return false;
			if( XUI::Instance().LoadFromFile( XT2A(param[0]) ) == false )
			{
				_tprintf( _T("load file failed.\n") );
			}
			else
			{
				m_pCurElement = XUI::Instance().GetRoot();
			}
			return true;
		}

		bool XUI_Commander::cmd_root( Params& param )
		{
			m_pCurElement = XUI::Instance().GetRoot();
			int count = 0;
			m_pCurElement->ForAllChild( EnumAllChild, &count );
			return true;
		}

		bool XUI_Commander::cmd_tree( Params& param )
		{
			if( !m_pCurElement ) return true;
			_uint32 count = 1;

			XUI_Wnd* pTreeRoot = ( param.size() == 0 )?m_pCurElement:GetElementByPath( param[0].c_str() );
			_tprintf( _T("[%03d] %s < %s >\n"), HIWORD(count), pTreeRoot->GetName().c_str(), typeid(pTreeRoot).raw_name() );
			pTreeRoot->ForAllChild( BuildChildTree, (LPVOID)&count );
			return true;
		}

		bool XUI_Commander::cmd_child( Params& param )
		{
			if( !m_pCurElement ) return true;
			int count = 0;
			if( param.size() == 0 )
			{
				_tprintf( _T("############### Child list ################\n") );
				m_pCurElement->ForAllChild( EnumAllChild, &count );
			}
			else if( param.size() == 1 )
			{
				XUI_Wnd* pXUI_Wnd = GetElementByPath( param[0].c_str() );

				if( pXUI_Wnd )
				{
					m_pCurElement = pXUI_Wnd;
					m_pCurElement->ForAllChild( EnumAllChild, (LPVOID)&count );
				}
			}
			else
			{
				return false;
			}
			return true;
		}

		bool XUI_Commander::cmd_parent( Params& param )
		{
			if( !m_pCurElement ) return true;
			XUI_Wnd* pParent = m_pCurElement->GetParent();
			if( pParent )
			{
				int count = 0;
				m_pCurElement = pParent;
				m_pCurElement->ForAllChild( EnumAllChild, (LPVOID)&count );
				return true;
			}
			return false;
		}

		bool XUI_Commander::cmd_lua( Params& param )
		{
			LPCSTR szCommand = XT2A( param[0] );
			try
			{
				Lua::Instance().set( "this", m_pCurElement );
				Lua::Instance().doString( szCommand );
				return true;
			}
			catch( std::exception const& e )
			{
				puts( e.what() );
				return false;
			}
		}
	}
}