#include "StdAfx.h"
#include "Setting.h"
#include "hgefontex.h"
#include "hgefontmanager.h"
HTEXTURE LoadTexture( _lpcstr lpszTexture )
{
	HTEXTURE hTexture = 0;
	HGE* hge = hgeCreate(HGE_VERSION);
	if( hge )
	{
		hTexture = hge->Texture_Load( lpszTexture );
	}
	hge->Release();

	return hTexture;
}

int GetTextureWidth( HTEXTURE hTexture )
{
	if( hTexture == 0 )
		return 0;
	
	int width = 0;
	HGE* hge = hgeCreate(HGE_VERSION);
	if( hge )
	{
		width = hge->Texture_GetWidth( hTexture );
	}
	hge->Release();

	return width;
}

int GetTextureHeight( HTEXTURE hTexture )
{
	if( hTexture == 0 )
		return 0;

	int height = 0;
	HGE* hge = hgeCreate(HGE_VERSION);
	if( hge )
	{
		height = hge->Texture_GetHeight( hTexture );
	}
	hge->Release();

	return height;
}

AppSetting::AppSetting(void)
: mFont( NULL )
{
}

AppSetting::~AppSetting(void)
{
}

bool AppSetting::Initialize( const char* filename )
{
	Manager::getInstance().set( "Texture",		FuncCall::create( LoadTexture ) );
	Manager::getInstance().set( "TextureWidth",	FuncCall::create( GetTextureWidth ) );
	Manager::getInstance().set( "TextureHeight",FuncCall::create( GetTextureHeight ) );

	RegisterArray< b2Vec2 >( "b2::VectorArray" );

	Class< hgeSprite >( "HGE::Sprite" )
		.constructor< HTEXTURE, float, float, float, float >()
		.set("setTexture", &hgeSprite::SetTexture)
		.set("setTextureRect", &hgeSprite::SetTextureRect)
		.set("setColor", &hgeSprite::SetColor)
		.set("setHotSpot", &hgeSprite::SetHotSpot)
		;

	Class< hgeAnimation >( "HGE::Animation" )
		.constructor< HTEXTURE, int, int, int, float, float, float, float, float >()
		.set( "play", &hgeAnimation::Play )
		.set( "stop", &hgeAnimation::Stop )
		.set( "resume", &hgeAnimation::Resume )
		.set( "setSpeed", &hgeAnimation::SetSpeed )
		.set( "setFrames", &hgeAnimation::SetFrames )
		.set( "setMode", &hgeAnimation::SetMode )
		;

	Class< b2Vec2 >( "b2::Vector2" )
		.constructor< float, float >()
		.member("x", &b2Vec2::x )
		.member("y", &b2Vec2::y )
		;

	Class< b2BodyDef>( "b2::BodyDef" )
		.constructor()
		.member("type",				&b2BodyDef::type)
		.member("position",			&b2BodyDef::position )
		.member("angle",			&b2BodyDef::angle )
		.member("linearVelocity",	&b2BodyDef::linearVelocity )
		.member("angularVelocity",	&b2BodyDef::angularVelocity )
		.member("linearDamping",	&b2BodyDef::linearDamping )
		.member("angularDamping",	&b2BodyDef::angularDamping )
		.member("allowSleep",		&b2BodyDef::allowSleep )
		.member("awake",			&b2BodyDef::awake )
		.member("fixedRotation",	&b2BodyDef::fixedRotation )
		.member("bullet",			&b2BodyDef::bullet )
		.member("active",			&b2BodyDef::active )
		.member("userdata",			&b2BodyDef::userData )
		.member("gravityScale",		&b2BodyDef::gravityScale )
		;

	Class< b2Shape, Instance::NoCopyNoDestroy >( "b2::Shape" )
		.enumValue( "circle",	b2Shape::e_circle )
		.enumValue( "edge",		b2Shape::e_edge )
		.enumValue( "polygon",	b2Shape::e_polygon )
		.enumValue( "chain",	b2Shape::e_chain )

		.member("type",		&b2Shape::m_type )
		.member("radius",	&b2Shape::m_radius )
		;

	Class< b2EdgeShape >( "b2::EdgeShape" )
		.inherits< b2Shape >()
		.constructor()
		.set( "Set", &b2EdgeShape::Set )
		.member( "vertex0", &b2EdgeShape::m_vertex0 )
		.member( "vertex1", &b2EdgeShape::m_vertex1 )
		.member( "vertex2", &b2EdgeShape::m_vertex2 )
		.member( "vertex3", &b2EdgeShape::m_vertex3 )
		.member( "hasvertex0", &b2EdgeShape::m_hasVertex0 )
		.member( "hasvertex3", &b2EdgeShape::m_hasVertex3 )
		;

	Class< b2PolygonShape >( "b2::PolygonShape" )
		.inherits< b2Shape >()
		.constructor()
		.set( "Set", &b2PolygonShape::Set )
		.set< void (b2PolygonShape::*)(float32, float32) >( "SetAsBox",	&b2PolygonShape::SetAsBox )
		.set< void (b2PolygonShape::*)(float32, float32, const b2Vec2&, float32) >( "SetAsBoxEx",	&b2PolygonShape::SetAsBox )
		;

	Class< b2CircleShape >( "b2::CircleShape" )
		.inherits< b2Shape >()
		.constructor()
		.member( "position", &b2CircleShape::m_p )
		;

	Class< b2World >( "b2::World" )
		.set( "CreateBody",		&b2World::CreateBody )
		.set( "CreateJoint",	&b2World::CreateJoint )
		;

	try
	{
		mScript.doFile( "config.lua" );
		mSetting = mScript.get< LuaObject >( "setting" );
		if( !mSetting.isvalid() )
			return false;

		LuaObject package = mSetting.get< LuaObject >( "package" );
		if( package.isvalid() && package.istable() )
		{
			HGE* hge = hgeCreate( HGE_VERSION );
			for( LuaTableIterator it(package); it; it.next() )
			{
				_lpcstr v = it.getValue< _lpcstr >();
				if( v )
					hge->Resource_AttachPack( (_astring(getResourcePath()) + v).c_str() );
			}
			hge->Release();
		}

		mScript.doFile( (_astring( getResourcePath() ) + mSetting.get< _lpcstr >( "defines" )).c_str() );

		mTexture = mScript.get< LuaObject >( "texture" );

		mSprites = mScript.get< LuaObject >( "sprites" );

		mAnimation = mScript.get< LuaObject >( "animation" );

		return true;
	}
	catch( std::runtime_error e )
	{
		// XGC_ASSERT_MSGA( false, e.what() );
		return false;
	}
}

_lpcstr AppSetting::getResourcePath()
{
	if( mSetting.isvalid() && mSetting.istable() )
	{
		return mSetting.get< _lpcstr, _lpcstr >( "resource" );
	}

	return "..\\Resource\\";
}

_lpcstr AppSetting::getFontName()
{
	if( mSetting.isvalid() && mSetting.istable() )
	{
		LuaObject font = mSetting.get< LuaObject >( "font" );
		if( font.isvalid() && font.istable() )
		{
			return font.get< _lpcstr >( "fontname" );
		}
	}

	return "C:\\Windows\\SIMSUN.TTF";
}

int AppSetting::getFontSize()
{
	if( mSetting.isvalid() && mSetting.istable() )
	{
		LuaObject font = mSetting.get< LuaObject >( "font" );
		if( font.isvalid() && font.istable() )
		{
			return font.get< int >( "fontsize" );
		}
	}

	return 16;
}

hgeFontEx* AppSetting::getFont()
{
	if( mFont == NULL )
	{
		mFont = hgeFontHelper( (_astring( getResourcePath() ) + getFontName()).c_str(), getFontSize() ).getFont();
		if( mFont )
		{
			mFont->SetColor( -1 );
		}
	}

	return mFont;
}