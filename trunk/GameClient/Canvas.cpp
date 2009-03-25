#include "StdAfx.h"
#include "Canvas.h"
#include "Application.h"

//////////////////////////////////////////////////////////////////////////
// CTextureManager class
//////////////////////////////////////////////////////////////////////////
CTextureManager::CTextureManager()
{

}

CTextureManager::~CTextureManager()
{
}

//---------------------------------------------------------------------//
// describe	: 得到路径指定的纹理句柄
// return	: 纹理句柄
//---------------------------------------------------------------------//
HTEXTURE CTextureManager::GetTexture( _lpcstr lpszTexpath )
{
	if( lpszTexpath == NULL ) return NULL;
	CTextureMap::iterator iter = m_TextureMap.find( lpszTexpath );
	if( iter != m_TextureMap.end() )
	{
		return iter->second;
	}

	HGE* hge = Application::Instance().getEngine();
	HTEXTURE h = hge->Texture_Load( lpszTexpath );
	m_TextureMap[lpszTexpath] = h;
	return h;
}

void CTextureManager::Clear()
{
	HGE* hge = Application::Instance().getEngine();
	CTextureMap::iterator iter = m_TextureMap.begin();
	while( iter != m_TextureMap.end() )
	{
		hge->Texture_Free( iter->second );
		++iter;
	}
	m_TextureMap.clear();
}
//////////////////////////////////////////////////////////////////////////
// CFontManager
//////////////////////////////////////////////////////////////////////////
CFontManager::CFontManager()
{

}

CFontManager::~CFontManager()
{
	CFontMap::iterator iter = m_FontMap.begin();
	while( iter != m_FontMap.end() )
	{
		delete iter->second;
		++iter;
	}
	m_FontMap.clear();
}

GfxFont* CFontManager::GetFont( _lpcstr lpszFont, int nSize, bool bBold, bool bItalic, bool bAntialias )
{
	UILib::XUI_FontAttribute font( lpszFont, nSize, bBold, bItalic, bAntialias );
	CFontMap::iterator iter = m_FontMap.find( font );
	if( iter != m_FontMap.end() )
	{
		return iter->second;
	}

	GfxFont* pFont = new GfxFont( XA2T(lpszFont), nSize, bBold, bItalic, bAntialias );
	m_FontMap[font] = pFont;
	return pFont;
}

//////////////////////////////////////////////////////////////////////////
// CClientSprite class
//////////////////////////////////////////////////////////////////////////
CClientSprite::CClientSprite()
: m_pSprite( NULL )
, m_u0(0.0f), m_v0(0.0f), m_u1(1.0f), m_v1(1.0f)
, m_x(0.0f), m_y(0.0f), m_w(0.0f), m_h(0.0f)
{
}

CClientSprite::~CClientSprite()
{
	SAFE_DELETE( m_pSprite );
}

void CClientSprite::Release()
{
	delete this;
}

bool CClientSprite::LoadTexture( _lpcstr lpszFileName, float x, float y, float w, float h )
{
	if( lpszFileName == NULL ) return FALSE;

	HTEXTURE hTexture = TextureManager::Instance().GetTexture( lpszFileName );
	if( !hTexture ) return FALSE;
	HGE* hge = Application::Instance().getEngine();

	m_pSprite = new hgeSprite( hTexture, x, y, w, h );
	if( m_pSprite == NULL ) return false;
	m_x = x, m_y = y, m_w = w, m_h = h;
	return true;
}

float CClientSprite::GetWidth()const
{
	return m_w;
}

float CClientSprite::GetHeight()const
{
	return m_h;
}

void	CClientSprite::SetUV(float U0, float V0, float U1, float V1)
{
	float x = U0 * m_w;
	float y = V0 * m_h;
	float w	= (U1 - U0)*m_w;
	float h = (V1 - V0)*m_h;
	m_pSprite->SetTextureRect( x, y, w, h );
}

void CClientSprite::Render( int nX, int nY, int nWidth, int nHeight, LPCRECT lpClipperRect )
{
	CRect rc( nX, nY, nX + nWidth, nY + nHeight );
	CRect intersect;
	intersect.IntersectRect( rc, lpClipperRect );
	if( intersect.IsRectEmpty() ) return;
	if( intersect != rc )
	{
		float x, y, w, h;
		m_pSprite->GetTextureRect( &x, &y, &w, &h );
		m_pSprite->SetTextureRect( 
			float(x + intersect.left - nX),
			float(y + intersect.top - nY),
			float(intersect.Width()),
			float(intersect.Height())
			);
		m_pSprite->Render( (float)intersect.left, (float)intersect.top );
	}
	else
	{
		m_pSprite->Render( (float)nX, (float)nY );
	}
}

void CClientSprite::Render( float x, float y )
{
	m_pSprite->Render( x, y );
}

void CClientSprite::RenderStretch( float x, float y, float w, float h )
{
	m_pSprite->RenderStretch( x, y, x + w, y + h );
}

void CClientSprite::RenderEx( float x, float y, float rot, float hscale /* = 1.0f */, float vscale /* = 1.0f */ )
{
	m_pSprite->RenderEx( x, y, rot, hscale, vscale );
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CClientFont::CClientFont( const XUI_FontAttribute& FontAttrib, GfxFont* pFont )
: XUI_IFont( FontAttrib )
, m_pFont( pFont )
{
}

CClientFont::~CClientFont()
{
};

SIZE CClientFont::GetStringSize( _lpctstr lpszString )
{
	return m_pFont->GetTextSize( lpszString );
}

INT CClientFont::GetCharacterWidth( _tchar szChar )
{
	return m_pFont->GetCharacterWidth( szChar );
}

INT	CClientFont::GetCharacterHeight()
{
	return m_pFont->GetCharacterHeight();
}

void	CClientFont::SetColor( uint32 dwColor )
{
	m_pFont->SetColor( dwColor );
}

void	CClientFont::Render( float x, float y, _lpctstr lpszText )const
{
	m_pFont->Render( x, y, lpszText );
}

void CClientFont::Render( float x, float y, _tchar szChar )const
{
	m_pFont->Render( x, y, szChar );
}

//////////////////////////////////////////////////////////////////////////
// 鼠标功能类
//////////////////////////////////////////////////////////////////////////
CXMouse::CXMouse( const XUI_SpriteAttribute& sprite )
{
	m_pCursor = XUI_CreateSprite( sprite.path.c_str(), sprite.x, sprite.y, sprite.w, sprite.h );
}

CXMouse::~CXMouse()
{
	XUI_DestroySprite( m_pCursor );
}

void	CXMouse::GetMousePos( float *x, float *y )
{
	Application::Instance()->Input_GetMousePos( x, y );
}

void	CXMouse::SetMousePos( float x, float y )
{
	Application::Instance()->Input_SetMousePos( x, y );
}

int32	CXMouse::GetMouseWheel()
{
	return Application::Instance()->Input_GetMouseWheel();
}

void	CXMouse::RenderMouse()
{
	float x, y;
	GetMousePos( &x, &y );
	m_pCursor->Render( x, y );
}

bool	CXMouse::IsPressedLButton()const
{
	return Application::Instance()->Input_KeyDown(HGEK_LBUTTON);
}

bool	CXMouse::IsReleaseLButton()const
{
	return Application::Instance()->Input_KeyUp(HGEK_LBUTTON);
}

bool	CXMouse::IsPressedRButton()const
{
	return Application::Instance()->Input_KeyDown(HGEK_RBUTTON);
}

bool	CXMouse::IsReleaseRButton()const
{
	return Application::Instance()->Input_KeyUp(HGEK_RBUTTON);
}

bool	CXMouse::IsPressedMButton()const
{
	return Application::Instance()->Input_KeyDown(HGEK_MBUTTON);
}

bool	CXMouse::IsReleaseMButton()const
{
	return Application::Instance()->Input_KeyUp(HGEK_MBUTTON);
}

bool	CXMouse::IsMouseOver()const
{
	return Application::Instance()->Input_IsMouseOver();
}
//////////////////////////////////////////////////////////////////////////

static bool _DrawText( _lpcstr lpszText, UILib::XUI_IFont* pFont, float x, float y )
{
	if( pFont == NULL ) pFont = GuiSystem::Instance().GetDefaultFont();
	CClientFont* pF = static_cast< CClientFont* >( pFont );
	if( pF )
	{
		pF->Render( x, y, XA2T(lpszText) );
	}
	return true;
}

static bool _DrawCharacter( _tchar szChar, UILib::XUI_IFont* pFont, float x, float y )
{
	if( pFont == NULL ) pFont = GuiSystem::Instance().GetDefaultFont();
	CClientFont* pF = static_cast< CClientFont* >( pFont );
	if( pF )
	{
		pF->Render( x, y, szChar );
	}
	return true;
}

//没有边框的矩形背景
static bool _DrawRect( const CRect& rcDest, uint32 dwBorderColor, uint32 dwBkColor/* = -1*/ )
{
	Application::Instance()->Gfx_RenderLine( (float)rcDest.left, (float)rcDest.top, (float)rcDest.right, (float)rcDest.top, dwBorderColor );
	Application::Instance()->Gfx_RenderLine( (float)rcDest.right, (float)rcDest.top, (float)rcDest.right, (float)rcDest.bottom, dwBorderColor );
	Application::Instance()->Gfx_RenderLine( (float)rcDest.right, (float)rcDest.bottom, (float)rcDest.left, (float)rcDest.bottom, dwBorderColor );
	Application::Instance()->Gfx_RenderLine( (float)rcDest.left, (float)rcDest.top, (float)rcDest.left, (float)rcDest.bottom, dwBorderColor );
	return true;
}

static bool _DrawPolygon( const CPoint* ptArray, uint32* dwColorArray, uint32 nCount, uint16* pTriListArray, int32 nTriCount )
{
	return true;
}

static bool _DrawSprite( const XUI_ISprite* Tex, int nX, int nY, int nWidth, int nHeight, LPCRECT lpClipperRect/* = NULL*/ )
{
	CClientSprite* pTexture = (CClientSprite*)Tex;
	pTexture->Render( nX, nY, nWidth, nHeight, lpClipperRect );
	return true;
}

static XUI_ISprite* _CreateSprite( _lpcstr filename, float x, float y, float w, float h )
{
	CClientSprite* pTexture = new CClientSprite();
	if( pTexture->LoadTexture( filename, x, y, w, h ) )
	{
		return pTexture;
	}
	delete pTexture;
	return NULL;
}

static XUI_ISprite* _CreateSpriteEx( const XUI_SpriteAttribute& SpriteAttribute )
{
	return _CreateSprite( SpriteAttribute.path.c_str(), SpriteAttribute.x, SpriteAttribute.y, SpriteAttribute.w, SpriteAttribute.h );
}

static void _DestroySprite( XUI_ISprite* pTexture )
{
	delete pTexture;
}

static XUI_IFont* _CreateFont( _lpcstr lpszFontName, int nSize, bool bBold, bool bItalic, bool bAntialias )
{
	GfxFont* pGfxFont = FontManager::Instance().GetFont( lpszFontName, nSize, bBold, bItalic, bAntialias );
	if( pGfxFont )
	{
		return new CClientFont( XUI_FontAttribute( lpszFontName, nSize, bBold, bItalic, bAntialias ), pGfxFont );
	}
	return NULL;
}

static XUI_IFont* _CreateFontEx( const XUI_FontAttribute& FontAttribute )
{
	return _CreateFont( FontAttribute.name.c_str(), FontAttribute.size, FontAttribute.bold, FontAttribute.italic, FontAttribute.antialias );
}

static void _DestroyFont( XUI_IFont* pFont )
{
	delete pFont;
}

void init_canvas()
{
	UILib::XUI_DrawText			= _DrawText;
	UILib::XUI_DrawCharacter	= _DrawCharacter;
	UILib::XUI_DrawRect			= _DrawRect;
	UILib::XUI_DrawPolygon		= _DrawPolygon;
	UILib::XUI_DrawSprite		= _DrawSprite;
	UILib::XUI_CreateSprite		= _CreateSprite;
	UILib::XUI_CreateSpriteEx	= _CreateSpriteEx;
	UILib::XUI_DestroySprite	= _DestroySprite;
	UILib::XUI_CreateFont		= _CreateFont;
	UILib::XUI_CreateFontEx		= _CreateFontEx;
	UILib::XUI_DestroyFont		= _DestroyFont;
}
