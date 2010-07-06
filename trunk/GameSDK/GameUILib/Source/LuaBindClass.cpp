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
		SLB::Class< TiXmlElement >( "xml::Element" )
			.constructor< const char* >()
			.set< TiXmlElement, void, const char*, const char* >( "SetAttribute",	&TiXmlElement::SetAttribute )
			.set< TiXmlElement, const char*, const char* >( "Attribute", &TiXmlElement::Attribute )
			.set( "InsertChild",	&TiXmlNode::InsertEndChild )
			.static_inherits< TiXmlNode >()
			;

		SLB::Class< xgcRect >( "ui::Rect" )
			.constructor< long, long, long, long >()
			.member( "left",	&xgcRect::left )
			.member( "top",		&xgcRect::top	)
			.member( "right",	&xgcRect::right )
			.member( "bottom",	&xgcRect::bottom )
			.set( "members",	&show_members< xgcRect > )
			.set( "save",		&save_file< xgcRect > )
			.set( "load",		&load_file< xgcRect > )
			;

		SLB::Class< xgcPoint >( "ui::Point" )
			.constructor< long, long >()
			.member( "x", &xgcPoint::x )
			.member( "y", &xgcPoint::y )
			.set( "members",	&show_members< xgcPoint > )
			.set( "save",		&save_file< xgcPoint > )
			.set( "load",		&load_file< xgcPoint > )
			;

		SLB::Class< xgcSize >( "ui::Size" )
			.constructor< long, long >()
			.member( "cx", &xgcSize::cx )
			.member( "cy", &xgcSize::cy )
			.set( "members",	&show_members< xgcSize > )
			.set( "save",		&save_file< xgcSize > )
			.set( "load",		&load_file< xgcSize > )
			;

		SLB::Class< XUI_SpriteAttribute >( "ui::XUI_SpriteAttribute" )
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

		SLB::Class< XUI_ISprite, SLB::Instance::NoCopy >( "ui::XUI_ISprite" )
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

		SLB::Class< XUI_FontAttribute >( "ui::XUI_FontAttribute" )
			.constructor< const char*, int, _uint32, bool, bool, bool >()
			.member_readonly( "name", &XUI_FontAttribute::m_name )
			.member_readonly( "size", &XUI_FontAttribute::m_size )
			.member_readonly( "color",&XUI_FontAttribute::m_color )
			.member_readonly( "bold", &XUI_FontAttribute::m_bold )
			.member_readonly( "italic",		&XUI_FontAttribute::m_italic )
			.member_readonly( "antialias",	&XUI_FontAttribute::m_antialias )
			.set( "members",&show_members< XUI_FontAttribute > )
			.set( "save",	&XUI_FontAttribute::save_file )
			.set( "load",	&XUI_FontAttribute::load_file )
			;

		SLB::Class< XUI_IFont, SLB::Instance::NoCopy >( "ui::XUI_IFont" )
			.set( "SetColor", &XUI_IFont::SetColor )
			;

		SLB::Class< CGuiSystem, SLB::Instance::NoCopyNoDestroy >( "ui::GuiSystem" )
			.set( "GetRoot",		&CGuiSystem::GetRoot )
			.set( "GetDesktop",		&CGuiSystem::GetDesktop )
			.set( "Load",			&CGuiSystem::LoadFromFile )
			.set( "Save",			&CGuiSystem::SaveToFile )
			.set( "GetResPath",		&CGuiSystem::GetResourcePath )
			;

		SLB::Class< XUI_Wnd, SLB::Instance::NoCopyNoDestroy >( "ui::Base" )
			.member( "id",			&XUI_Base::SetID,		&XUI_Base::GetID )
			;

		SLB::Class< XUI_Wnd, SLB::Instance::NoCopyNoDestroy >( "ui::Wnd" )
			.static_inherits< XUI_Base >()
			.set( "MoveWindow",		&XUI_Wnd::MoveWindow )
			.set( "OffsetWindow",	&XUI_Wnd::Offset )
			.set( "SendUIMessage",	reinterpret_cast< _ulong ( UILib::XUI_Wnd::* )( _uint32, _ulong, _ulong ) >( &XUI_Wnd::SendUIMessage ) )
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
			.member_readonly( "position",	&XUI_Wnd::m_WindowPosition )
			.member_readonly( "size",	&XUI_Wnd::m_WindowSize )
			;

		SLB::Class< XUI_Window, SLB::Instance::NoCopy >( "ui:Window" )
			.static_inherits< XUI_Wnd >()
			.set( "Width",	&XUI_Window::GetWidth )
			.set( "Height", &XUI_Window::GetHeight )
			.member( "x",	&XUI_Window::m_nOffsetX )
			.member( "y",	&XUI_Window::m_nOffsetY )
			;

		SLB::Class< XUI_Button, SLB::Instance::NoCopy >( "ui::Button" )
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

		SLB::Class< XUI_EditBox, SLB::Instance::NoCopy >( "ui::EditCtrl" )
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
			SLB::Manager::getInstance().set( "CreateSprite",	SLB::FuncCall::create( XUI_CreateSprite ) );
			SLB::Manager::getInstance().set( "CreateSpriteEx",	SLB::FuncCall::create( XUI_CreateSpriteEx ) );
			SLB::Manager::getInstance().set( "DestroySprite",	SLB::FuncCall::create( XUI_DestroySprite ) );

			SLB::Manager::getInstance().set( "CreateFont",		SLB::FuncCall::create( XUI_CreateFont ) );
			SLB::Manager::getInstance().set( "CreateFontEx",	SLB::FuncCall::create( XUI_CreateFontEx ) );
			SLB::Manager::getInstance().set( "DestroyFont",		SLB::FuncCall::create( XUI_DestroyFont ) );

			SLB::Manager::getInstance().set( "DrawText",		SLB::FuncCall::create( XUI_DrawTextA ) );
			SLB::Manager::getInstance().set( "DrawCharacter",	SLB::FuncCall::create( XUI_DrawCharacterA ) );
			SLB::Manager::getInstance().set( "DrawSprite",		SLB::FuncCall::create( XUI_DrawSprite ) );
			SLB::Manager::getInstance().set( "DrawRect",		SLB::FuncCall::create( XUI_DrawRect ) );

			//////////////////////////////////////////////////////////////////////////
			// lua相关函数
			SLB::Manager::getInstance().set( "SetTimer",		SLB::FuncCall::create( LuaSetTimer ) );
			SLB::Manager::getInstance().set( "KillTimer",		SLB::FuncCall::create( LuaKillTimer ) );
			SLB::Manager::getInstance().set( "CreateUI",		SLB::FuncCall::create( CreateUI ) );

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

		return GuiSystem::Instance().SetTimer( _tfunction( call( function ) ), repeat, timer );
	}

	//////////////////////////////////////////////////////////////////////////
	// 删除定时器
	//////////////////////////////////////////////////////////////////////////
	void LuaKillTimer( unsigned int handle )
	{
		GuiSystem::Instance().KillTimer( handle );
	}

	XUI_Wnd*	CreateUI( _lpcstr lpszLable )
	{
		return CUIFactory::GetInstance().Creator( XA2T(lpszLable) );
	}
}
