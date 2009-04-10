#pragma once
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <string>
#include "defines.h"
#include "DataHelper.h"
class TiXmlElement;
using namespace XGC;
namespace UILib
{
	struct XUI_SpriteAttribute
	{
		XUI_SpriteAttribute( _lpcstr _path, float _x, float _y, float _w, float _h );
		XUI_SpriteAttribute();

		bool operator==( const XUI_SpriteAttribute& rsh )const;
		bool operator<( const XUI_SpriteAttribute& rsh )const;

		bool save_file( TiXmlElement* pNode );
		bool load_file( TiXmlElement* pNode );

		std::string	path;
		float		x, y, w, h;
	};

	struct XUI_FontAttribute
	{
		std::string		name;
		int				size;
		bool			bold, italic, antialias;

		XUI_FontAttribute();
		XUI_FontAttribute( const char* lpszFont, int nSize, bool bBold, bool bItalic, bool bAntialias );
		XUI_FontAttribute( const XUI_FontAttribute& src );

		bool operator==( const XUI_FontAttribute& rsh )const;
		bool operator<( const XUI_FontAttribute& rsh )const;

		const char* GetFontName()const{ return name.c_str(); }
		int			GetSize()const{ return size; }

		bool save_file( TiXmlElement* pNode );
		bool load_file( TiXmlElement* pNode );
	};

	struct XUI_IMouse
	{
		virtual ~XUI_IMouse(){}
		virtual void	GetMousePos( float *x, float *y ) = 0;
		virtual void	SetMousePos( float x, float y ) = 0;
		virtual int32	GetMouseWheel() = 0;
		virtual void	RenderMouse() = 0;
		virtual bool	IsPressedLButton()const = 0;
		virtual bool	IsReleaseLButton()const = 0;
		virtual bool	IsPressedRButton()const = 0;
		virtual bool	IsReleaseRButton()const = 0;
		virtual bool	IsPressedMButton()const = 0;
		virtual bool	IsReleaseMButton()const = 0;
		virtual bool	IsMouseOver()const = 0;
	};

	struct XUI_ISprite	:	protected	XUI_SpriteAttribute
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
		virtual void	Render( float x, float y )	= 0;
		virtual void	RenderStretch( float x, float y, float w, float h ) = 0;
		virtual void	RenderEx( float x, float y, float rot, float hscale, float vscale ) = 0;
	};

	struct XUI_IFont	:	protected	XUI_FontAttribute
	{
		XUI_IFont(){}
		XUI_IFont( const XUI_FontAttribute& FontAttrib ): XUI_FontAttribute( FontAttrib ){}
		virtual ~XUI_IFont(){}

		virtual VOID SetColor( uint32 color ) = 0;
		virtual SIZE GetStringSize( _lpctstr lpszString ) = 0;

		virtual INT GetCharacterWidth( _tchar szChar ) = 0;
		virtual INT	GetCharacterHeight() = 0;
	};

	typedef bool		(*pfnDrawText)		( _lpcstr lpszText, XUI_IFont* pFont, float x, float y );
	typedef bool		(*pfnDrawCharacter)	( _tchar lpszText, XUI_IFont* pFont, float x, float y );
	typedef bool		(*pfnDrawRect)		( const CRect& rcDest, uint32 dwBorderColor, uint32 dwBkColor );	//没有边框的矩形背景
	typedef bool		(*pfnDrawPolygon)	( const CPoint* ptArray, uint32* dwColorArray, uint32 nCount, uint16* pTriListArray, int32 nTriCount );
	typedef bool		(*pfnDrawSprite)	( const XUI_ISprite* Tex,int nX, int nY, int nWidth, int nHeight, LPCRECT lpClipperRect );
	typedef XUI_ISprite*(*pfnCreateSprite)	( _lpcstr filename, float x, float y, float w, float h );
	typedef XUI_ISprite*(*pfnCreateSpriteEx)( const XUI_SpriteAttribute& SpriteAttribute );
	typedef void		(*pfnDestroySprite)	( XUI_ISprite* pSprite );
	typedef XUI_IFont*	(*pfnCreateFont)	( _lpcstr lpszFontName, int nSize, bool bBold, bool bItalic, bool bAntialias );
	typedef XUI_IFont*	(*pfnCreateFontEx)	( const XUI_FontAttribute& FontAttribute );
	typedef void		(*pfnDestroyFont)	( XUI_IFont* pFont );

	extern pfnDrawText			XUI_DrawText;
	extern pfnDrawCharacter		XUI_DrawCharacter;
	extern pfnDrawRect			XUI_DrawRect;
	extern pfnDrawPolygon		XUI_DrawPolygon;
	extern pfnDrawSprite		XUI_DrawSprite;
	extern pfnCreateSprite		XUI_CreateSprite;
	extern pfnCreateSpriteEx	XUI_CreateSpriteEx;
	extern pfnDestroySprite		XUI_DestroySprite;
	extern pfnCreateFont		XUI_CreateFont;
	extern pfnCreateFontEx		XUI_CreateFontEx;
	extern pfnDestroyFont		XUI_DestroyFont;
}
