#include "XUI_Canvas.h"
#include "tinyxml.h"

namespace UILib
{
	pfnDrawText			XUI_DrawText		= 0;
	pfnDrawCharacter	XUI_DrawCharacter	= 0;
	pfnDrawRect			XUI_DrawRect		= 0;
	pfnDrawPolygon		XUI_DrawPolygon		= 0;
	pfnDrawSprite		XUI_DrawSprite		= 0;
	pfnCreateSprite		XUI_CreateSprite	= 0;
	pfnDestroySprite	XUI_DestroySprite	= 0;
	pfnCreateFont		XUI_CreateFont		= 0;
	pfnDestroyFont		XUI_DestroyFont		= 0;

	XUI_SpriteAttribute::XUI_SpriteAttribute( const char* _path, float _x, float _y, float _w, float _h )
		: path( _path )
		, x( _x )
		, y( _y )
		, w( _w )
		, h( _h )
	{
	}

	XUI_SpriteAttribute::XUI_SpriteAttribute()
		: path( "" )
		, x( 0.0f )
		, y( 0.0f )
		, w( 0.0f )
		, h( 0.0f )
	{
	}

	bool XUI_SpriteAttribute::save_file( TiXmlElement* pNode )
	{
		TiXmlElement p( "Path" );
		p.SetAttribute( "direction", path.c_str() );
		pNode->InsertEndChild( p );

		TiXmlElement Position( "Positon" );
		Position.SetDoubleAttribute( "x", x );
		Position.SetDoubleAttribute( "y", y );
		pNode->InsertEndChild( Position );

		TiXmlElement Size( "Size" );
		Size.SetDoubleAttribute( "w", w );
		Size.SetDoubleAttribute( "h", h );
		pNode->InsertEndChild( Size );

		return true;
	}

	bool XUI_SpriteAttribute::load_file( TiXmlElement* pNode )
	{
		TiXmlElement* pElement = pNode->FirstChildElement( "Path" );
		path = pElement->Attribute( "direction" );

		pElement = pNode->FirstChildElement( "Position" );
		pElement->QueryFloatAttribute( "x", &x );
		pElement->QueryFloatAttribute( "y", &y );

		pElement = pNode->FirstChildElement( "Size" );
		pElement->QueryFloatAttribute( "w", &w );
		pElement->QueryFloatAttribute( "h", &h );

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	XUI_FontAttribute::XUI_FontAttribute()
		: name( "" )
		, size( 12 )
		, bold( false )
		, italic( false )
		, antialias( false )
	{

	}


	XUI_FontAttribute::XUI_FontAttribute( const char* lpszFont, int nSize, bool bBold, bool bItalic, bool bAntialias )
		: name( lpszFont )
		, size( nSize )
		, bold( bBold )
		, italic( bItalic )
		, antialias( bAntialias )
	{

	}

	XUI_FontAttribute::XUI_FontAttribute( const XUI_FontAttribute& src )
		: name( src.name )
		, size( src.size )
		, bold( src.bold )
		, italic( src.italic )
		, antialias( src.antialias )
	{

	}

	bool XUI_FontAttribute::operator==( const XUI_FontAttribute& rsh )const
	{ 
		return 
			name == rsh.name &&
			size == rsh.size && 
			bold == rsh.bold && 
			italic == rsh.italic && 
			antialias == rsh.antialias; 
	}

	bool XUI_FontAttribute::operator<( const XUI_FontAttribute& rsh )const
	{
		return 
			name < rsh.name?true:
			size < rsh.size?true:
			bold < rsh.bold?true:
			italic < rsh.italic?true:
			antialias < rsh.antialias;
	}

	bool XUI_FontAttribute::save_file( TiXmlElement* pNode )
	{
		TiXmlElement* pElement = pNode->ToElement();
		if( pElement )
		{
			pElement->SetAttribute( "name", name.c_str() );
			pElement->IntAttribute( "size", size );
			pElement->BoolAttribute( "bold", bold );
			pElement->BoolAttribute( "italic", italic );
			pElement->BoolAttribute( "antialias", antialias );
		}
		return true;
	}

	bool XUI_FontAttribute::load_file( TiXmlElement* pNode )
	{
		TiXmlElement* pElement = pNode->ToElement();
		if( pElement )
		{
			name = pElement->Attribute( "name" );
			pElement->Attribute( "size", &size );

			bold		= pElement->BoolAttribute( "bold" );
			italic		= pElement->BoolAttribute( "italic" );
			antialias	= pElement->BoolAttribute( "antialias" );
		}
		return true;
	}
}
