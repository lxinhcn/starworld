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
#ifndef HGE_TT_FONT_MANAGER_H_
#define HGE_TT_FONT_MANAGER_H_

#include <map>
#include <string>
#include "hge.h"

class hgeFontEx;

/**
* TTFontManager
*
* Manage ttf font to support multibyte language
* Although this class is not a real singleton ,
* but to use it safely, make sure you donot create more than 
* one instance.
* 
*/
class hgeFontManagerStub
{
friend hgeFontManagerStub& getFontManager();
private:
	hgeFontManagerStub();
	~hgeFontManagerStub();

public:
	hgeFontEx* getFont(const char* filename,unsigned int fontsize);

private:
	typedef std::map< std::pair< std::string, unsigned int >, hgeFontEx* > FontMap;

	FontMap Fonts;

	HGE *hge;
};

hgeFontManagerStub& getFontManager();

class hgeFontEx;
struct EXPORT hgeFontHelper
{
	hgeFontHelper( const char* face, size_t size );

	hgeFontEx* operator->(){ return pFont; }

private:
	hgeFontEx* pFont;
};
#endif //end HGE_TT_FONT_MANAGER_H_