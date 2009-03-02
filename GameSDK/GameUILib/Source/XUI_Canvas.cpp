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

}
