#include "XUI_Canvas.h"
#include "tinyxml.h"

namespace UILib
{
	SpriteAttribute::SpriteAttribute( const char* _path, float _x, float _y, float _w, float _h )
		: path( _path )
		, x( _x )
		, y( _y )
		, w( _w )
		, h( _h )
	{
	}

	SpriteAttribute::SpriteAttribute()
		: path( "" )
		, x( 0.0f )
		, y( 0.0f )
		, w( 0.0f )
		, h( 0.0f )
	{
	}

	bool SpriteAttribute::save_file( TiXmlElement* pNode )
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

	bool SpriteAttribute::load_file( TiXmlElement* pNode )
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
	FontAttribute::FontAttribute()
		: name( _T("") )
		, size( 12 )
		, bold( false )
		, italic( false )
		, antialias( false )
	{

	}


	FontAttribute::FontAttribute( LPCTSTR lpszFont, int nSize, bool bBold, bool bItalic, bool bAntialias )
		: name( lpszFont )
		, size( nSize )
		, bold( bBold )
		, italic( bItalic )
		, antialias( bAntialias )
	{

	}

	FontAttribute::FontAttribute( const FontAttribute& src )
		: name( src.name )
		, size( src.size )
		, bold( src.bold )
		, italic( src.italic )
		, antialias( src.antialias )
	{

	}

	bool FontAttribute::operator==( const FontAttribute& rsh )const
	{ 
		return 
			name == rsh.name &&
			size == rsh.size && 
			bold == rsh.bold && 
			italic == rsh.italic && 
			antialias == rsh.antialias; 
	}

	bool FontAttribute::operator<( const FontAttribute& rsh )const
	{
		return 
			name < rsh.name?true:
			size < rsh.size?true:
			bold < rsh.bold?true:
			italic < rsh.italic?true:
			antialias < rsh.antialias;
	}

}
