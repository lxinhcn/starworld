#ifndef TEXTPANEL_H
#define TEXTPANEL_H

#pragma once

typedef struct CStringItem
{
	char* sText;
	std::vector<size_t> vctBreaks;

	CStringItem();
	~CStringItem();

	void ComputeBreaks(HDC hdc, UINT uWidth);
}*PStringItem;

struct TEXTPANEL_PARAMS: public PANEL_PARAMS
{
	UINT uMargin;

	int iSize;
	UINT uWeight;
	bool bItalic;
	char strFacename[256];
	DWORD dwFontColor;

	TEXTPANEL_PARAMS();
	void ReadFromSection(CSectionNode* pSection, const char* sPath);
};

class CTextPanel;
HRESULT CreateTextPanel(CTextPanel* pTextPanel, const TEXTPANEL_PARAMS* pTextPanelParams);

class CTextPanel :
	public CPanel
{
public:
	CTextPanel(void);
	~CTextPanel(void);
protected:
	CUIFont* m_pFont;
	UINT m_uFontHeight;
	UINT m_uTextFieldWidth;
	UINT m_uTextFieldHeight;
	UINT m_uTextHeight;

	std::vector<PStringItem> m_vctStrings;

	void RenderSelf(const CClipper& clipper);
public:
	void SetUIFont(CUIFont* pFont);
	void SetMargin(UINT uMargin);

	void AddString(const char* sText);
	int ScrollVerticle(int iOffset);
};

HRESULT CreateTextPanelFromFile(CTextPanel* pTextPanel, CSectionNode* pSection, const char* sPath);

#endif
