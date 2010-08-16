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
		std::string path = std::string() + XUI::Instance().GetResourcePath() + key;
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
	XGC::ui::XUI_FontAttribute font( lpszFont, nSize, dwColor, bBold, bItalic, bAntialias );
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
: XUI_Font( FontAttrib )
, m_pFont( pFont )
{
}

CClientFont::~CClientFont()
{
};

iSize CClientFont::GetStringSize( _lpctstr lpszString )
{
	return m_pFont->GetTextSize( lpszString );
}

int CClientFont::GetCharacterWidth( _tchar szChar )
{
	return m_pFont->GetCharacterWidth( szChar );
}

int CClientFont::GetCharacterHeight()
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
ClientInput::ClientInput( _lpcstr pszCursorConfig )
: m_nCurIndex( 0 )
, m_nTimerHandle( -1 )
{
	TiXmlDocument doc;
	_astring strPathname( XUI::Instance().GetResourcePath() );
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
				if( pElement->Attribute("file") )
				{
					pCursor = CreateCursor(pElement->Attribute("file"));
				}
				else
				{
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
		}
		free( path );
	}
	else
	{
		XGC_WRITELOGA( XUILOG, "cursor read error! reson: %s. line = %d, column = %d", doc.Error(), doc.ErrorRow(), doc.ErrorCol() );
	}
	SetMouse( XUI_MOUSE_ARROW );
}

ClientInput::~ClientInput()
{
	for( int i = 0; i < _countof(m_pCursor); ++i )
	{
		XUI_DestroyAnimation( m_pCursor[i] );
	}
}

//--------------------------------------------------------//
//	created:	18:11:2007   11:22
//	filename: 	d:\Develop\StarGame\GameClient\Canvas.h
//	author:		Albert
//
//	purpose:	通过光标文件创建鼠标指针
//--------------------------------------------------------//
XUI_IAnimation*	ClientInput::CreateCursor( _lpcstr cursorfile )
{
#pragma pack( push )
	#pragma pack( 2 )
	struct CursorHeader
	{
		_uint16 reserved;
		_uint16 type;
		_uint16 image_count;
	};

	struct CursorDirentry
	{
		_byte width;	// Specifies image width in pixels. Can be 0, 255 or a number between 0 to 255. Should be 0 if image width is 256 pixels.
		_byte height;	// Specifies image height in pixels. Can be 0, 255 or a number between 0 to 255. Should be 0 if image height is 256 pixels
		_byte palette;	// Specifies number of colors in the color palette. Should be 0 if the image is truecolor
		_byte reseverd;	// Reserved. Should be 0
		_uint16	hotspot_x;
		_uint16	hotspot_y;
		_uint32 resource_size;
		_uint32 image_offset;
	};
#pragma pack( pop )

	FILE *fp;
	XUI_IAnimation *pCursor = NULL;
	_astring strPathname( XUI::Instance().GetResourcePath() );
	strPathname += cursorfile;

	if( fopen_s( &fp, strPathname.c_str(), "rb" ) == 0 )
	{
		CursorHeader	file_header;
		if( fread( (void*)&file_header, sizeof(file_header), 1, fp ) == 1 )
		{
			CursorDirentry	cursor_header;
			if( fread( (void*)&cursor_header, sizeof(CursorDirentry), 1, fp ) == 1 )
			{
				fseek( fp, cursor_header.image_offset, SEEK_SET );
				_lpstr bitmap = (_lpstr)malloc( cursor_header.resource_size );
				fread( bitmap, cursor_header.resource_size, 1, fp );

				LPBITMAPINFOHEADER binfo = (LPBITMAPINFOHEADER)bitmap;
				binfo->biHeight /= 2;

				_uint32 andMaskLen = cursor_header.resource_size 
					- binfo->biSize	// 位图的头
					- (binfo->biClrUsed?binfo->biClrUsed:binfo->biBitCount>8?0:(1<<binfo->biBitCount))*sizeof(RGBQUAD) // 调色板
					- binfo->biHeight*((((binfo->biWidth*binfo->biPlanes*binfo->biBitCount)+31)>>5)<<2)
					;

				binfo->biSizeImage = binfo->biHeight*((((binfo->biWidth*binfo->biPlanes*binfo->biBitCount)+31)>>5)<<2);
				HTEXTURE h = TextureManager::Instance().GetTexture( cursorfile, bitmap, binfo->biSize + binfo->biSizeImage );

				_lpcstr data = bitmap + binfo->biSize + binfo->biSizeImage;
				HGE *hge = hgeCreate(HGE_VERSION);
				_uint32 *pixel = (_uint32*)hge->Texture_Lock( h, false );
				// 取巧的办法，正确途径还是应该使用And Mask
				for( int i = 0; i < binfo->biWidth*binfo->biHeight; ++i )
				{
					if( !(pixel[i] & 0x00ffffff) )
					{
						pixel[i] &= 0x00ffffff;
					}
				}

				//for( int y = 0; y < binfo->biHeight; ++y )
				//{
				//	for( int x = 0; x < binfo->biWidth; ++x )
				//	{
				//		_lpcstr l = data + (binfo->biHeight-1-y)*(((binfo->biWidth+31)>>5)<<2);
				//		if(l[x/8]&(1<<(x%8)))
				//			*pixel++ &= 0x00ffffff;
				//		else
				//			*pixel++ |= 0xff000000;
				//	}
				//}

				hge->Texture_Unlock( h );
				hge->Release();
				pCursor = XUI_CreateAnimation( 
					cursorfile,
					1,
					1.0f,
					0,
					0,
					cursor_header.width,
					cursor_header.height );
				pCursor->SetCenter( cursor_header.hotspot_x, cursor_header.hotspot_y );
				free( bitmap );
			}
		}
		fclose( fp );
	}
	return pCursor;
}

void ClientInput::UpdateMouse( float fDeltaTime )
{
	m_pCursor[m_nCurIndex]->Update( fDeltaTime );
}

void ClientInput::GetMousePos( int *x, int *y )
{
	POINT pt;
	GetCursorPos( &pt );
	ScreenToClient(Application::Instance()->System_GetState(HGE_HWND), &pt);
	*x = pt.x;
	*y = pt.y;
}

void	ClientInput::SetMousePos( int x, int y )
{
	POINT pt;
	pt.x=(long)x; pt.y=(long)y;
	ClientToScreen(Application::Instance()->System_GetState(HGE_HWND), &pt);
	SetCursorPos(pt.x,pt.y);
}

_int32	ClientInput::GetMouseWheel()
{
	return Application::Instance()->Input_GetMouseWheel();
}

void	ClientInput::RenderMouse()
{
	int x, y;
	GetMousePos( &x, &y );
	XUI_IAnimation *pCursor = m_pCursor[m_nCurIndex];
	pCursor->Render( x, y );
}

void	ClientInput::SetMouse( _uint16 id )
{
	if( id < _countof(m_pCursor) )
	{
		m_nCurIndex = id;
	}
}

bool	ClientInput::GetKeyState( int key )const
{
	return Application::Instance()->Input_GetKeyState( key );
}

bool	ClientInput::IsPressedLButton()const
{
	return Application::Instance()->Input_KeyDown(HGEK_LBUTTON);
}

bool	ClientInput::IsReleaseLButton()const
{
	return Application::Instance()->Input_KeyUp(HGEK_LBUTTON);
}

bool	ClientInput::IsPressedRButton()const
{
	return Application::Instance()->Input_KeyDown(HGEK_RBUTTON);
}

bool	ClientInput::IsReleaseRButton()const
{
	return Application::Instance()->Input_KeyUp(HGEK_RBUTTON);
}

bool	ClientInput::IsPressedMButton()const
{
	return Application::Instance()->Input_KeyDown(HGEK_MBUTTON);
}

bool	ClientInput::IsReleaseMButton()const
{
	return Application::Instance()->Input_KeyUp(HGEK_MBUTTON);
}

bool	ClientInput::IsMouseOver()const
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

void CClientAnimation::Render( int x, int y )
{
	m_pAnimation->Render( (float)x, (float)y );
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

static void _DrawText( _lpctstr lpszText, XGC::ui::XUI_Font* pFont, int x, int y )
{
	if( pFont == NULL ) pFont = XUI::Instance().GetDefaultFont();
	CClientFont* pF = static_cast< CClientFont* >( pFont );
	if( pF )
	{
		pF->Render( (float)x, (float)y, lpszText );
	}
}

static void _DrawCharacter( _tchar szChar, XGC::ui::XUI_Font* pFont, int x, int y )
{
	if( pFont == NULL ) pFont = XUI::Instance().GetDefaultFont();
	CClientFont* pF = static_cast< CClientFont* >( pFont );
	if( pF )
	{
		pF->Render( (float)x, (float)y, szChar );
	}
}

//没有边框的矩形背景
static void _DrawRect( const iRect& rc, _uint32 bordercolor, _uint32 backgroundcolor/* = -1*/ )
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

static void _DrawPolygon( const iPoint* ptArray, _uint32* dwColorArray, _uint32 nCount, _uint16* pTriListArray, _int32 nTriCount )
{
	// Application::Instance()->Gfx_RenderTriple
}

static void _DrawSprite( const XUI_ISprite* Tex, int x, int y, int width, int height )
{
	CClientSprite* pSprite = (CClientSprite*)Tex;
	pSprite->Render( (float)x, (float)y, (float)width, (float)height );
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

static XUI_Font* _CreateFont( _lpcstr lpszFontName, int nSize, _uint32 dwColor, bool bBold, bool bItalic, bool bAntialias )
{
	GfxFont* pGfxFont = FontManager::Instance().GetFont( lpszFontName, nSize, dwColor, bBold, bItalic, bAntialias );
	if( pGfxFont )
	{
		pGfxFont->SetColor( dwColor );
		return new CClientFont( XUI_FontAttribute( lpszFontName, nSize, dwColor, bBold, bItalic, bAntialias ), pGfxFont );
	}
	return NULL;
}

static XUI_Font* _CreateFontEx( const XUI_FontAttribute& FontAttribute )
{
	return _CreateFont( FontAttribute.m_name.c_str(), FontAttribute.m_size, FontAttribute.m_color, FontAttribute.m_bold, FontAttribute.m_italic, FontAttribute.m_antialias );
}

static void _DestroyFont( XUI_Font* pFont )
{
	delete pFont;
}

void init_canvas()
{
	XGC::ui::XUI_SetClipping		= _SetClipping;
	XGC::ui::XUI_DrawText			= _DrawText;
	XGC::ui::XUI_DrawCharacter		= _DrawCharacter;
	XGC::ui::XUI_DrawRect			= _DrawRect;
	XGC::ui::XUI_DrawLine			= _DrawLine;
	XGC::ui::XUI_DrawPolygon		= _DrawPolygon;
	XGC::ui::XUI_DrawSprite			= _DrawSprite;
	XGC::ui::XUI_CreateSprite		= _CreateSprite;
	XGC::ui::XUI_CreateSpriteEx		= _CreateSpriteEx;
	XGC::ui::XUI_DestroySprite		= _DestroySprite;
	XGC::ui::XUI_CreateAnimation	= _CreateAnimation;
	XGC::ui::XUI_CreateAnimationEx	= _CreateAnimationEx;
	XGC::ui::XUI_DestroyAnimation	= _DestroyAnimation;
	XGC::ui::XUI_CreateFont			= _CreateFont;
	XGC::ui::XUI_CreateFontEx		= _CreateFontEx;
	XGC::ui::XUI_DestroyFont		= _DestroyFont;
}
