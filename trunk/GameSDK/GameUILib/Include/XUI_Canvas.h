#pragma once
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <string>
#include "GuiMacroDefine.h"
class TiXmlElement;
namespace UILib
{
	struct SpriteAttribute
	{
		SpriteAttribute( const char* _path, float _x, float _y, float _w, float _h );
		SpriteAttribute();

		bool save_file( TiXmlElement* pNode );
		bool load_file( TiXmlElement* pNode );

		std::string	path;
		float		x, y, w, h;
	};

	struct FontAttribute
	{
		_string			name;
		int				size;
		bool			bold, italic, antialias;

		FontAttribute()
			: name( _T("") )
			, size( 12 )
			, bold( false )
			, italic( false )
			, antialias( false )
		{

		}


		FontAttribute( LPCTSTR lpszFont, int nSize, bool bBold, bool bItalic, bool bAntialias )
			: name( lpszFont )
			, size( nSize )
			, bold( bBold )
			, italic( bItalic )
			, antialias( bAntialias )
		{

		}

		FontAttribute( const FontAttribute& src )
			: name( src.name )
			, size( src.size )
			, bold( src.bold )
			, italic( src.italic )
			, antialias( src.antialias )
		{

		}

		bool operator==( const FontAttribute& rsh )const
		{ 
			return 
				name == rsh.name &&
				size == rsh.size && 
				bold == rsh.bold && 
				italic == rsh.italic && 
				antialias == rsh.antialias; 
		}

		bool operator<( const FontAttribute& rsh )const
		{
			return 
				name < rsh.name?true:
				size < rsh.size?true:
				bold < rsh.bold?true:
				italic < rsh.italic?true:
				antialias < rsh.antialias;
		}

		LPCTSTR GetFontName()const{ return name.c_str(); }
		int		GetSize()const{ return size; }
	};

	struct XUI_ISprite	:	protected	SpriteAttribute
	{
		virtual ~XUI_ISprite(){}
		virtual DWORD	GetWidth()const		= 0;
		virtual DWORD	GetHeight()const	= 0;

		virtual void	SetUV(float U0, float V0, float U1, float V1) = 0;
		virtual float	GetU0() const = 0;
		virtual float	GetV0() const = 0;
		virtual float	GetU1() const = 0;
		virtual float	GetV1() const = 0;

		virtual void	SetAlpha( float fAlpha )	= 0;
		virtual float	GetAlpha()const				= 0;

		virtual void	SetAngle( float fAngle )	= 0;
		virtual float	GetAngle()const				= 0;

		virtual void	SetDiffuse( DWORD dwValue ) = 0;
		virtual DWORD	GetDiffuse() const			= 0;

		virtual void	Release()					= 0;
	};

	struct XUI_IFont	:	protected	FontAttribute
	{
		XUI_IFont(){}
		XUI_IFont( const FontAttribute& FontAttrib ): FontAttribute( FontAttrib ){}
		virtual ~XUI_IFont(){}
		virtual INT		Ge_stringWidth( LPCTSTR lpszString )const = 0;
		virtual INT		Ge_stringHeight( LPCTSTR lpszString )const = 0;
	};

	typedef bool		(*pfnDrawText)		( LPCTSTR lpszText, XUI_IFont* pFont, int nX, int nY, DWORD dwColor, LPCRECT lpRect );
	typedef bool		(*pfnDrawRect)		( const RECT& rcDest, DWORD dwBorderColor, DWORD dwBkColor );	//没有边框的矩形背景
	typedef bool		(*pfnDrawPolygon)	( const LPPOINT ptArray, DWORD* dwColorArray, int nCount, unsigned short* pTriListArray, int nTriCount );
	typedef bool		(*pfnDrawSprite)	( const XUI_ISprite* Tex,int nX, int nY, int nWidth, int nHeight, LPCRECT lpClipperRect );
	typedef XUI_ISprite*(*pfnCreateSprite)	( LPCTSTR lpszTexpath, float x, float y, float w, float h );
	typedef void		(*pfnDestroySprite)	( XUI_ISprite* pSprite );
	typedef XUI_IFont*	(*pfnCreateFont)	( LPCTSTR lpszFontName, int nSize, bool bBold, bool bItalic, bool bAntialias );
	typedef void		(*pfnDestroyFont)	( XUI_IFont* pFont );

	extern pfnDrawText			XUI_DrawText;
	extern pfnDrawRect			XUI_DrawRect;
	extern pfnDrawPolygon		XUI_DrawPolygon;
	extern pfnDrawSprite		XUI_DrawSprite;
	extern pfnCreateSprite		XUI_CreateSprite;
	extern pfnDestroySprite		XUI_DestroySprite;
	extern pfnCreateFont		XUI_CreateFont;
	extern pfnDestroyFont		XUI_DestroyFont;
}
