#include "StdAfx.h"
#include "Canvas.h"
#include "Application.h"
#define RESOURCEPATH	"..\\Resource\\UI\\Media\\"

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
	HTEXTURE h = hge->Texture_Load( ( std::string() + RESOURCEPATH + lpszTexpath ).c_str() );
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

void CClientSprite::Render( float x, float y, float w, float h )
{
	//float tx, ty, tw, th;
	//m_pSprite->GetTextureRect( &tx, &ty, &tw, &th );
	//m_pSprite->SetTextureRect( tx + x, ty + y, w, h );

	m_pSprite->RenderStretch( x, y, x+w, y+h );
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

static void _SetClipping( int32 x, int32 y, int32 w, int32 h )
{
	Application::Instance()->Gfx_SetClipping( x, y, w, h );
}

static void _DrawText( _lpctstr lpszText, UILib::XUI_IFont* pFont, float x, float y )
{
	if( pFont == NULL ) pFont = GuiSystem::Instance().GetDefaultFont();
	CClientFont* pF = static_cast< CClientFont* >( pFont );
	if( pF )
	{
		pF->Render( x, y, lpszText );
	}
}

static void _DrawCharacter( _tchar szChar, UILib::XUI_IFont* pFont, float x, float y )
{
	if( pFont == NULL ) pFont = GuiSystem::Instance().GetDefaultFont();
	CClientFont* pF = static_cast< CClientFont* >( pFont );
	if( pF )
	{
		pF->Render( x, y, szChar );
	}
}

//没有边框的矩形背景
static void _DrawRect( const x_rect& rc, uint32 bordercolor, uint32 backgroundcolor/* = -1*/ )
{
	if( GETA(bordercolor) != 0 )
	{
		Application::Instance()->Gfx_RenderLine( (float)rc.left, (float)rc.top, (float)rc.right, (float)rc.top, bordercolor );
		Application::Instance()->Gfx_RenderLine( (float)rc.right, (float)rc.top, (float)rc.right, (float)rc.bottom, bordercolor );
		Application::Instance()->Gfx_RenderLine( (float)rc.right, (float)rc.bottom, (float)rc.left, (float)rc.bottom, bordercolor );
		Application::Instance()->Gfx_RenderLine( (float)rc.left, (float)rc.top, (float)rc.left, (float)rc.bottom, bordercolor );
	}

	if( GETA(backgroundcolor) != 0 )
	{
		hgeSprite sprite( 0, 0, 0, 0, 0 );
		sprite.SetColor( backgroundcolor );
		sprite.RenderStretch( (float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom );
	}
}

static void _DrawLine( float x0, float y0, float x1, float y1, uint32 color )
{
	Application::Instance()->Gfx_RenderLine( x0, y0, x1, y1, color );
}

static void _DrawPolygon( const x_point* ptArray, uint32* dwColorArray, uint32 nCount, uint16* pTriListArray, int32 nTriCount )
{
	// Application::Instance()->Gfx_RenderTriple
}

static void _DrawSprite( const XUI_ISprite* Tex, int nX, int nY, int nWidth, int nHeight )
{
	CClientSprite* pTexture = (CClientSprite*)Tex;
	pTexture->Render( (float)nX, (float)nY, (float)nWidth, (float)nHeight );
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

static LRESULT CALLBACK _DefWindowProc( __in HWND hWnd, __in UINT Msg, __in WPARAM wParam, __in LPARAM lParam)
{
	return Application::Instance().m_pDefWindowProc( hWnd, Msg, wParam, lParam );
}

void init_canvas()
{
	UILib::XUI_SetClipping		= _SetClipping;
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
	UILib::XUI_DefWindowProc	= _DefWindowProc;
}
