#include "GuiHeader.h"
#include "LuaBindClass.h"
#include "DataHelper.h"
#include "UICommander.h"
//////////////////////////////////////////////////////////////////////////
#include "XUI_Button.h"
#include "XUI_Dialog.h"
#include "XUI_EditBox.h"
#include "XUI_Window.h"
//////////////////////////////////////////////////////////////////////////
#include "LuaDebuger.h"
//////////////////////////////////////////////////////////////////////////
#include "SLB\SLB.hpp"
using namespace SLB;

using namespace XGC;
namespace UILib
{
	LuaBindClass::LuaBindClass(void)
		: m_pLuaDebuger( NULL )
	{
	}

	LuaBindClass::~LuaBindClass(void)
	{
		delete m_pLuaDebuger;
	}

	void LuaBindClass::Initialize()
	{
		m_pLuaDebuger = new LuaDebuger();
		m_pLuaDebuger->initialize( Lua::Instance().getState(), "ui"  );

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

		Class< XUI_SpriteAttribute >( "ui::XUI_SpriteAttribute" )
			.constructor< const char*, float, float, float, float >()
			.member_readonly( "f",	&XUI_SpriteAttribute::path )
			.member_readonly( "x",	&XUI_SpriteAttribute::x )
			.member_readonly( "y",	&XUI_SpriteAttribute::y )
			.member_readonly( "w",	&XUI_SpriteAttribute::w )
			.member_readonly( "h",	&XUI_SpriteAttribute::h )
			.set( "members",&show_members< XUI_SpriteAttribute > )
			.set( "save",	&XUI_SpriteAttribute::save_file )
			.set( "load",	&XUI_SpriteAttribute::load_file )
			;

		Class< XUI_ISprite, Instance::NoCopy >( "ui::XUI_ISprite" )
			.set( "Render", &XUI_ISprite::Render )
			.set( "RenderStretch", &XUI_ISprite::RenderStretch )
			.set( "RenderEx", &XUI_ISprite::RenderEx )
			.set( "GetWidth",	&XUI_ISprite::GetWidth )
			.set( "GetHeight",	&XUI_ISprite::GetHeight )
			.set( "SetUV",		&XUI_ISprite::SetUV )
			.set( "GetU0",		&XUI_ISprite::GetU0 )
			.set( "GetU1",		&XUI_ISprite::GetU1 )
			.set( "GetV0",		&XUI_ISprite::GetV0 )
			.set( "GetV1",		&XUI_ISprite::GetV1 )
			.member( "alpha",	&XUI_ISprite::SetAlpha,		&XUI_ISprite::GetAlpha )
			.member( "angle",	&XUI_ISprite::SetAngle,		&XUI_ISprite::GetAngle )
			.member( "diffuse", &XUI_ISprite::SetDiffuse,	&XUI_ISprite::GetDiffuse )
			;

		Class< XUI_FontAttribute >( "ui::XUI_FontAttribute" )
			.constructor< const char*, int, bool, bool, bool >()
			.member_readonly( "name", &XUI_FontAttribute::name )
			.member_readonly( "size", &XUI_FontAttribute::size )
			.member_readonly( "bold", &XUI_FontAttribute::bold )
			.member_readonly( "italic",		&XUI_FontAttribute::italic )
			.member_readonly( "antialias",	&XUI_FontAttribute::antialias )
			.set( "members",&show_members< XUI_FontAttribute > )
			.set( "save",	&XUI_FontAttribute::save_file )
			.set( "load",	&XUI_FontAttribute::load_file )
			;

		Class< XUI_IFont, Instance::NoCopy >( "ui::XUI_IFont" )
			;
		Class< XUI_Wnd, Instance::NoCopyNoDestroy >( "ui::Base" )
			.member( "id",			&XUI_Base::SetID,		&XUI_Base::GetID )
			;

		Class< XUI_Wnd, Instance::NoCopyNoDestroy >( "ui::Wnd" )
			.static_inherits< XUI_Base >()
			.set( "MoveWindow",		&XUI_Wnd::MoveWindow )
			.set( "OffsetWindow",	&XUI_Wnd::Offset )
			.set( "SendUIMessage",	&XUI_Wnd::SendUIMessage )
			.set( "SetName",		&XUI_Wnd::SetName )
			.set( "GetName",		&XUI_Wnd::GetName )
			.set( "members",		&XUI_Wnd::show_members )
			.set( "save",			&XUI_Wnd::save_file )
			.set( "load",			&XUI_Wnd::load_file )
			.member( "name",		&XUI_Wnd::SetName,		&XUI_Wnd::GetName )
			.member( "visible",		&XUI_Wnd::ShowWindow,	&XUI_Wnd::IsVisible )
			.member( "enable",		&XUI_Wnd::EnableWindow, &XUI_Wnd::IsEnable	)
			.member( "font",		&XUI_Wnd::SetFontAttribute,			&XUI_Wnd::GetFontAttribute )
			.member( "background",	&XUI_Wnd::SetBackgroundAttribute,	&XUI_Wnd::GetBackgroundAttribute )
			.member( "transparent",	&XUI_Wnd::m_bTranslateParent )
			.member_readonly( "window",	&XUI_Wnd::m_WindowRect )
			;

		Class< XUI_Window >( "ui:Window" )
			.static_inherits< XUI_Wnd >()
			.set( "Width",	&XUI_Window::GetWidth )
			.set( "Height", &XUI_Window::GetHeight )
			.member( "x",	&XUI_Window::m_nOffsetX )
			.member( "y",	&XUI_Window::m_nOffsetY )
			;

		Class< XUI_Button >( "ui::Button" )
			.static_inherits< XUI_Wnd >()
			.constructor()
			.set( "SetState",	&XUI_Button::SetState )
			.set( "SetSkin",	&XUI_Button::SetSkin )
			.member( "caption",	&XUI_Button::m_strCaption )
			.member_readonly( "skin",	&XUI_Button::m_SpriteAttribute )
			.enumValue( "Normal",		XUI_Button::Normal )
			.enumValue( "MouseOver",	XUI_Button::MouseOver )
			.enumValue( "ButtonDown",	XUI_Button::ButtonDown )
			.enumValue( "Disable",		XUI_Button::Disable )
			;

		Class< XUI_EditBox >( "ui::EditCtrl" )
			.static_inherits< XUI_Wnd >()
			.constructor()
			.member( "text", &XUI_EditBox::SetText, &XUI_EditBox::GetText )
			.member( "warp", &XUI_EditBox::m_bWarpText )
			;


		try
		{
			Manager::getInstance().set( "CreateSprite",		FuncCall::create( XUI_CreateSprite ) );
			Manager::getInstance().set( "CreateSpriteEx",	FuncCall::create( XUI_CreateSpriteEx ) );
			Manager::getInstance().set( "DestroySprite",	FuncCall::create( XUI_DestroySprite ) );

			Manager::getInstance().set( "CreateFont",	FuncCall::create( XUI_CreateFont ) );
			Manager::getInstance().set( "CreateFontEx",	FuncCall::create( XUI_CreateFontEx ) );
			Manager::getInstance().set( "DestroyFont",	FuncCall::create( XUI_DestroyFont ) );

			Manager::getInstance().set( "DrawText",		FuncCall::create( XUI_DrawText ) );
			Manager::getInstance().set( "DrawCharacter",FuncCall::create( XUI_DrawCharacter ) );
			Manager::getInstance().set( "DrawSprite",	FuncCall::create( XUI_DrawSprite ) );
			Manager::getInstance().set( "DrawRect",		FuncCall::create( XUI_DrawRect ) );

			_tchar path[_MAX_PATH+_MAX_FNAME];
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
