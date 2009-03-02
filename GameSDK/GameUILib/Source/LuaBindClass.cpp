#include "GuiHeader.h"
#include "LuaBindClass.h"
#include "DataHelper.h"
#include "UICommander.h"
//////////////////////////////////////////////////////////////////////////
#include "XUI_Button.h"
#include "XUI_Dialog.h"
#include "XUI_EditBox.h"
#include "XUI_Window.h"
#include "SLB\SLB.hpp"
using namespace SLB;

using namespace XGC;
namespace UILib
{
	LuaBindClass::LuaBindClass(void)
	{
	}

	LuaBindClass::~LuaBindClass(void)
	{
	}

	void LuaBindClass::Initialize()
	{
		//SLB::Class< CUICommander, SLB::Instance::NoCopyNoDestroy >( "ui::Commander" )
		//	.set( "run", &CUICommander::ProcessCommand )
			;

		Class< TiXmlElement >( "xml::Element" )
			.constructor< const char* >()
			.set< TiXmlElement, void, const char*, const char* >( "SetAttribute",	&TiXmlElement::SetAttribute )
			.set< TiXmlElement, const char*, const char* >( "Attribute", &TiXmlElement::Attribute )
			.set( "InsertChild",	&TiXmlNode::InsertEndChild )
			.static_inherits< TiXmlNode >()
			;

		Class< CRect >( "ui::Rect" )
			.constructor< long, long, long, long >()
			.member( "left",	&CRect::left )
			.member( "top",		&CRect::top	)
			.member( "right",	&CRect::right )
			.member( "bottom",	&CRect::bottom )
			.set( "members",	&show_members< CRect > )
			.set( "save",		&save_file< CRect > )
			.set( "load",		&load_file< CRect > )
		;

		Class< CPoint >( "ui::Point" )
			.constructor< long, long >()
			.member( "x", &CPoint::x )
			.member( "y", &CPoint::y )
			.set( "members",	&show_members< CPoint > )
			.set( "save",		&save_file< CPoint > )
			.set( "load",		&load_file< CPoint > )
			;

		Class< CSize >( "ui::Size" )
			.constructor< long, long >()
			.member( "cx", &CSize::cx )
			.member( "cy", &CSize::cy )
			.set( "members",	&show_members< CSize > )
			.set( "save",		&save_file< CSize > )
			.set( "load",		&load_file< CSize > )
			;

		Class< SpriteAttribute >( "ui::SpriteAttribute" )
			.constructor< const char*, float, float, float, float >()
			.member_readonly( "f",	&SpriteAttribute::path )
			.member_readonly( "x",	&SpriteAttribute::x )
			.member_readonly( "y",	&SpriteAttribute::y )
			.member_readonly( "w",	&SpriteAttribute::w )
			.member_readonly( "h",	&SpriteAttribute::h )
			.set( "members",&show_members< SpriteAttribute > )
			.set( "save",	&SpriteAttribute::save_file )
			.set( "load",	&SpriteAttribute::load_file )
			;

		Class< XUI_Wnd, SLB::Instance::NoCopyNoDestroy >( "ui::Wnd" )
			.set( "MoveWindow",		&XUI_Wnd::Move )
			.set( "OffsetWindow",	&XUI_Wnd::Offset )
			.set( "SendMessage",	&XUI_Wnd::SendMessage )
			.set( "SetName",		&XUI_Wnd::SetName )
			.set( "GetName",		&XUI_Wnd::GetName )
			.set( "SetID",			&XUI_Wnd::SetID )
			.set( "GetID",			&XUI_Wnd::GetID )
			.set( "ShowWindow",		&XUI_Wnd::Show )
			.set( "EnableWindow",	&XUI_Wnd::Enable )
			.set( "members",		&show_members< XUI_Wnd > )
			.set( "save",			&XUI_Wnd::save_file )
			.set( "load",			&XUI_Wnd::load_file )
			.member( "id",			&XUI_Wnd::m_nID )
			.member( "window",		&XUI_Wnd::m_wndRect )
			.member( "name",		&XUI_Wnd::m_strName )
			.member( "visible",		&XUI_Wnd::m_bVisible)
			.member( "enable",		&XUI_Wnd::m_bEnable )
			.member( "transparent",	&XUI_Wnd::m_bTranslateParent )
			;

		Class< XUI_Window >( "ui:Window" )
			.set( "Width",	&XUI_Window::GetWidth )
			.set( "Height", &XUI_Window::GetHeight )
			.member( "x",	&XUI_Window::m_nOffsetX )
			.member( "y",	&XUI_Window::m_nOffsetY )
			.static_inherits< XUI_Wnd >()
			;

		Class< XUI_Button >( "ui::Button" )
			.constructor()
			.set( "SetState",	&XUI_Button::SetState )
			.set( "SetSkin",	&XUI_Button::SetSkin )
			.member( "caption",	&XUI_Button::m_strCaption )
			.member_readonly( "skin",	&XUI_Button::m_SpriteAttribute )
			.enumValue( "Normal",		XUI_Button::Normal )
			.enumValue( "MouseOver",	XUI_Button::MouseOver )
			.enumValue( "ButtonDown",	XUI_Button::ButtonDown )
			.enumValue( "Disable",		XUI_Button::Disable )
			.static_inherits< XUI_Wnd >()
			;

		try
		{
			TCHAR path[_MAX_PATH+_MAX_FNAME];
			helper::NormalizePath(  _T("..\\Resource\\Scripts\\"), path, _countof( path ) );
			// 设置脚本根目录
			_string _script( path );
			set( "script", _script );

			_script.append( _T("initialize.lua") );

			doFile( _script.c_str() );
		}
		catch( std::runtime_error& err )
		{
			OutputDebugStringA( err.what() );
			puts( err.what() );
		}
	}
}
