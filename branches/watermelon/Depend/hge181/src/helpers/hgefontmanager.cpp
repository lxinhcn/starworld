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
#include "..\..\include\hgefontmanager.h"
#include "..\..\include\hgefontex.h"

__inline hgeFontManagerStub& getFontManager(){ static hgeFontManagerStub mFontManager; return mFontManager; }

hgeFontManagerStub::hgeFontManagerStub()
{
	hge = hgeCreate(HGE_VERSION);
}

hgeFontManagerStub::~hgeFontManagerStub()
{
	for ( FontMap::iterator i = Fonts.begin(); i != Fonts.end(); ++i )
	{
		delete i->second;
	}
	Fonts.clear();

	hge->Release();
}

hgeFontEx* hgeFontManagerStub::getFont(const char* fontname,unsigned int fontsize)
{
	if (!fontname)
		return NULL;

	FontMap::iterator iter = Fonts.find( std::make_pair( fontname, fontsize ) );

	if( iter == Fonts.end() )
	{
		hgeFontEx* pFont = new hgeFontEx();
		if( pFont )
		{
			if ( !pFont->Create( fontname, fontsize ) )
			{
				delete pFont;
				return	0;
			}

			FontMap::_Pairib ib = Fonts.insert( std::make_pair( std::make_pair( fontname, fontsize ), pFont ) );

			return ib.second?ib.first->second:NULL;
		}
	}

	return iter->second;
}

hgeFontHelper::hgeFontHelper( const char* face, size_t size )
{
	pFont = getFontManager().getFont( face, size );
}