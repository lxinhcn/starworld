#pragma once
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <string>
#include <objbase.h>
#include <Dimm.h>
#include "defines.h"
#include "DataHelper.h"


class TiXmlElement;
using namespace XGC;

#define XUI_ARGB(a,r,g,b)	((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b))
#define XUI_GETA(col)		((col)>>24)
#define XUI_GETR(col)		(((col)>>16) & 0xFF)
#define XUI_GETG(col)		(((col)>>8) & 0xFF)
#define XUI_GETB(col)		((col) & 0xFF)
#define XUI_SETA(col,a)		(((col) & 0x00FFFFFF) + (DWORD(a)<<24))
#define XUI_SETR(col,r)		(((col) & 0xFF00FFFF) + (DWORD(r)<<16))
#define XUI_SETG(col,g)		(((col) & 0xFFFF00FF) + (DWORD(g)<<8))
#define XUI_SETB(col,b)		(((col) & 0xFFFFFF00) + DWORD(b))

#define XUI_MOUSE_APPSTARTING		0
#define XUI_MOUSE_ARROW				0
#define XUI_MOUSE_TEXT				1
#define XUI_MOUSE_MOVE				2
#define XUI_MOUSE_SIZENESW			3
#define XUI_MOUSE_SIZENS			4
#define XUI_MOUSE_SIZENWSE			5
#define XUI_MOUSE_SIZEWE			6
#define XUI_MOUSE_WAIT				7

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
		virtual void	SetMouse( uint32 id ) = 0;
		virtual bool	IsPressedLButton()const = 0;
		virtual bool	IsReleaseLButton()const = 0;
		virtual bool	IsPressedRButton()const = 0;
		virtual bool	IsReleaseRButton()const = 0;
		virtual bool	IsPressedMButton()const = 0;
		virtual bool	IsReleaseMButton()const = 0;
		virtual bool	IsMouseOver()const = 0;
	};

	struct XUI_ISprite	:	public	XUI_SpriteAttribute
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

	typedef void		(*pfnSetClipping)	( int32 x, int32 y, int32 w, int32 h );
	typedef void		(*pfnDrawText)		( _lpcwstr lpszText, XUI_IFont* pFont, float x, float y );
	typedef void		(*pfnDrawCharacter)	( _wchar lpszText, XUI_IFont* pFont, float x, float y );
	typedef void		(*pfnDrawRect)		( const x_rect& rc, uint32 bordercolor, uint32 backgroundcolor );	//没有边框的矩形背景
	typedef void		(*pfnDrawLine)		( float x0, float y0, float x1, float y1 );
	typedef void		(*pfnDrawPolygon)	( const x_point* ptArray, uint32* dwColorArray, uint32 nCount, uint16* pTriListArray, int32 nTriCount );
	typedef void		(*pfnDrawSprite)	( const XUI_ISprite* Tex,int nX, int nY, int nWidth, int nHeight );
	typedef XUI_ISprite*(*pfnCreateSprite)	( _lpcstr filename, float x, float y, float w, float h );
	typedef XUI_ISprite*(*pfnCreateSpriteEx)( const XUI_SpriteAttribute& SpriteAttribute );
	typedef void		(*pfnDestroySprite)	( XUI_ISprite* pSprite );
	typedef XUI_IFont*	(*pfnCreateFont)	( _lpcstr lpszFontName, int nSize, bool bBold, bool bItalic, bool bAntialias );
	typedef XUI_IFont*	(*pfnCreateFontEx)	( const XUI_FontAttribute& FontAttribute );
	typedef void		(*pfnDestroyFont)	( XUI_IFont* pFont );
	typedef LRESULT		(CALLBACK *pfnDefWindowProc)( __in HWND hWnd, __in UINT Msg, __in WPARAM wParam, __in LPARAM lParam);

	extern pfnSetClipping		XUI_SetClipping;
	extern pfnDrawText			XUI_DrawText;
	extern pfnDrawCharacter		XUI_DrawCharacter;
	extern pfnDrawRect			XUI_DrawRect;
	extern pfnDrawPolygon		XUI_DrawPolygon;
	extern pfnDrawLine			XUI_DrawLine;
	extern pfnDrawSprite		XUI_DrawSprite;
	extern pfnCreateSprite		XUI_CreateSprite;
	extern pfnCreateSpriteEx	XUI_CreateSpriteEx;
	extern pfnDestroySprite		XUI_DestroySprite;
	extern pfnCreateFont		XUI_CreateFont;
	extern pfnCreateFontEx		XUI_CreateFontEx;
	extern pfnDestroyFont		XUI_DestroyFont;
	extern pfnDefWindowProc		XUI_DefWindowProc;

	extern void XUI_DrawTextA( _lpcstr lpszText, XUI_IFont* pFont, float x, float y );
	extern void XUI_DrawCharacterA( const char* szMbs, XUI_IFont* pFont, float x, float y );
}
