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
		std::string		name;
		int				size;
		bool			bold, italic, antialias;

		FontAttribute();
		FontAttribute( const char* lpszFont, int nSize, bool bBold, bool bItalic, bool bAntialias );
		FontAttribute( const FontAttribute& src );

		bool operator==( const FontAttribute& rsh )const;
		bool operator<( const FontAttribute& rsh )const;

		const char* GetFontName()const{ return name.c_str(); }
		int			GetSize()const{ return size; }

		bool save_file( TiXmlElement* pNode );
		bool load_file( TiXmlElement* pNode );
	};

	struct XUI_ISprite	:	protected	SpriteAttribute
	{
		virtual ~XUI_ISprite(){}
		virtual float	GetWidth()const		= 0;
		virtual float	GetHeight()const	= 0;

		virtual void	SetUV(float U0, float V0, float U1, float V1) = 0;
		virtual float	GetU0() const = 0;
		virtual float	GetV0() const = 0;
		virtual float	GetU1() const = 0;
		virtual float	GetV1() const = 0;

		virtual void	SetAlpha( float fAlpha )	= 0;
		virtual float	GetAlpha()const				= 0;

		virtual void	SetAngle( float fAngle )	= 0;
		virtual float	GetAngle()const				= 0;

		virtual void	SetDiffuse( float dwValue ) = 0;
		virtual float	GetDiffuse() const			= 0;

		virtual void	Release()					= 0;
	};

	struct XUI_IFont	:	protected	FontAttribute
	{
		XUI_IFont(){}
		XUI_IFont( const FontAttribute& FontAttrib ): FontAttribute( FontAttrib ){}
		virtual ~XUI_IFont(){}
		virtual SIZE GetStringSize( LPCTSTR lpszString ) = 0;

		virtual INT GetCharacterWidth( TCHAR szChar ) = 0;
		virtual INT	GetCharacterHeight() = 0;
	};

	typedef bool		(*pfnDrawText)		( LPCTSTR lpszText, XUI_IFont* pFont, float x, float y );
	typedef bool		(*pfnDrawCharacter)	( TCHAR lpszText, XUI_IFont* pFont, float x, float y );
	typedef bool		(*pfnDrawRect)		( const RECT& rcDest, DWORD dwBorderColor, DWORD dwBkColor );	//没有边框的矩形背景
	typedef bool		(*pfnDrawPolygon)	( const LPPOINT ptArray, DWORD* dwColorArray, int nCount, unsigned short* pTriListArray, int nTriCount );
	typedef bool		(*pfnDrawSprite)	( const XUI_ISprite* Tex,int nX, int nY, int nWidth, int nHeight, LPCRECT lpClipperRect );
	typedef XUI_ISprite*(*pfnCreateSprite)	( LPCTSTR lpszTexpath, float x, float y, float w, float h );
	typedef void		(*pfnDestroySprite)	( XUI_ISprite* pSprite );
	typedef XUI_IFont*	(*pfnCreateFont)	( LPCTSTR lpszFontName, int nSize, bool bBold, bool bItalic, bool bAntialias );
	typedef void		(*pfnDestroyFont)	( XUI_IFont* pFont );

	extern pfnDrawText			XUI_DrawText;
	extern pfnDrawCharacter		XUI_DrawCharacter;
	extern pfnDrawRect			XUI_DrawRect;
	extern pfnDrawPolygon		XUI_DrawPolygon;
	extern pfnDrawSprite		XUI_DrawSprite;
	extern pfnCreateSprite		XUI_CreateSprite;
	extern pfnDestroySprite		XUI_DestroySprite;
	extern pfnCreateFont		XUI_CreateFont;
	extern pfnDestroyFont		XUI_DestroyFont;
}
