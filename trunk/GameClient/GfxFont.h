#ifndef GDIFONT_H
#define GDIFONT_H

class GfxFont
{
public:
	GfxFont( _lpctstr lpsFontName, int nFaceSize, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bAntialias = TRUE);
	~GfxFont(void);

public:
	// 渲染文本
	void	Print( float x, float y, const char *format, ... );
	void	Render(float x, float y, const wchar_t* text );
	inline void	Render(float x, float y, const wchar_t text );

	// 设置与获取颜色
	void	SetColor( _uint32 dwColor, int i = -1 );
	_uint32	GetColor( int i = 0 );

	// 获取文本宽高
	SIZE	GetTextSize( const wchar_t* text );

	LONG	GetCharacterWidth( const wchar_t character );
	LONG	GetCharacterHeight()const;

	// 根据坐标获取字符
	wchar_t	GetCharacterFromPos( const wchar_t* text, float pixel_x, float pixel_y );

	// 设置字间距
	void	SetKerningWidth( float kerning );
	void	SetKerningHeight( float kerning );

	// 获取字间距
	float	GetKerningWidth()const;
	float	GetKerningHeight()const;

	// 字体大小
	float	GetFontSize()const;

private:
	// 根据字符获取轮廓
	unsigned int	GetGlyphByCharacter( wchar_t c );
	inline float	GetWidthFromCharacter( wchar_t c, bool original = false );
	inline void		CacheCharacter(unsigned int idx, wchar_t c);

	typedef struct tagEngineFontGlyph
	{
		HTEXTURE	t;
		float		w;
		float		h;
		float		x;
		float		y;
		float		c;
	}TENGINEFONTGLYPH;

	static const unsigned int font_count = 0xFFFF;// = sizeof(wchar_t);
	TENGINEFONTGLYPH	m_Glyphs[font_count];
	UINT				m_nAntialias;//反锯齿
	LONG				m_nAscent;//基线
	_uint32				m_dwFontColor;
	float				m_nFontSize;
	float				m_nKerningWidth;
	float				m_nKerningHeight;

	HGE*				m_pHGE;
	hgeSprite*			m_pSprite;

	// GDI设备
	HDC					m_hMemDC;
	HFONT				m_hFont;
};
#endif//GDIFONT_H
