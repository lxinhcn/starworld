/**
 * HGE FreeType2 font system support. This system makes HGE
 * support multibyte language. 
 *
 * Special thanks to uirou, he wrote some codes to make Irrlicht
 * engine support multibyte language. And now i use some of his codes
 * here to make HGE do the same fever to HGE-users. 
 * 
 * Original author in Irrlicht : uirou
 * Modified version in HGE :     Kevin Lynx
 * mail : zmhn320@163.com
 * Date : 8/18/2007
 *
 * this class depends on HGE 1.6 (maybe later version is avaiable too )and
 * FreeType 2.2.1
 */
#include "..\..\include\hgefontex.h"
#include "..\..\include\hgefont.h"
#include "..\..\include\hgefontmanager.h"
#include <ft2build.h>
#include <freetype/freetype.h>

/**
 * hgeTTFace
 *
 */
struct FontPage
{
	FontPage()
		: hTexture(0)
		, hge(NULL)
		, offsetX(0)
		, offsetY(0)
		, width(0)
		, height(0)
		, next(NULL)
		, bFull(false)
	{
		hge = hgeCreate(HGE_VERSION);
		if( hge )
		{
			width	= hge->System_GetState(HGE_MAXTEXTUREW);
			height	= hge->System_GetState(HGE_MAXTEXTUREH);
			hTexture = hge->Texture_Create( width, height );
			hge->Release();
		}
	}

	~FontPage()
	{
		if(hge)
		{
			hge->Texture_Free( hTexture );
		}
		width	= 0;
		height	= 0;
	}

	bool cache( FT_GlyphSlot glyph, int w, int h )
	{
		FT_Bitmap  bits;

		bits = glyph->bitmap;

		unsigned char* pt = bits.buffer;
		unsigned int txw = hge->Texture_GetWidth(hTexture);
		unsigned int* texp = (unsigned int*)hge->Texture_Lock( hTexture, false, offsetX, offsetY, w, h );

		for (int i = 0;i < bits.rows;i++)
		{
			unsigned int *rowp = texp;
			for (int j = 0;j < bits.width;j++)
			{
				if (*pt)
				{
					*rowp = *pt;
					*rowp *= 0x01010101;
				}
				else
				{
					*rowp = 0;
				}
				pt++;
				rowp++;
			}
			texp += txw;
		}

		hge->Texture_Unlock(hTexture);
		offsetX += w;

		if(offsetX >= hge->System_GetState(HGE_MAXTEXTUREW) )
		{
			offsetX = 0;
			offsetY += h;
			if( offsetY >= hge->System_GetState(HGE_MAXTEXTUREH) )
			{
				bFull = true;
				next = new FontPage();
			}
		}

		return true;
	}

	HGE* hge;
	HTEXTURE hTexture;
	int width;
	int height;
	int offsetX;
	int offsetY;
	bool bFull;

	FontPage* next;
};

class hgeFontFace
{
public:
	bool loaded;
	FT_Library	library;
	FT_Face		face;

	hgeFontFace()
	{
		loaded = false;
	}

	hgeFontFace( const hgeFontFace& rsh )
	{
		loaded = rsh.loaded;
		library = rsh.library;
		face = rsh.face;

		const_cast< hgeFontFace& >( rsh ).loaded = false;
	}

	virtual ~hgeFontFace()
	{
		if (loaded)
		{
			FT_Done_Face(face);
			FT_Done_FreeType(library);
		}
	}

	hgeFontFace& operator=( const hgeFontFace& rsh )
	{
		loaded = rsh.loaded;
		library = rsh.library;
		face = rsh.face;

		const_cast< hgeFontFace& >( rsh ).loaded = false;
	}

	bool load(const char* filename)
	{
		if (FT_Init_FreeType( &library ))
		{
			return	false;
		}

		if (FT_New_Face( library,filename,0,&face ))
		{
			return	false;
		}

		loaded = true;
		return true;
	}
};

typedef std::map< std::string, hgeFontFace > hgeFontFaceContainer;

hgeFontFaceContainer	gFontFaceContainer;

/**
 * hgeTTGlyph
 *
 */
class hgeFontGlyph
{
public:
	bool cached;

	hgeFontGlyph()
		: tex(0)
	{
	}

	~hgeFontGlyph()
	{
	}

	void cache(FT_Face face, unsigned int idx, unsigned int size, FontPage* page )
	{
		FT_Set_Pixel_Sizes(face,0,size);
		if (!FT_Load_Glyph(face,idx,FT_LOAD_NO_HINTING|FT_LOAD_NO_BITMAP))
		{
			FT_GlyphSlot glyph = face->glyph;
			if (glyph->format == ft_glyph_format_outline )
			{
				if (!FT_Render_Glyph( glyph, FT_RENDER_MODE_NORMAL))
				{
					FT_Bitmap  bits = glyph->bitmap;

					tex = page->hTexture;

					offsetx = glyph->bitmap_left;
					offsety = glyph->bitmap_top;

					imgw = bits.width;
					imgh = bits.rows;

					top = page->offsetY;
					left = page->offsetX;
					cached = page->cache(glyph, imgw, imgh);
				}
			}
		}
	}
	
	int offsetx;
	int offsety;

	int top;
	int left;
	int imgw;
	int imgh;
	HTEXTURE tex;
};

////////////////////////////////////////////////////////
hgeFontEx::hgeFontEx()
	: Glyphs( NULL )
{
	hge = hgeCreate(HGE_VERSION);

	attached = false;
	GlobalKerningWidth = 0;
	GlobalKerningHeight = 0;
	spr = new hgeSprite( 0, 0, 0, 0, 0 );
	page = new FontPage();
}

hgeFontEx::~hgeFontEx()
{
	attached = false;

	free( Glyphs );
	delete spr;
	hge->Release();
}

bool hgeFontEx::Create( const char* fontname, unsigned int fontsize )
{
	hgeFontFaceContainer::iterator iter = gFontFaceContainer.find( fontname );
	if( iter == gFontFaceContainer.end() )
	{
		hgeFontFace face;
		if( !face.load( fontname ) )
			return false;

		hgeFontFaceContainer::_Pairib ib = gFontFaceContainer.insert( std::make_pair( fontname, face ) );
		if( !ib.second )
		{
			return false;
		}

		iter = ib.first;
	}

	attach( &iter->second, fontsize );

	return true;
}

bool hgeFontEx::attach( hgeFontFace* Face, unsigned int size )
{
	if (!hge)
		return false;

	tt_face = Face;

	this->size = size;

	hgeFontGlyph* pTmpGlyph = (hgeFontGlyph*)realloc( Glyphs, sizeof(hgeFontGlyph)*( tt_face->face->num_glyphs ) );
	if( !pTmpGlyph )
		return false;

	Glyphs = pTmpGlyph;
	mGlyphsCount = tt_face->face->num_glyphs;

	for (int i = 0;i < mGlyphsCount;i++)
	{
		Glyphs[i].cached = false;
	}
	attached = true;
	return	true;
}

unsigned int hgeFontEx::getGlyphByChar(wchar_t c)
{
	unsigned int idx = FT_Get_Char_Index( tt_face->face, c );
	if (idx && !Glyphs[idx - 1].cached)
	{
		FontPage* curPage = page;
		while( curPage && curPage->bFull )
			curPage = curPage->next;

		if( curPage )
			Glyphs[idx - 1].cache(tt_face->face, idx, size, curPage );
	}
	return	idx;
}

hgeFontEx::Size  hgeFontEx::getDimension(const wchar_t* text)
{
	Size dim(0, size);

	for(const wchar_t* p = text; *p; ++p)
	{
		dim.width += getWidthFromCharacter(*p) + GlobalKerningWidth;
	}
	return dim;
}

inline int hgeFontEx::getWidthFromCharacter(wchar_t c)
{
	unsigned int n = getGlyphByChar(c);
	if ( n > 0)
	{
		int w = Glyphs[n - 1].imgw;
		int left = Glyphs[n - 1].offsetx;
		if (w + left > 0)
			return w + left;
	}

	if (c >= 0x2000)
	{
		return	size;
	} 
	else 
	{
		return	size / 2;
	}
}

unsigned int hgeFontEx::GetColor()
{
	return spr?spr->GetColor():0;
}

void hgeFontEx::SetColor( unsigned int color )
{
	spr->SetColor(color);
}

void hgeFontEx::SetSpacing( float spacing )
{
	this->setKerningWidth( static_cast<int>( spacing ) );
}

float hgeFontEx::GetSpacing()
{
	return static_cast<float>( getKerningWidth() );
}

float hgeFontEx::GetStringWidth( const wchar_t *str )
{
	Size textDimension = getDimension( str );
	return static_cast<float>( textDimension.width );
}

float hgeFontEx::GetHeight()
{
	Size textDimension = getDimension( L"A" );
	return static_cast<float>( textDimension.height );
}

void hgeFontEx::Render( float offsetX, float offsetY, int align, const wchar_t *text )
{
	if( align != HGETEXT_LEFT )
	{
		Size textDimension;
		textDimension = getDimension(text);

		if( align == HGETEXT_CENTER )
			offsetX = offsetX - ( textDimension.width >>1 );
		
		if( align == HGETEXT_RIGHT )
			offsetX = offsetX - textDimension.width;
	}

	unsigned int n;
	
	while(*text)
	{
		n = getGlyphByChar(*text);
		if ( n > 0)
		{
			spr->SetTexture( Glyphs[n-1].tex );
			spr->SetTextureRect( Glyphs[n-1].left, Glyphs[n-1].top, Glyphs[n-1].imgw, Glyphs[n-1].imgh );
			spr->Render( offsetX + Glyphs[n-1].offsetx, offsetY + size - Glyphs[n-1].offsety );
 
			offsetX += getWidthFromCharacter(*text) + GlobalKerningWidth;
		} 
		else 
		{
			offsetX += getWidthFromCharacter(*text) + GlobalKerningWidth;
		}
		++text;
	}
}

void hgeFontEx::Render( float x, float y )
{
	spr->SetTexture( page->hTexture );
	spr->SetTextureRect(0,0,hge->System_GetState(HGE_MAXTEXTUREW),hge->System_GetState(HGE_MAXTEXTUREH));
	spr->Render(x,y);
}

void hgeFontEx::Print( float x, float y, int align, const wchar_t *format, ... )
{
	va_list l;
	va_start( l, format );

	wchar_t text[1024];
	wvsprintfW( text, format, l );

	Render( x, y, align, text );
}


void hgeFontEx::Render( float offsetX, float offsetY, const wchar_t* text, bool hcenter, bool vcenter )
{
	
    if( hcenter || vcenter )
	{
		Size textDimension;
		textDimension = getDimension(text);

		if (hcenter)
			offsetX = offsetX - ( textDimension.width >> 1 );

		if (vcenter)
			offsetY = offsetY - ( textDimension.height >> 1 );
	}

	unsigned int n;
	
	while(*text)
	{
		n = getGlyphByChar(*text);
		if ( n > 0)
		{
			spr->SetTexture( Glyphs[n-1].tex );
			spr->SetTextureRect( Glyphs[n-1].left, Glyphs[n-1].top, Glyphs[n-1].imgw, Glyphs[n-1].imgh );
			spr->Render( offsetX + Glyphs[n-1].offsetx, offsetY + size - Glyphs[n-1].offsety );
 
			offsetX += getWidthFromCharacter(*text) + GlobalKerningWidth;
		} 
		else 
		{
			offsetX += getWidthFromCharacter(*text) + GlobalKerningWidth;
		}
		++text;
	}
}

//! Calculates the index of the character in the text which is on a specific position.
int hgeFontEx::getCharacterFromPos(const wchar_t* text, int pixel_x)
{
	int x = 0;
	int idx = 0;

	while (text[idx])
	{
		x += getWidthFromCharacter(text[idx]);

		if (x >= pixel_x)
			return idx;

		++idx;
	}

	return -1;
}

void hgeFontEx::setKerningWidth ( int kerning )
{
	GlobalKerningWidth = kerning;
}

int hgeFontEx::getKerningWidth(const wchar_t* thisLetter, const wchar_t* previousLetter)
{
	return GlobalKerningWidth;
}

void hgeFontEx::setKerningHeight ( int kerning )
{
	GlobalKerningHeight = kerning;
}

int hgeFontEx::getKerningHeight ()
{
	return GlobalKerningHeight;
}

