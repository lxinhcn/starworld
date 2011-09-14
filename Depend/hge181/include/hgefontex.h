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
#ifndef _HGE_TT_FONT_H_
#define _HGE_TT_FONT_H_

#include "hge.h"
#include "hgerect.h"
#include <vector>

class hgeSprite;
class hgeFontGlyph;
class hgeFontFace;

struct FontPage;

/**
 * hgeTTFont
 *
 * You can use this class to render some multibyte language texts.
 * And this is the first class you should pay attention to .
 * I mean , donot care hgeTTFace and hgeGlyph. 
 *
 * And you cannot new a hgeTTFont by yourself, it recommended that 
 * using hgeTTFontManager to get a hgeTTFont object. It's safe and easy 
 * if you do that.
 *
 * I'll keep the interface as same as hgeFontEx class. 
 */
class EXPORT hgeFontEx
{
public:
	/**
	 * a tiny class that describe a Size.
	 *
	 */
	struct Size
	{
		int width, height;

		Size( int w = 0, int h = 0 ) : width( w ), height( h )
		{
		}
	};
private:
	friend class hgeFontManagerStub;
	/**
	 * Constructor
	 *
	 * Donot care it because you'll never new a hgeTTFont by yourself
	 */
	hgeFontEx();
	
	/**
	 * Destructor
	 *
	 */
	virtual ~hgeFontEx();
	
public:
	/**
	 * Render
	 *
	 * renders the text string to the screen
	 * @param x Screen-x coordinate
	 * @param y Screen-y coordinate
	 * @param align Alignment specificator. May be one of these: HGETEXT_LEFT, HGETEXT_RIGHT, HGETEXT_CENTER. 
	 * @param string the string to be rendered
	 */
	void Render( float x, float y, int align, const wchar_t *string );
	void Render( float x, float y, const wchar_t* text, bool hcenter = false, bool vcenter = false );

	void Print( float x, float y, int align, const wchar_t *format, ... );

	bool Create( const char* fontname, unsigned int fontsize );
	/**
	 * GetColor
	 *
	 */
	unsigned int GetColor();
	
	/**
	 * SetColor
	 *
	 */
	void SetColor( unsigned int color );

	/** 
	 * SetSpacing
	 *
	 */
	void SetSpacing( float spacing );

	/** 
	 * GetSpacing
	 *
	 */
	float GetSpacing();

	/**
	 * GetStringWidth
	 *
	 */
	float GetStringWidth( const wchar_t *str );

	/**
	 * GetHeight
	 *
	 */
	float GetHeight();

	///////////////////////////////////////////////////////////////////////////////////////
	/**
	 * Donot care these functions below, i recommend you donot use them.
	 *
	 *
	 */
	bool			attach( hgeFontFace* Face, unsigned int size );
	Size			getDimension( const wchar_t* text );
	unsigned int	getGlyphByChar( wchar_t c );
	int				getCharacterFromPos( const wchar_t* text, int pixel_x );
	int				getWidthFromCharacter( wchar_t c );
	void			setKerningWidth( int kerning );
	void			setKerningHeight( int kerning );

	int				getKerningWidth( const wchar_t* thisLetter=0, const wchar_t* previousLetter=0 );
	int				getKerningHeight();

private:

	bool			attached;
	unsigned int	size;

	HGE				*hge;
	hgeFontGlyph	*Glyphs;
	hgeFontFace		*tt_face;

	FontPage		*page;

	int			GlobalKerningWidth;
	int			GlobalKerningHeight;

	hgeSprite	*spr;
	int			mGlyphsCount;
};

#endif //end _HGE_TT_FONT_H_
