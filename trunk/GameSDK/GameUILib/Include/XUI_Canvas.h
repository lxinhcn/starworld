#pragma once
#include "Commonlib.h"
#include "DataHelper.h"
class TiXmlElement;

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
#define XUI_MOUSE_CROSS				2
#define XUI_MOUSE_SIZENESW			3
#define XUI_MOUSE_SIZENS			4
#define XUI_MOUSE_SIZENWSE			5
#define XUI_MOUSE_SIZEWE			6
#define XUI_MOUSE_SIZEALL			7
#define XUI_MOUSE_WAIT				8
#define XUI_MOUSE_BUSY				9
#define XUI_MOUSE_HAND				10
#define XUI_MOUSE_HANDWRITE			11
#define XUI_MOUSE_STOP				12
#define XUI_MOUSE_HELP				13
namespace XGC
{
	namespace ui
	{
		struct xuiSpriteInfo
		{
			xuiSpriteInfo( _lpcstr _path, float _x, float _y, float _w, float _h );
			xuiSpriteInfo();

			bool operator==( const xuiSpriteInfo& rsh )const;
			bool operator<( const xuiSpriteInfo& rsh )const;

			bool save_file( TiXmlElement* pNode );
			bool load_file( TiXmlElement* pNode );

			_astring	path;
			float		x, y, w, h;
		};

		struct xuiAnimationInfo
		{
			_astring	key;
			_uint32		frames;
			float		fps;
			float		x, y, w, h;
		};

		struct xuiFontInfo
		{
			_astring		m_name;
			_uint32			m_size;
			_uint32			m_color;
			bool			m_bold, m_italic, m_antialias;

			xuiFontInfo();
			xuiFontInfo( const char* lpszFont, _int32 nSize, _uint32 dwColor, bool bBold, bool bItalic, bool bAntialias );
			xuiFontInfo( const xuiFontInfo& src );

			bool operator==( const xuiFontInfo& rsh )const;
			bool operator<( const xuiFontInfo& rsh )const;

			const char* name()const{ return m_name.c_str(); }
			int			size()const{ return m_size; }

			bool save_file( TiXmlElement* pNode );
			bool load_file( TiXmlElement* pNode );
		};

		//struct XUI_ISprite	:	public	xuiSpriteInfo
		//{
		//	virtual ~XUI_ISprite(){}
		//	virtual float	GetWidth()const		= 0;
		//	virtual float	GetHeight()const	= 0;

		//	virtual void	SetCenter( float x, float y ) = 0;
		//	virtual void	SetUV(float U0, float V0, float U1, float V1) = 0;
		//	virtual float	GetU0() const = 0;
		//	virtual float	GetV0() const = 0;
		//	virtual float	GetU1() const = 0;
		//	virtual float	GetV1() const = 0;

		//	virtual void	SetAlpha( float fAlpha )	= 0;
		//	virtual float	GetAlpha()const				= 0;

		//	virtual void	SetAngle( float fAngle )	= 0;
		//	virtual float	GetAngle()const				= 0;

		//	virtual void	SetDiffuse( float dwValue ) = 0;
		//	virtual float	GetDiffuse() const			= 0;

		//	virtual void	Release()					= 0;
		//	virtual void	Render( float x, float y )	= 0;
		//	virtual void	RenderStretch( float x, float y, float w, float h ) = 0;
		//	virtual void	RenderEx( float x, float y, float rot, float hscale, float vscale ) = 0;
		//};

		//struct XUI_IAnimation
		//{
		//	virtual ~XUI_IAnimation(){}
		//	virtual void Play() = 0;
		//	virtual void Stop() = 0;
		//	virtual void Resume() = 0;
		//	virtual void Update( float fDelta ) = 0;
		//	virtual void Render( int x, int y ) = 0;

		//	virtual void	SetCurrentFrame( int nFrame ) = 0;
		//	virtual int		GetCurrentFrame()const = 0;

		//	virtual void	SetSpeed( float fFPS ) = 0;
		//	virtual float	GetSpeed()const = 0;

		//	virtual void	SetFrames( int nFrames ) = 0;
		//	virtual int		GetFrames()const = 0;

		//	virtual void	SetCenter( float x, float y ) = 0;
		//	virtual void	GetCenter( float &x, float &y )const = 0;
		//};

		//struct XUI_Font	:	protected	xuiFontInfo
		//{
		//	XUI_Font(){}
		//	XUI_Font( const xuiFontInfo& FontAttrib ): xuiFontInfo( FontAttrib ){}
		//	virtual ~XUI_Font(){}

		//	virtual VOID SetColor( _uint32 color ) = 0;
		//	virtual iSize GetStringSize( _lpctstr lpszString ) = 0;

		//	virtual int GetCharacterWidth( _tchar szChar ) = 0;
		//	virtual int GetCharacterHeight() = 0;
		//};

		typedef void* xuiFont;
		typedef void* xuiSprite;
		typedef void* xuiAnimation;

		//struct __declspec(novtable) XUI_Input
		//{
		//	virtual void	GetMousePos( int *x, int *y ) = 0;
		//	virtual void	SetMousePos( int x, int y ) = 0;
		//	virtual _int32	GetMouseWheel() = 0;
		//	virtual void	RenderMouse() = 0;
		//	virtual void	SetMouse( _uint16 id ) = 0;
		//	virtual bool	GetKeyState( int key )const = 0;
		//	virtual bool	IsPressedLButton()const = 0;
		//	virtual bool	IsReleaseLButton()const = 0;
		//	virtual bool	IsPressedRButton()const = 0;
		//	virtual bool	IsReleaseRButton()const = 0;
		//	virtual bool	IsPressedMButton()const = 0;
		//	virtual bool	IsReleaseMButton()const = 0;
		//	virtual bool	IsMouseOver()const = 0;
		//	virtual void	UpdateMouse( float fDeltaTime ) = 0;
		//};

		typedef void			(*pfnSetClipping)		( int x, int y, int w, int h );
		typedef void			(*pfnDrawText)			( _lpcwstr text, xuiFont font, int x, int y );
		typedef void			(*pfnDrawCharacter)		( _wchar text, xuiFont font, int x, int y );
		typedef void			(*pfnDrawRect)			( const iRect& rc, _uint32 bordercolor, _uint32 backgroundcolor );	//没有边框的矩形背景
		typedef void			(*pfnDrawLine)			( float x0, float y0, float x1, float y1, _uint32 color );
		typedef void			(*pfnDrawPolygon)		( const iPoint* points, _uint32* colors, _uint32 count, _uint16* trilist, int tricount );
		typedef void			(*pfnDrawSprite)		( const xuiSprite sprite, int x, int y, int width, int height );
		typedef xuiSprite		(*pfnCreateSprite)		( _lpcstr filename, float x, float y, float w, float h );
		typedef void			(*pfnDestroySprite)		( xuiSprite sprite );
		typedef xuiAnimation	(*pfnCreateAnimation)	( _lpcstr filename, int frames, float fps, float x, float y, float w, float h );
		typedef void			(*pfnDestroyAnimation)	( xuiAnimation animation );
		typedef xuiFont			(*pfnCreateFont)		( _lpcstr fontname, int size, _uint32 color, bool bold, bool italic, bool antialias );
		typedef void			(*pfnDestroyFont)		( xuiFont font );
		typedef int				(*pfnGetCharacterWidth) ( xuiFont font, _tchar character );
		typedef int				(*pfnGetCharacterHeight)( xuiFont font );
		typedef int				(*pfnGetStringWidth)	( xuiFont font, _lpctstr text );

		extern pfnSetClipping			XUI_SetClipping;
		extern pfnDrawText				XUI_DrawText;
		extern pfnDrawCharacter			XUI_DrawCharacter;
		extern pfnDrawRect				XUI_DrawRect;
		extern pfnDrawPolygon			XUI_DrawPolygon;
		extern pfnDrawLine				XUI_DrawLine;
		extern pfnDrawSprite			XUI_DrawSprite;
		extern pfnCreateSprite			XUI_CreateSprite;
		extern pfnCreateAnimation		XUI_CreateAnimation;
		extern pfnDestroySprite			XUI_DestroySprite;
		extern pfnDestroyAnimation		XUI_DestroyAnimation;
		extern pfnCreateFont			XUI_CreateFont;
		extern pfnDestroyFont			XUI_DestroyFont;
		extern pfnGetCharacterWidth		XUI_GetCharacterWidth;
		extern pfnGetCharacterHeight	XUI_GetCharacterHeight;
		extern pfnGetStringWidth		XUI_GetStringWidth;

		extern void XUI_DrawTextA( _lpcstr lpszText, xuiFont pFont, int x, int y );
		extern void XUI_DrawCharacterA( const char* szMbs, xuiFont pFont, int x, int y );
	}
}
