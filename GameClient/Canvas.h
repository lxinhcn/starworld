#pragma once
#include "GfxFont.h"
#include <hash_map>
using namespace UILib;

class CTextureManager
{
friend struct Loki::CreateStatic< CTextureManager >;
private:
	CTextureManager();
	~CTextureManager();

public:
	//---------------------------------------------------------------------//
	// describe	: 得到路径指定的纹理句柄
	// return	: 纹理句柄
	//---------------------------------------------------------------------//
	HTEXTURE GetTexture( _lpcstr lpszTexpath, _lpstr data = 0, size_t size = 0 );

	//---------------------------------------------------------------------//
	// describe	: 清除所有资源
	// return	:
	//---------------------------------------------------------------------//
	void Clear();

private:
	typedef stdext::hash_map< std::string, HTEXTURE >	CTextureMap;
	CTextureMap		m_TextureMap;	// 纹理映射
};
typedef Loki::SingletonHolder< CTextureManager, Loki::CreateStatic >	TextureManager;

class CFontManager
{
friend struct Loki::CreateStatic< CFontManager >;
private:
	CFontManager();
	~CFontManager();

public:
	GfxFont* GetFont( _lpcstr lpszFont, int nSize, bool bBold, bool bItalic, bool bAntialias );

private:
	typedef std::map< UILib::XUI_FontAttribute, GfxFont* >	CFontMap;
	CFontMap	m_FontMap;
};
typedef Loki::SingletonHolder< CFontManager, Loki::CreateStatic >	FontManager;

struct CXMouse	:	public XUI_IMouse
{
	//struct CursorDefine
	//{
	//	byte			m_hotx;			// 热点X坐标
	//	byte			m_hoty;			// 热点Y坐标
	//	byte			m_width;		// 鼠标宽度
	//	byte			m_height;		// 鼠标高度
	//	byte			m_frame_count;	// 帧数
	//	byte			m_frame_seq;	
	//	_lpcstr			m_filename;		// 文件名
	//};

	struct CursorDefine
	{
		HCURSOR		hCursor;
		LPTSTR		pszCursor;
	};

	CXMouse( CursorDefine* cursor, int count );
	//LoadCursor( _lpstr lpszPathname );
	virtual ~CXMouse();
	virtual void	GetMousePos( float *x, float *y );
	virtual void	SetMousePos( float x, float y );
	virtual int32	GetMouseWheel();
	virtual void	RenderMouse();
	virtual void	SetMouse( uint16 id );
	virtual bool	IsPressedLButton()const;
	virtual bool	IsReleaseLButton()const;
	virtual bool	IsPressedRButton()const;
	virtual bool	IsReleaseRButton()const;
	virtual bool	IsPressedMButton()const;
	virtual bool	IsReleaseMButton()const;
	virtual bool	IsMouseOver()const;

	bool OnTimer( unsigned int handle, unsigned short& repeat, unsigned int& timer );
private:
	//XUI_IMouse::CursorDefine*	m_pCursorArray;
	CursorDefine	*m_pCursorArray;
	int32			m_nCount;
	int32			m_nCurIndex;
	uint32			m_nTimerHandle;
};

class CClientSprite	:	public UILib::XUI_ISprite
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

class CClientFont	:	public UILib::XUI_IFont
{
public:
	CClientFont( const XUI_FontAttribute& FontAttrib, GfxFont* pFont );
	~CClientFont();

	virtual SIZE GetStringSize( _lpctstr lpszString );

	virtual INT GetCharacterWidth( _tchar szChar );
	virtual INT	GetCharacterHeight();

public:
	void SetColor( uint32 dwColor );
	void Render( float x, float y, _lpctstr lpszText )const;
	void Render( float x, float y, _tchar szChar )const;
private:
	GfxFont*	m_pFont;
};

class CClientAnimation	:	public XUI_IAnimation
{
public:
	CClientAnimation( HTEXTURE hTex, int nFrames, float FPS, float x, float y, float w, float h );
	~CClientAnimation();

	virtual void Play();
	virtual void Stop();
	virtual void Resume();
	virtual void Update( float fDelta );
	virtual void Render( float x, float y );

	virtual void	SetCurrentFrame( int nFrame );
	virtual int		GetCurrentFrame()const;

	virtual void	SetSpeed( float fFPS );
	virtual float	GetSpeed()const;

	virtual void	SetFrames( int nFrames );
	virtual int		GetFrames()const;

private:
	hgeAnimation	m_Animation;
};

void init_canvas();