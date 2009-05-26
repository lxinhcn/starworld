#include "GuiHeader.h"
#include "LuaBindClass.h"
#include "DataHelper.h"
#include "GuiSystem.h"
#include "UICommander.h"
//////////////////////////////////////////////////////////////////////////
#include "XUI_Button.h"
#include "XUI_Dialog.h"
#include "XUI_EditBox.h"
#include "XUI_Window.h"
#include "UIFactory.h"
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

	bool LuaBindClass::SetupDebuger()
	{
		if( m_pLuaDebuger == NULL )
		{
			m_pLuaDebuger = new LuaDebuger();
			m_pLuaDebuger->initialize( getState(), "ui"  );
		}
		return true;
	}

	void LuaBindClass::Initialize()
	{
		Class< TiXmlElement >( "xml::Element" )
			.constructor< const char* >()
			.set< TiXmlElement, void, const char*, const char* >( "SetAttribute",	&TiXmlElement::SetAttribute )
			.set< TiXmlElement, const char*, const char* >( "Attribute", &TiXmlElement::Attribute )
			.set( "InsertChild",	&TiXmlNode::InsertEndChild )
			.static_inherits< TiXmlNode >()
			;

		Class< x_rect >( "ui::Rect" )
			.constructor< long, long, long, long >()
			.member( "left",	&x_rect::left )
			.member( "top",		&x_rect::top	)
			.member( "right",	&x_rect::right )
			.member( "bottom",	&x_rect::bottom )
			.set( "members",	&show_members< x_rect > )
			.set( "save",		&save_file< x_rect > )
			.set( "load",		&load_file< x_rect > )
			;

		Class< x_point >( "ui::Point" )
			.constructor< long, long >()
			.member( "x", &x_point::x )
			.member( "y", &x_point::y )
			.set( "members",	&show_members< x_point > )
			.set( "save",		&save_file< x_point > )
			.set( "load",		&load_file< x_point > )
			;

		Class< x_size >( "ui::Size" )
			.constructor< long, long >()
			.member( "cx", &x_size::cx )
			.member( "cy", &x_size::cy )
			.set( "members",	&show_members< x_size > )
			.set( "save",		&save_file< x_size > )
			.set( "load",		&load_file< x_size > )
			;

		Class< XUI_SpriteAttribute >( "ui::XUI_SpriteAttribute" )
			.constructor< _lpcstr, float, float, float, float >()
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
			.static_inherits< XUI_SpriteAttribute >()
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
			.set( "SetColor", &XUI_IFont::SetColor )
			;

		Class< CGuiSystem, Instance::NoCopyNoDestroy >( "ui::GuiSystem" )
			.set( "GetRoot",		&CGuiSystem::GetRoot )
			.set( "GetDesktop",		&CGuiSystem::GetDesktop )
			.set( "Load",			&CGuiSystem::LoadFromFile )
			.set( "Save",			&CGuiSystem::SaveToFile )
			.set( "GetResPath",		&CGuiSystem::GetResourcePath )
			;

		Class< XUI_Wnd, Instance::NoCopyNoDestroy >( "ui::Base" )
			.member( "id",			&XUI_Base::SetID,		&XUI_Base::GetID )
			;

		Class< XUI_Wnd, Instance::NoCopyNoDestroy >( "ui::Wnd" )
			.static_inherits< XUI_Base >()
			.set( "MoveWindow",		&XUI_Wnd::MoveWindow )
			.set( "OffsetWindow",	&XUI_Wnd::Offset )
			.set( "SendUIMessage",	reinterpret_cast< ulong ( UILib::XUI_Wnd::* )( uint32, ulong, ulong ) >( &XUI_Wnd::SendUIMessage ) )
			.set( "members",		&XUI_Wnd::show_members )
			.set( "save",			&XUI_Wnd::save_file )
			.set( "load",			&XUI_Wnd::load_file )
			.set( "AddChild",		&XUI_Wnd::AddChild )
			.set( "AddChildAt",		&XUI_Wnd::AddChildAt )
			.set( "RemoveChild",	&XUI_Wnd::RemoveChild )
			.set( "GetParent",		&XUI_Wnd::GetParent )
			.set( "FindChildByName",&XUI_Wnd::FindChildByName )
			.set( "FindChildByID",	&XUI_Wnd::FindChildByID )
			.set( "BringToUp",		&XUI_Wnd::BringToUp )
			.set( "BringToDown",	&XUI_Wnd::BringToDown )
			.set( "BringToFront",	&XUI_Wnd::BringToFront )
			.set( "BringToEnd",		&XUI_Wnd::BringToEnd )
			.member( "name",		&XUI_Wnd::SetName,		&XUI_Wnd::GetName )
			.member( "visible",		&XUI_Wnd::ShowWindow,	&XUI_Wnd::IsVisible )
			.member( "enable",		&XUI_Wnd::EnableWindow,	&XUI_Wnd::IsEnable	)
			.member( "font",		&XUI_Wnd::m_pFont )
			.member( "transparent",	&XUI_Wnd::m_bTranslateParent )
			.member( "updatefunc",	&XUI_Wnd::m_strUpdateFunc )
			.member( "background",	&XUI_Wnd::m_pBackGround )
			.member_readonly( "window",	&XUI_Wnd::m_WindowRect )
			;

		Class< XUI_Window, Instance::NoCopy >( "ui:Window" )
			.static_inherits< XUI_Wnd >()
			.set( "Width",	&XUI_Window::GetWidth )
			.set( "Height", &XUI_Window::GetHeight )
			.member( "x",	&XUI_Window::m_nOffsetX )
			.member( "y",	&XUI_Window::m_nOffsetY )
			;

		Class< XUI_Button, Instance::NoCopy >( "ui::Button" )
			.static_inherits< XUI_Wnd >()
			.constructor()
			.set( "SetState",	&XUI_Button::SetState )
			.member( "caption",	&XUI_Button::m_strCaption )
			.member( "skin",	&XUI_Button::m_pButtonSkin )
			.enumValue( "Normal",		XUI_Button::Normal )
			.enumValue( "MouseOver",	XUI_Button::MouseOver )
			.enumValue( "ButtonDown",	XUI_Button::ButtonDown )
			.enumValue( "Disable",		XUI_Button::Disable )
			;

		Class< XUI_EditBox, Instance::NoCopy >( "ui::EditCtrl" )
			.static_inherits< XUI_Wnd >()
			.constructor()
			.member( "text",			&XUI_EditBox::SetText, &XUI_EditBox::GetText )
			.member( "warp",			&XUI_EditBox::m_bWarpText )
			.member( "bordercolor",		&XUI_EditBox::m_dwBorderColor )
			.member( "backgroundcolor",	&XUI_EditBox::m_dwBackgroundColor )
			.member( "singleline",		&XUI_EditBox::m_bSingleLine )
			;

		try
		{
			Manager::getInstance().set( "CreateSprite",		FuncCall::create( XUI_CreateSprite ) );
			Manager::getInstance().set( "CreateSpriteEx",	FuncCall::create( XUI_CreateSpriteEx ) );
			Manager::getInstance().set( "DestroySprite",	FuncCall::create( XUI_DestroySprite ) );

			Manager::getInstance().set( "CreateFont",		FuncCall::create( XUI_CreateFont ) );
			Manager::getInstance().set( "CreateFontEx",		FuncCall::create( XUI_CreateFontEx ) );
			Manager::getInstance().set( "DestroyFont",		FuncCall::create( XUI_DestroyFont ) );

			Manager::getInstance().set( "DrawText",			FuncCall::create( XUI_DrawTextA ) );
			Manager::getInstance().set( "DrawCharacter",	FuncCall::create( XUI_DrawCharacterA ) );
			Manager::getInstance().set( "DrawSprite",		FuncCall::create( XUI_DrawSprite ) );
			Manager::getInstance().set( "DrawRect",			FuncCall::create( XUI_DrawRect ) );

			//////////////////////////////////////////////////////////////////////////
			// lua相关函数
			Manager::getInstance().set( "SetTimer",			FuncCall::create( LuaSetTimer ) );
			Manager::getInstance().set( "CreateUI",			FuncCall::create( CreateUI ) );

			_tchar path[_MAX_PATH+_MAX_FNAME];
			_tfullpath( path, _T("..\\Resource\\Scripts\\"), _countof( path ) );
			// 设置脚本根目录
			_string _script( path );
			set( "script", _script );
			set( "gui", &GuiSystem::Instance() );

			_script.append( _T("initialize.lua") );
			doFile( _script.c_str() );
		}
		catch( std::runtime_error& err )
		{
			OutputDebugStringA( err.what() );
			puts( err.what() );
		}
	}

	unsigned int LuaSetTimer( SLB::LuaObject& function, unsigned short repeat, unsigned short timer )
	{
		struct call
		{
			call( SLB::LuaObject& func )
				: function( func )
			{

			}

			~call()
			{

			}

			bool operator()( unsigned int handle, unsigned short& repeat, unsigned int& timer )
			{
				try
				{
					return function( handle, repeat, timer );
				}
				catch( std::runtime_error& error )
				{
					puts( error.what() );
				}

				return false;
			}

			SLB::LuaCall< bool( unsigned int /*handle*/, unsigned short& /*repeat*/, unsigned int& /*timer*/) > function;
		};

		return GuiSystem::Instance().SetTimer( event_function( call( function ) ), repeat, timer );
	}

	XUI_Wnd*	CreateUI( _lpcstr lpszLable )
	{
		return CUIFactory::GetInstance().Creator( XA2T(lpszLable) );
	}
}
