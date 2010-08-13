/*-----------------------------------------------------------------------------
	Lua Studio

Copyright (c) 1996-2008 Michal Kowalski
-----------------------------------------------------------------------------*/

#ifndef _config_settings_
#define _config_settings_


struct CConfigSettings
{
  static LOGFONT* const fonts[];
  static CFont* const cfonts[];
  static COLORREF* text_color[];
  static COLORREF* bkgnd_color[];
  static COLORREF* color_syntax[];
  static BYTE* syntax_font_style[];
};


#endif
