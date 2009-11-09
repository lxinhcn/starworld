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

	CTextureMap::_Pairib ret = m_TextureMap.insert( CTextureMap::value_type( key, h ) );
	if( !ret.second )
	{
		hge->Texture_Free(h);
		return NULL;
	}
	return h;
}

//---------------------------------------------------------------------//
// describe	: 创建纹理
// return	: 纹理句柄
//---------------------------------------------------------------------//
HTEXTURE CTextureManager::CreateTexture( _lpcstr key, int w, int h )
{
	if( key == NULL ) return NULL;
	CTextureMap::iterator iter = m_TextureMap.find( key );
	if( iter != m_TextureMap.end() )
	{
		return NULL;
	}

	HGE* hge = Application::Instance().getEngine();
	HTEXTURE t = hge->Texture_Create( w, h );
	CTextureMap::_Pairib ret = m_TextureMap.insert( CTextureMap::value_type( key, t ) );
	if( !ret.second )
	{
		hge->Texture_Free(t);
		return NULL;
	}

	return t;
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

GfxFont* CFontManager::GetFont( _lpcstr lpszFont, int nSize, _uint32 dwColor, bool bBold, bool bItalic, bool bAntialias )
{
	UILib::XUI_FontAttribute font( lpszFont, nSize, dwColor, bBold, bItalic, bAntialias );
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
	HTEXTURE hTexture = TextureManager::Instance().GetTexture( _path );
	if( hTexture )
	{
		m_pSprite = new hgeSprite( hTexture, x, y, w, h );
	}
}

CClientSprite::~CClientSprite()
{
	SAFE_DELETE( m_pSprite );
}

void CClientSprite::Release()
{
	delete this;
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
CClientMouse::CClientMouse( _lpcstr pszCursorConfig )
: m_nCurIndex( 0 )
, m_nTimerHandle( -1 )
{
	TiXmlDocument doc;
	_astring strPathname( GuiSystem::Instance().GetResourcePath() );
	strPathname += pszCursorConfig;

	if( doc.LoadFile( strPathname.c_str() ) )
	{
		_lpstr path = _strdup( pszCursorConfig );
		_lpstr end = strrchr( path, _T('\\') );
		if( end ) *(end+1) = 0;

		TiXmlElement *pRoot = doc.FirstChildElement( "CURSOR" );
		if( pRoot )
		{
			for( TiXmlElement *pElement = pRoot->FirstChildElement();
				pElement != NULL;
				pElement = pElement->NextSiblingElement()
				)
			{
				_lpcstr pszArrow[] = { "ARROW","TEXT","CROSS","SIZENESW","SIZENS","SIZENWSE","SIZEWE","SIZEALL","WAIT","BUSY","HAND","HANDWRITE","STOP","HELP", };
				_lpcstr pszValue = pElement->Value();
				int idx = 0;
				while( stricmp( pszArrow[idx], pszValue ) ) ++idx;

				if( idx >= _countof(m_pCursor) ) continue;
				XUI_IAnimation *&pCursor = m_pCursor[idx];
				pCursor = XUI_CreateAnimation( 
					pElement->Attribute( "texture" ),
					pElement->IntAttribute( "frames", 1 ),
					pElement->FloatAttribute( "fps", 1.0f ),
					pElement->FloatAttribute( "x" ),
					pElement->FloatAttribute( "y" ),
					pElement->FloatAttribute( "w" ),
					pElement->FloatAttribute( "h" ) );
				pCursor->SetCenter( pElement->FloatAttribute( "hotx" ), pElement->FloatAttribute( "hoty" ) );
			}
		}
		free( path );
	}
	else
	{
		XGC_WRITELOGA( XUI, "cursor read error! reson: %s. line = %d, column = %d", doc.Error(), doc.ErrorRow(), doc.ErrorCol() );
	}
	SetMouse( XUI_MOUSE_ARROW );
}

CClientMouse::~CClientMouse()
{
	for( int i = 0; i < _countof(m_pCursor); ++i )
	{
		XUI_DestroyAnimation( m_pCursor[i] );
	}
}

void CClientMouse::UpdateMouse( float fDeltaTime )
{
	m_pCursor[m_nCurIndex]->Update( fDeltaTime );
}

void CClientMouse::GetMousePos( float *x, float *y )
{
	POINT pt;
	GetCursorPos( &pt );
	ScreenToClient(Application::Instance()->System_GetState(HGE_HWND), &pt);
	*x = (float)pt.x;
	*y = (float)pt.y;
}

void	CClientMouse::SetMousePos( float x, float y )
{
	POINT pt;
	pt.x=(long)x; pt.y=(long)y;
	ClientToScreen(Application::Instance()->System_GetState(HGE_HWND), &pt);
	SetCursorPos(pt.x,pt.y);
}

_int32	CClientMouse::GetMouseWheel()
{
	return Application::Instance()->Input_GetMouseWheel();
}

void	CClientMouse::RenderMouse()
{
	float x, y;
	GetMousePos( &x, &y );
	XUI_IAnimation *pCursor = m_pCursor[m_nCurIndex];
	pCursor->Render( x, y );
}

void	CClientMouse::SetMouse( _uint16 id )
{
	if( id < _countof(m_pCursor) )
	{
		m_nCurIndex = id;
	}
}

bool	CClientMouse::IsPressedLButton()const
{
	return Application::Instance()->Input_KeyDown(HGEK_LBUTTON);
}

bool	CClientMouse::IsReleaseLButton()const
{
	return Application::Instance()->Input_KeyUp(HGEK_LBUTTON);
}

bool	CClientMouse::IsPressedRButton()const
{
	return Application::Instance()->Input_KeyDown(HGEK_RBUTTON);
}

bool	CClientMouse::IsReleaseRButton()const
{
	return Application::Instance()->Input_KeyUp(HGEK_RBUTTON);
}

bool	CClientMouse::IsPressedMButton()const
{
	return Application::Instance()->Input_KeyDown(HGEK_MBUTTON);
}

bool	CClientMouse::IsReleaseMButton()const
{
	return Application::Instance()->Input_KeyUp(HGEK_MBUTTON);
}

bool	CClientMouse::IsMouseOver()const
{
	return Application::Instance()->Input_IsMouseOver();
}

//////////////////////////////////////////////////////////////////////////
CClientAnimation::CClientAnimation( _lpcstr filename, int frames, float fps, float x, float y, float w, float h )
: m_pAnimation( NULL )
{
	m_pAnimation = new hgeAnimation( 
		TextureManager::Instance().GetTexture( filename ),
		frames,
		fps,
		x,
		y,
		w,
		h );
}

CClientAnimation::~CClientAnimation()
{
	SAFE_DELETE( m_pAnimation );
}

void CClientAnimation::Play()
{
	m_pAnimation->Play();
}

void CClientAnimation::Stop()
{
	m_pAnimation->Stop();
}

void CClientAnimation::Resume()
{
	m_pAnimation->Resume();
}

void CClientAnimation::Update( float fDelta )
{
	m_pAnimation->Update( fDelta );
}

void CClientAnimation::Render( float x, float y )
{
	m_pAnimation->Render( x, y );
}

void CClientAnimation::SetCurrentFrame( int nFrame )
{
	m_pAnimation->SetFrame( nFrame );
}

int	CClientAnimation::GetCurrentFrame()const
{
	return m_pAnimation->GetFrame();
}

void CClientAnimation::SetSpeed( float fFPS )
{
	m_pAnimation->SetSpeed( fFPS );
}

float CClientAnimation::GetSpeed()const
{
	return m_pAnimation->GetSpeed();
}

void CClientAnimation::SetFrames( int nFrames )
{
	m_pAnimation->SetFrames( nFrames );
}

int	CClientAnimation::GetFrames()const
{
	return m_pAnimation->GetFrames();
}

void CClientAnimation::SetCenter( float x, float y )
{
	m_pAnimation->SetHotSpot( x, y );
}

void CClientAnimation::GetCenter( float &x, float &y )const
{
	m_pAnimation->GetHotSpot( &x, &y );
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
	return new CClientSprite( filename, x, y, w, h);
}

static XUI_ISprite* _CreateSpriteEx( const XUI_SpriteAttribute& SpriteAttribute )
{
	return _CreateSprite( SpriteAttribute.path.c_str(), SpriteAttribute.x, SpriteAttribute.y, SpriteAttribute.w, SpriteAttribute.h );
}

static void _DestroySprite( XUI_ISprite* pTexture )
{
	delete pTexture;
}

static XUI_IAnimation* _CreateAnimation( _lpcstr filename, int frames, float fps, float x, float y, float w, float h )
{
	return new CClientAnimation( filename, frames, fps, x, y, w, h );
}

static XUI_IAnimation* _CreateAnimationEx( const XUI_AnimationAttribute& AnimationAttribute )
{
	const XUI_AnimationAttribute& a = AnimationAttribute;
	return new CClientAnimation( a.key.c_str(), a.frames, a.fps, a.x, a.y, a.w, a.h );
}

static void _DestroyAnimation( XUI_IAnimation *pAnimation )
{
	delete pAnimation;
}

static XUI_IFont* _CreateFont( _lpcstr lpszFontName, int nSize, _uint32 dwColor, bool bBold, bool bItalic, bool bAntialias )
{
	GfxFont* pGfxFont = FontManager::Instance().GetFont( lpszFontName, nSize, dwColor, bBold, bItalic, bAntialias );
	if( pGfxFont )
	{
		pGfxFont->SetColor( dwColor );
		return new CClientFont( XUI_FontAttribute( lpszFontName, nSize, dwColor, bBold, bItalic, bAntialias ), pGfxFont );
	}
	return NULL;
}

static XUI_IFont* _CreateFontEx( const XUI_FontAttribute& FontAttribute )
{
	return _CreateFont( FontAttribute.name.c_str(), FontAttribute.size, FontAttribute.color, FontAttribute.bold, FontAttribute.italic, FontAttribute.antialias );
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
	UILib::XUI_CreateAnimation	= _CreateAnimation;
	UILib::XUI_CreateAnimationEx= _CreateAnimationEx;
	UILib::XUI_DestroyAnimation = _DestroyAnimation;
	UILib::XUI_CreateFont		= _CreateFont;
	UILib::XUI_CreateFontEx		= _CreateFontEx;
	UILib::XUI_DestroyFont		= _DestroyFont;
	UILib::XUI_DefWindowProc	= _DefWindowProc;
}
