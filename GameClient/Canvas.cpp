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
HTEXTURE CTextureManager::GetTexture( _lpcstr key, _lpstr data, size_t size )
{
	if( key == NULL ) return NULL;
	CTextureMap::iterator iter = m_TextureMap.find( key );
	if( iter != m_TextureMap.end() )
	{
		return iter->second;
	}

	HGE* hge = Application::Instance().getEngine();
	HTEXTURE h = NULL;
	if( data && size )
	{
		h = hge->Texture_Load( data, size );
	}
	else
	{
		std::string path = std::string() + GuiSystem::Instance().GetResourcePath() + key;
		h = hge->Texture_Load( path.c_str() );
	}
	m_TextureMap[key] = h;
	return h;
}

void CTextureManager::Clear()
{
	HGE* hge = hgeCreate( HGE_VERSION );
	CTextureMap::iterator iterMap = m_TextureMap.begin();
	while( iterMap != m_TextureMap.end() )
	{
		hge->Texture_Free( iterMap->second );
		++iterMap;
	}
	m_TextureMap.clear();

	hge->Release();
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
CClientSprite::CClientSprite( _lpcstr _path, float _x, float _y, float _w, float _h )
: m_pSprite( NULL )
, m_u0(0.0f), m_v0(0.0f), m_u1(1.0f), m_v1(1.0f)
{
	path = _path, x = _x, y = _y, w = _w, h = _h;
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
	return true;
}

void CClientSprite::SetCenter( float x, float y )
{
	m_pSprite->SetHotSpot( x, y );
}

void CClientSprite::SetUV(float U0, float V0, float U1, float V1)
{
	m_u0 = U0, m_u1 = U1, m_v0 = V0, m_v1 = V1;

	float tx, ty, tw, th;
	m_pSprite->GetTextureRect( &tx, &ty, &tw, &th );
	float _x = x + U0 * tw;
	float _y = y + V0 * th;
	float _w = (U1 - U0) * tw;
	float _h = (V1 - V0) * th;
	//char szLog[1024];
	//_snprintf( szLog, sizeof(szLog), "x = %f, y = %f, w = %f, h = %f", x, y, w, h );
	//XUI_DrawTextA( szLog, NULL, 10, 60 );
	m_pSprite->SetTextureRect( _x, _y, _w, _h, false );
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

void	CClientFont::SetColor( _uint32 dwColor )
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
CXMouse::CXMouse( CXMouse::CursorDefine* cursor, int count )
: m_nCount( count )
, m_nCurIndex( 0 )
, m_pCursorArray( cursor )
, m_nTimerHandle( -1 )
{
	//m_pCursorArray = new XUI_IMouse::CursorDefine[count];
	//for( int i = 0; i < count; ++i )
	//{
	//	m_pCursorArray[i].m_frame_count = cursor[i].m_frame_count;
	//	m_pCursorArray[i].m_frame_seq = cursor[i].m_frame_seq;
	//	m_pCursorArray[i].m_cur_frame = 0;
	//	m_pCursorArray[i].m_texture = XUI_CreateSprite( cursor[i].m_filename, 1.0f*i*cursor[i].m_width, 0.0f, 1.0f*cursor[i].m_width, 1.0f*cursor[i].m_height*cursor[i].m_frame_count );
	//	m_pCursorArray[i].m_texture->SetCenter( cursor[i].m_hotx, cursor[i].m_hoty );
	//}
	SetMouse( XUI_MOUSE_ARROW );
}

CXMouse::~CXMouse()
{
	//GuiSystem::Instance().KillTimer( m_nTimerHandle );
	//for( int i = 0; i < m_nCount; ++i )
	//{
	//	XUI_DestroySprite( m_pCursorArray[i].m_texture );
	//}
	//delete[] m_pCursorArray;
}

bool	CXMouse::OnTimer( unsigned int handle, unsigned short& repeat, unsigned int& timer )
{
	//repeat = 1;
	//XUI_IMouse::CursorDefine* pCursor = m_pCursorArray + m_nCurIndex;
	//if( pCursor )
	//{
	//	++pCursor->m_cur_frame;
	//	if( pCursor->m_texture )
	//		pCursor->m_texture->SetUV( 
	//			pCursor->m_texture->GetU0(), pCursor->m_cur_frame%pCursor->m_frame_count*1.0f/pCursor->m_frame_count,
	//			pCursor->m_texture->GetU1(), pCursor->m_cur_frame%pCursor->m_frame_count*1.0f/pCursor->m_frame_count + 1 );
	//}
	return true;
}

void	CXMouse::GetMousePos( float *x, float *y )
{
	Application::Instance()->Input_GetMousePos( x, y );
}

void	CXMouse::SetMousePos( float x, float y )
{
	Application::Instance()->Input_SetMousePos( x, y );
}

_int32	CXMouse::GetMouseWheel()
{
	return Application::Instance()->Input_GetMouseWheel();
}

void	CXMouse::RenderMouse()
{
	return;
	//float x, y;
	//GetMousePos( &x, &y );
	//CursorDefine* pCursor = m_pCursorArray + m_nCurIndex;
	//if( pCursor && pCursor->m_texture )
	//{
	//	pCursor->m_texture->Render( x, y );
	//}
}

void	CXMouse::SetMouse( _uint16 id )
{
	//if( id < m_nCount )
	//{
	//	XUI_IMouse::CursorDefine* pCursor = m_pCursorArray + m_nCurIndex;
	//	GuiSystem::Instance().KillTimer( m_nTimerHandle );
	//	if( pCursor && pCursor->m_frame_seq )
	//		m_nTimerHandle = GuiSystem::Instance().SetTimer( event_function( this, &CXMouse::OnTimer ), 1, TIMER_SECOND(pCursor->m_frame_seq/16) );
	//	else
	//		m_nTimerHandle = -1;
	//	m_nCurIndex = id;
	//}

	if( id < m_nCount )
	{
		CursorDefine* pCursor = m_pCursorArray + m_nCurIndex;
		if( pCursor->hCursor )
			SetCursor( pCursor->hCursor );
		else
		{
			pCursor->hCursor = LoadCursor( NULL, pCursor->pszCursor );
			SetCursor( pCursor->hCursor );
		}
	}
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
CClientAnimation::CClientAnimation( HTEXTURE hTex, int nFrames, float FPS, float x, float y, float w, float h )
: m_Animation( hTex, nFrames, FPS, x, y, w, h )
{

}

CClientAnimation::~CClientAnimation()
{

}

void CClientAnimation::Play()
{
	m_Animation.Play();
}

void CClientAnimation::Stop()
{
	m_Animation.Stop();
}

void CClientAnimation::Resume()
{
	m_Animation.Resume();
}

void CClientAnimation::Update( float fDelta )
{
	m_Animation.Update( fDelta );
}

void CClientAnimation::Render( float x, float y )
{
	m_Animation.Render( x, y );
}

void CClientAnimation::SetCurrentFrame( int nFrame )
{
	m_Animation.SetFrame( nFrame );
}

int	CClientAnimation::GetCurrentFrame()const
{
	return m_Animation.GetFrame();
}

void CClientAnimation::SetSpeed( float fFPS )
{
	m_Animation.SetSpeed( fFPS );
}

float CClientAnimation::GetSpeed()const
{
	return m_Animation.GetSpeed();
}

void CClientAnimation::SetFrames( int nFrames )
{
	m_Animation.SetFrames( nFrames );
}

int	CClientAnimation::GetFrames()const
{
	return m_Animation.GetFrames();
}

//////////////////////////////////////////////////////////////////////////

static void _SetClipping( _int32 x, _int32 y, _int32 w, _int32 h )
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
static void _DrawRect( const xgcRect& rc, _uint32 bordercolor, _uint32 backgroundcolor/* = -1*/ )
{
	HGE* hge = hgeCreate(HGE_VERSION);

	if( GETA(bordercolor) != 0 )
	{
		hge->Gfx_RenderLine( (float)rc.left, (float)rc.top, (float)rc.right, (float)rc.top, bordercolor );
		hge->Gfx_RenderLine( (float)rc.right, (float)rc.top, (float)rc.right, (float)rc.bottom, bordercolor );
		hge->Gfx_RenderLine( (float)rc.right, (float)rc.bottom, (float)rc.left, (float)rc.bottom, bordercolor );
		hge->Gfx_RenderLine( (float)rc.left, (float)rc.top, (float)rc.left, (float)rc.bottom, bordercolor );
	}

	if( GETA(backgroundcolor) != 0 )
	{
		hgeSprite sprite( 0, 0, 0, 0, 0 );
		sprite.SetColor( backgroundcolor );
		sprite.RenderStretch( (float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom );
	}
	hge->Release();
}

static void _DrawLine( float x0, float y0, float x1, float y1, _uint32 color )
{
	Application::Instance()->Gfx_RenderLine( x0, y0, x1, y1, color );
}

static void _DrawPolygon( const xgcPoint* ptArray, _uint32* dwColorArray, _uint32 nCount, _uint16* pTriListArray, _int32 nTriCount )
{
	// Application::Instance()->Gfx_RenderTriple
}

static void _DrawSprite( const XUI_ISprite* Tex, int nX, int nY, int nWidth, int nHeight )
{
	CClientSprite* pSprite = (CClientSprite*)Tex;
	pSprite->Render( (float)nX, (float)nY, (float)nWidth, (float)nHeight );
}

static XUI_ISprite* _CreateSprite( _lpcstr filename, float x, float y, float w, float h )
{
	CClientSprite* pTexture = new CClientSprite( filename, x, y, w, h);
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
	UILib::XUI_DrawLine			= _DrawLine;
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
