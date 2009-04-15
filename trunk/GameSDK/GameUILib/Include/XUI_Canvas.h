#pragma once
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <string>
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
#define MAX_CANDLIST		12

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

	struct XUI_IME
	{
		virtual void Render( float x, float y ) = 0;

		struct CCandList
		{
			CCandList();
			wchar_t awszCandidate[MAX_CANDLIST][256];
			std::string strBuffer;
			int32   nFirstSelected; // First character position of the selected string in HoriCand
			int32   nHoriSelectedLen; // Length of the selected string in HoriCand
			uint32	dwCount;       // Number of valid entries in the candidate list
			uint32	dwSelection;   // Currently selected candidate entry relative to page top
			uint32	dwPageSize;
			int32   nReadingError; // Index of the error character
			bool  bShowWindow;   // Whether the candidate list window is visible
			x_rect rcCandidate;   // Candidate rectangle computed and filled each time before rendered
		};

		static CCandList m_Candlist;	// 输入法绘制结构。

		static INPUTCONTEXT* (WINAPI * _ImmLockIMC)( HIMC );
		static BOOL		(WINAPI * _ImmUnlockIMC)( HIMC );
		static LPVOID	(WINAPI * _ImmLockIMCC)( HIMCC );
		static BOOL		(WINAPI * _ImmUnlockIMCC)( HIMCC );
		static BOOL		(WINAPI * _ImmDisableTextFrameService)( DWORD );
		static LONG		(WINAPI * _ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD );
		static DWORD	(WINAPI * _ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD );
		static HIMC		(WINAPI * _ImmGetContext)( HWND );
		static BOOL		(WINAPI * _ImmReleaseContext)( HWND, HIMC );
		static HIMC		(WINAPI * _ImmAssociateContext)( HWND, HIMC );
		static BOOL		(WINAPI * _ImmGetOpenStatus)( HIMC );
		static BOOL		(WINAPI * _ImmSetOpenStatus)( HIMC, BOOL );
		static BOOL		(WINAPI * _ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD );
		static HWND		(WINAPI * _ImmGetDefaultIMEWnd)( HWND );
		static UINT		(WINAPI * _ImmGetIMEFileNameA)( HKL, LPSTR, UINT );
		static UINT		(WINAPI * _ImmGetVirtualKey)( HWND );
		static BOOL		(WINAPI * _ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD );
		static BOOL		(WINAPI * _ImmSetConversionStatus)( HIMC, DWORD, DWORD );
		static BOOL		(WINAPI * _ImmSimulateHotKey)( HWND, DWORD );
		static BOOL		(WINAPI * _ImmIsIME)( HKL );

		// Function pointers: Traditional Chinese IME
		static UINT (WINAPI * _GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT );
		static BOOL (WINAPI * _ShowReadingWindow)( HIMC, BOOL );

		// Function pointers: Verion library imports
		static BOOL (APIENTRY * _VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT );
		static BOOL (APIENTRY * _GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID );
		static DWORD (APIENTRY * _GetFileVersionInfoSizeA)( LPSTR, LPDWORD );
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

	typedef bool		(*pfnSetClipping)	( int32 x, int32 y, int32 w, int32 h );
	typedef void		(*pfnDrawText)		( _lpcstr lpszText, XUI_IFont* pFont, float x, float y );
	typedef void		(*pfnDrawCharacter)	( _tchar lpszText, XUI_IFont* pFont, float x, float y );
	typedef void		(*pfnDrawRect)		( const x_rect& rc, uint32 bordercolor, uint32 backgroundcolor );	//没有边框的矩形背景
	typedef void		(*pfnDrawLine)		( float x0, float y0, float x1, float y1 );
	typedef void		(*pfnDrawPolygon)	( const x_point* ptArray, uint32* dwColorArray, uint32 nCount, uint16* pTriListArray, int32 nTriCount );
	typedef void		(*pfnDrawSprite)	( const XUI_ISprite* Tex,int nX, int nY, int nWidth, int nHeight, LPCRECT lpClipperRect );
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
}
