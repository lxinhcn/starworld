#pragma once
#include "GfxFont.h"
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
	HTEXTURE GetTexture( _lpctstr lpszTexpath );

	//---------------------------------------------------------------------//
	// describe	: 清除所有资源
	// return	:
	//---------------------------------------------------------------------//
	void Clear();

private:
	typedef std::map< _string, HTEXTURE >	CTextureMap;
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
	GfxFont* GetFont( _lpctstr lpszFont, int nSize, bool bBold, bool bItalic, bool bAntialias );

private:
	typedef std::map< UILib::XUI_FontAttribute, GfxFont* >	CFontMap;
	CFontMap	m_FontMap;
};
typedef Loki::SingletonHolder< CFontManager, Loki::CreateStatic >	FontManager;

class CClientSprite	:	public UILib::XUI_ISprite
{
public:
	CClientSprite();
	~CClientSprite();
	bool LoadTexture( _lpctstr lpszFileName, float x, float y, float w, float h );

	virtual float	GetWidth()const;
	virtual float	GetHeight()const;

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

	void Render( int nX, int nY, int nWidth, int nHeight, LPCRECT lpClipperRect );
private:
	hgeSprite*	m_pSprite;
	float		m_u0, m_v0, m_u1, m_v1;
	float		m_x, m_y, m_w, m_h;
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

void init_canvas();