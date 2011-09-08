#include "XUI_Header.h"
#include "XUI_Canvas.h"
#include "tinyxml.h"

namespace XGC
{
	namespace ui
	{
		pfnSetClipping			XUI_SetClipping			= 0;	//   XUI_SetClipping( int x, int y, int w, int h );
		pfnDrawText				XUI_DrawText			= 0;	//   XUI_DrawText( _lpcstr lpszText, xuiFont pFont, float x, float y );
		pfnDrawCharacter		XUI_DrawCharacter		= 0;	//   XUI_DrawCharacter( _tchar lpszText, xuiFont pFont, float x, float y );
		pfnDrawRect				XUI_DrawRect			= 0;	//   XUI_DrawRect( const iRect& rcDest, _uint32 dwBorderColor, _uint32 dwBkColor );
		pfnDrawPolygon			XUI_DrawPolygon			= 0;	//   XUI_DrawPolygon( const iPoint* ptArray, _uint32* dwColorArray, _uint32 nCount, uint16* pTriListArray, int32 nTriCount );
		pfnDrawLine				XUI_DrawLine			= 0;	//   XUI_DrawLine( float x0, float y0, float x1, float y1 );
		pfnDrawSprite			XUI_DrawSprite			= 0;	//   XUI_DrawSprite( const xuiSprite Tex,int nX, int nY, int nWidth, int nHeight, LPCRECT lpClipperRect );
		pfnCreateSprite			XUI_CreateSprite		= 0;	//   XUI_CreateSprite( _lpcstr filename, float x, float y, float w, float h );
		pfnDestroySprite		XUI_DestroySprite		= 0;	//   XUI_DestroySprite( xuiSprite pSprite );
		pfnCreateAnimation		XUI_CreateAnimation		= 0;	//	 XUI_CreateAnimation( _lpcstr filename, int frames, float fps, float x, float y, float w, float h );
		pfnDestroyAnimation		XUI_DestroyAnimation	= 0;	//	 XUI_DestroyAnimation( XUI_IAnimation* pAnimation );
		pfnCreateFont			XUI_CreateFont			= 0;	//   XUI_CreateFont( _lpcstr lpszFontName, int nSize, bool bBold, bool bItalic, bool bAntialias );
		pfnDestroyFont			XUI_DestroyFont			= 0;	//   XUI_DestroyFont( xuiFont pFont );
		pfnGetCharacterWidth	XUI_GetCharacterWidth	= 0;
		pfnGetCharacterHeight	XUI_GetCharacterHeight	= 0;
		pfnGetStringWidth		XUI_GetStringWidth		= 0;

		void XUI_DrawTextA( _lpcstr text, xuiFont font, int x, int y )
		{
			_string strText = XA2T( text );
			XUI_DrawText( strText.c_str(), font, x, y );
		}

		void XUI_DrawCharacterA( const char* szMbs, xuiFont font, int x, int y )
		{
			_wchar sz;
			if( mbtowc( &sz, szMbs, MB_CUR_MAX ) >= 0 )
			{
				XUI_DrawCharacter( sz, font, x, y );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// Sprite Attribute
		//////////////////////////////////////////////////////////////////////////
		xuiSpriteInfo::xuiSpriteInfo( _lpcstr _path, float _x, float _y, float _w, float _h )
			: path( _path )
			, x( _x )
			, y( _y )
			, w( _w )
			, h( _h )
		{
		}

		xuiSpriteInfo::xuiSpriteInfo()
			: path( "" )
			, x( 0.0f )
			, y( 0.0f )
			, w( 0.0f )
			, h( 0.0f )
		{
		}

		bool xuiSpriteInfo::save_file( TiXmlElement* pNode )
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

		bool xuiSpriteInfo::load_file( TiXmlElement* pNode )
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

		bool xuiSpriteInfo::operator==( const xuiSpriteInfo& rsh )const
		{
			return 
				path == rsh.path &&
				x == rsh.x && 
				y == rsh.y &&
				w == rsh.w &&
				h == rsh.h;
		}

		bool xuiSpriteInfo::operator<( const xuiSpriteInfo& rsh )const
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
		xuiFontInfo::xuiFontInfo()
			: m_name( "" )
			, m_size( 12 )
			, m_bold( false )
			, m_italic( false )
			, m_antialias( false )
		{

		}


		xuiFontInfo::xuiFontInfo( const char* lpszFont, _int32 nSize, _uint32 dwColor, bool bBold, bool bItalic, bool bAntialias )
			: m_name( lpszFont )
			, m_size( nSize )
			, m_color( dwColor )
			, m_bold( bBold )
			, m_italic( bItalic )
			, m_antialias( bAntialias )
		{

		}

		xuiFontInfo::xuiFontInfo( const xuiFontInfo& src )
			: m_name( src.m_name )
			, m_size( src.m_size )
			, m_color( src.m_color )
			, m_bold( src.m_bold )
			, m_italic( src.m_italic )
			, m_antialias( src.m_antialias )
		{

		}

		bool xuiFontInfo::operator==( const xuiFontInfo& rsh )const
		{ 
			return 
				m_name == rsh.m_name &&
				m_size == rsh.m_size && 
				m_bold == rsh.m_bold && 
				m_italic == rsh.m_italic && 
				m_antialias == rsh.m_antialias; 
		}

		bool xuiFontInfo::operator<( const xuiFontInfo& rsh )const
		{
			return 
				m_name < rsh.m_name?true:
				m_size < rsh.m_size?true:
				m_bold < rsh.m_bold?true:
				m_italic < rsh.m_italic?true:
				m_antialias < rsh.m_antialias;
		}

		bool xuiFontInfo::save_file( TiXmlElement* pNode )
		{
			pNode->SetAttribute( "class", "font" );
			if( pNode )
			{
				pNode->SetAttribute( "name", m_name.c_str() );
				pNode->IntAttribute( "size", m_size );
				pNode->IntAttribute( "color", m_color );
				pNode->BoolAttribute( "bold", m_bold );
				pNode->BoolAttribute( "italic", m_italic );
				pNode->BoolAttribute( "antialias", m_antialias );
			}
			return true;
		}

		bool xuiFontInfo::load_file( TiXmlElement* pNode )
		{
			TiXmlElement* pElement = pNode->ToElement();
			if( pElement )
			{
				m_name = pElement->Attribute( "name" );
				pElement->Attribute( "size", (int*)&m_size );

				m_color		= pElement->IntAttribute( "color" );
				m_bold		= pElement->BoolAttribute( "bold" );
				m_italic	= pElement->BoolAttribute( "italic" );
				m_antialias	= pElement->BoolAttribute( "antialias" );
			}
			return true;
		}
	}
}
