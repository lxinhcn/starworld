#pragma once
#include "GfxFont.h"
#include <hash_map>
using namespace XGC::ui;

class CTextureManager
{
friend struct CreateStatic< CTextureManager >;
private:
	CTextureManager();
	~CTextureManager();

public:
	//---------------------------------------------------------------------//
	// describe	: 得到路径指定的纹理句柄
	// return	: 纹理句柄
	//---------------------------------------------------------------------//
	HTEXTURE GetTexture( _lpcstr key, _lpstr data = 0, size_t size = 0 );

	//---------------------------------------------------------------------//
	// describe	: 创建纹理
	// return	: 纹理句柄
	//---------------------------------------------------------------------//
	HTEXTURE CreateTexture( _lpcstr key, int w, int h );

	//---------------------------------------------------------------------//
	// describe	: 清除所有资源
	// return	:
	//---------------------------------------------------------------------//
	void Clear();

private:
	typedef stdext::hash_map< std::string, HTEXTURE >	CTextureMap;
	CTextureMap		m_TextureMap;	// 纹理映射
};
typedef SingletonHolder< CTextureManager, CreateStatic >	TextureManager;

class CFontManager
{
friend struct CreateStatic< CFontManager >;
private:
	CFontManager();
	~CFontManager();

public:
	GfxFont* GetFont( _lpcstr lpszFont, int nSize, _uint32 dwColor, bool bBold, bool bItalic, bool bAntialias );

private:
	typedef std::map< XUI_FontAttribute, GfxFont* >	CFontMap;
	CFontMap	m_FontMap;
};
typedef SingletonHolder< CFontManager, CreateStatic >	FontManager;

struct ClientInput	:	public XUI_Input
{
	XUI_IAnimation	*m_CursorAni[14];

	ClientInput( _lpcstr pszCursorConfig );
	virtual ~ClientInput();
	virtual void	GetMousePos( int *x, int *y );
	virtual void	SetMousePos( int x, int y );
	virtual _int32	GetMouseWheel();
	virtual void	RenderMouse();
	virtual void	SetMouse( _uint16 id );
	virtual bool	GetKeyState( int key )const;
	virtual bool	IsPressedLButton()const;
	virtual bool	IsReleaseLButton()const;
	virtual bool	IsPressedRButton()const;
	virtual bool	IsReleaseRButton()const;
	virtual bool	IsPressedMButton()const;
	virtual bool	IsReleaseMButton()const;
	virtual bool	IsMouseOver()const;

	virtual void	UpdateMouse( float fDeltaTime );
protected:
	//--------------------------------------------------------//
	//	created:	18:11:2007   11:22
	//	filename: 	d:\Develop\StarGame\GameClient\Canvas.h
	//	author:		Albert
	//
	//	purpose:	通过光标文件创建鼠标指针
	//--------------------------------------------------------//
	XUI_IAnimation*	CreateCursor( _lpcstr cursorfile );

private:
	_int32			m_nCount;
	_int32			m_nCurIndex;
	_uint32			m_nTimerHandle;
	XUI_IAnimation	*m_pCursor[14];
};

class CClientSprite	:	public XUI_ISprite
{
public:
	CClientSprite( _lpcstr _path, float _x, float _y, float _w, float _h );
	~CClientSprite();
	bool LoadTexture( _lpcstr lpszFileName, float x, float y, float w, float h );

	virtual float	GetWidth()const{ return w; }
	virtual float	GetHeight()const{ return h; }

	virtual void	SetCenter( float x, float y );
	virtual void	SetUV(float U0, float V0, float U1, float V1);
	virtual float	GetU0() const{ return m_u0; }
	virtual float	GetV0() const{ return m_v0; }
	virtual float	GetU1() const{ return m_u1; }
	virtual float	GetV1() const{ return m_v1; }

	virtual void	SetAlpha( float fAlpha ){ m_alpha = fAlpha; }
	virtual float	GetAlpha()const{ return m_alpha; }

	virtual void	SetAngle( float fAngle ){ m_angle = fAngle; }
	virtual float	GetAngle()const{ return m_angle; }

	virtual void	SetDiffuse( float fValue ){ m_diffuse = fValue; }
	virtual float	GetDiffuse()const{ return m_diffuse; }

	virtual void	Release();
	virtual void	Render( float x, float y );
	virtual void	RenderStretch( float x, float y, float w, float h );
	virtual void	RenderEx( float x, float y, float rot, float hscale = 1.0f, float vscale = 1.0f );

	void Render( float x, float y, float w, float h );
private:
	hgeSprite*	m_pSprite;
	float		m_u0, m_v0, m_u1, m_v1;
	float		m_alpha;
	float		m_angle;
	float		m_diffuse;
};

class CClientFont	:	public XUI_Font
{
public:
	CClientFont( const XUI_FontAttribute& FontAttrib, GfxFont* pFont );
	~CClientFont();

	virtual iSize GetStringSize( _lpctstr lpszString );

	virtual int GetCharacterWidth( _tchar szChar );
	virtual int GetCharacterHeight();

public:
	void SetColor( _uint32 dwColor );
	void Render( float x, float y, _lpctstr lpszText )const;
	void Render( float x, float y, _tchar szChar )const;
private:
	GfxFont*	m_pFont;
};

class CClientAnimation	:	public XUI_IAnimation
{
public:
	CClientAnimation( _lpcstr filename, int frames, float fps, float x, float y, float w, float h );
	~CClientAnimation();

	virtual void Play();
	virtual void Stop();
	virtual void Resume();
	virtual void Update( float fDelta );
	virtual void Render( int x, int y );

	virtual void	SetCurrentFrame( int nFrame );
	virtual int		GetCurrentFrame()const;

	virtual void	SetSpeed( float fFPS );
	virtual float	GetSpeed()const;

	virtual void	SetFrames( int nFrames );
	virtual int		GetFrames()const;

	virtual void	SetCenter( float x, float y );
	virtual void	GetCenter( float &x, float &y )const;

private:
	hgeAnimation	*m_pAnimation;
};

void init_canvas();