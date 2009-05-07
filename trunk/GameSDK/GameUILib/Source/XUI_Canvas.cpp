#include "XUI_Canvas.h"
#include "Helplib.h"
#include "tinyxml.h"

namespace UILib
{
	pfnSetClipping		XUI_SetClipping		= 0;	//   XUI_SetClipping( int x, int y, int w, int h );
	pfnDrawText			XUI_DrawText		= 0;	//   XUI_DrawText( _lpcstr lpszText, XUI_IFont* pFont, float x, float y );
	pfnDrawCharacter	XUI_DrawCharacter	= 0;	//   XUI_DrawCharacter( _tchar lpszText, XUI_IFont* pFont, float x, float y );
	pfnDrawRect			XUI_DrawRect		= 0;	//   XUI_DrawRect( const x_rect& rcDest, uint32 dwBorderColor, uint32 dwBkColor );
	pfnDrawPolygon		XUI_DrawPolygon		= 0;	//   XUI_DrawPolygon( const x_point* ptArray, uint32* dwColorArray, uint32 nCount, uint16* pTriListArray, int32 nTriCount );
	pfnDrawLine			XUI_DrawLine		= 0;	//   XUI_DrawLine( float x0, float y0, float x1, float y1 );
	pfnDrawSprite		XUI_DrawSprite		= 0;	//   XUI_DrawSprite( const XUI_ISprite* Tex,int nX, int nY, int nWidth, int nHeight, LPCRECT lpClipperRect );
	pfnCreateSprite		XUI_CreateSprite	= 0;	//   XUI_CreateSprite( _lpcstr filename, float x, float y, float w, float h );
	pfnCreateSpriteEx	XUI_CreateSpriteEx	= 0;	//   XUI_CreateSpriteEx( const XUI_SpriteAttribute& SpriteAttribute );
	pfnDestroySprite	XUI_DestroySprite	= 0;	//   XUI_DestroySprite( XUI_ISprite* pSprite );
	pfnCreateFont		XUI_CreateFont		= 0;	//   XUI_CreateFont( _lpcstr lpszFontName, int nSize, bool bBold, bool bItalic, bool bAntialias );
	pfnCreateFontEx		XUI_CreateFontEx	= 0;	//   XUI_CreateFontEx( const XUI_FontAttribute& FontAttribute );
	pfnDestroyFont		XUI_DestroyFont		= 0;	//   XUI_DestroyFont( XUI_IFont* pFont );
	pfnDefWindowProc	XUI_DefWindowProc	= 0;	//   XUI_DefWindowProc( __in HWND hWnd, __in UINT Msg, __in WPARAM wParam, __in LPARAM lParam);

	void XUI_DrawTextA( _lpcstr lpszText, XUI_IFont* pFont, float x, float y )
	{
		XUI_DrawText( XA2W(lpszText), pFont, x, y );
	}

	void XUI_DrawCharacterA( const char* szMbs, XUI_IFont* pFont, float x, float y )
	{
		_wchar sz;
		if( mbtowc( &sz, szMbs, MB_CUR_MAX ) >= 0 )
		{
			XUI_DrawCharacter( sz, pFont, x, y );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Sprite Attribute
	//////////////////////////////////////////////////////////////////////////
	XUI_SpriteAttribute::XUI_SpriteAttribute( _lpcstr _path, float _x, float _y, float _w, float _h )
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
		pNode->SetAttribute( "class", "sprite" );
		TiXmlElement p( "path" );
		p.SetAttribute( "direction", path.c_str() );
		pNode->InsertEndChild( p );

		TiXmlElement Position( "position" );
		Position.SetDoubleAttribute( "x", x );
		Position.SetDoubleAttribute( "y", y );
		pNode->InsertEndChild( Position );

		TiXmlElement Size( "size" );
		Size.SetDoubleAttribute( "w", w );
		Size.SetDoubleAttribute( "h", h );
		pNode->InsertEndChild( Size );

		return true;
	}

	bool XUI_SpriteAttribute::load_file( TiXmlElement* pNode )
	{
		TiXmlElement* pElement = pNode->FirstChildElement( "path" );
		path = pElement->Attribute( "direction" );

		pElement = pNode->FirstChildElement( "position" );
		ASSERT( pElement );
		if( pElement )
		{
			pElement->QueryFloatAttribute( "x", &x );
			pElement->QueryFloatAttribute( "y", &y );
		}

		pElement = pNode->FirstChildElement( "size" );
		ASSERT(pElement);
		if( pElement )
		{
			pElement->QueryFloatAttribute( "w", &w );
			pElement->QueryFloatAttribute( "h", &h );
		}

		return true;
	}

	bool XUI_SpriteAttribute::operator==( const XUI_SpriteAttribute& rsh )const
	{
		return 
			path == rsh.path &&
			x == rsh.x && 
			y == rsh.y &&
			w == rsh.w &&
			h == rsh.h;
	}

	bool XUI_SpriteAttribute::operator<( const XUI_SpriteAttribute& rsh )const
	{
		return 
			path < rsh.path?true:
			x < rsh.x?true: 
			y < rsh.y?true:
			w < rsh.w?true:
			h < rsh.h;
	}

	//////////////////////////////////////////////////////////////////////////
	// Font Attribute
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
		pNode->SetAttribute( "class", "font" );
		if( pNode )
		{
			pNode->SetAttribute( "name", name.c_str() );
			pNode->IntAttribute( "size", size );
			pNode->BoolAttribute( "bold", bold );
			pNode->BoolAttribute( "italic", italic );
			pNode->BoolAttribute( "antialias", antialias );
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
