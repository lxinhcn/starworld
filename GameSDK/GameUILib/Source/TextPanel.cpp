#include "header.h"
#include "gui.h"

CStringItem::CStringItem():
	sText(NULL)
{
}

CStringItem::~CStringItem()
{
	SAFE_DELETE_ARRAY(sText);
}

void CStringItem::ComputeBreaks(HDC hdc, UINT uWidth)
{
	vctBreaks.clear();
	size_t i=0;
	size_t j=0;
	bool bInMultiByte=false;

	vctBreaks.push_back(0);
	while(sText[i])
	{
		char c=sText[i];
		if (c=='\n')
		{
			bInMultiByte=false;

			j=i+1;
			vctBreaks.push_back(j);
		}
		else if (c=='\t' || c==' ')
		{
			bInMultiByte=false;

			j=i+1;
		}
		else
		{
			if (c>127)
			{
				bInMultiByte=!bInMultiByte;

				if (bInMultiByte)
					j=i;
			}
			else
			{
				bInMultiByte=false;
			}

			size_t iBegin=vctBreaks[vctBreaks.size()-1];
			SIZE sz;
			UINT uLineWidth=GetTextExtentPoint32(hdc, &sText[iBegin], (int)(i-iBegin), &sz);
			if ((UINT)sz.cx>uWidth)
			{
				if (j>0)
					vctBreaks.push_back(j);
				else
					vctBreaks.push_back(i-1);
			}
		}

		i++;
	}
	//vctBreaks.push_back(i);
}

TEXTPANEL_PARAMS::TEXTPANEL_PARAMS():
	PANEL_PARAMS(),
	iSize(0), uWeight(0), bItalic(false), dwFontColor(0),
	uMargin(0)
{
	strFacename[0]=0;
}

void TEXTPANEL_PARAMS::ReadFromSection(CSectionNode* pSection, const char* sPath)
{
	PANEL_PARAMS::ReadFromSection(pSection, sPath);

	const char* consFontSize="FontSize";
	const char* consFontWeight="FontWeight";
	const char* consFontItalic="Italic";
	const char* consFontFacename="FontFacename";
	const char* consFontColor="FontColor";
	
	const char* consMargin="Margin";
	CKeyNode* pNode=NULL;
	if (pNode=pSection->GetKey(consFontSize))
	{
		pNode->GetInt(1, &iSize);
	}

	if (pNode=pSection->GetKey(consFontWeight))
	{
		pNode->GetInt(1, &uWeight);
	}

	if (pNode=pSection->GetKey(consFontItalic))
	{
		int iItalic;
		pNode->GetInt(1, &iItalic);
		bItalic=iItalic?true:false;
	}

	if (pNode=pSection->GetKey(consFontFacename))
	{
		strcpy(strFacename,pNode->GetValue());
	}

	if (pNode=pSection->GetKey(consFontColor))
	{
		float fr, fg, fb, fa;
		pNode->GetFloat(4, &fr, &fg, &fb, &fa);
		dwFontColor=D3DXCOLOR(fr, fg, fb, fa);
	}

	if (pNode=pSection->GetKey(consMargin))
	{
		pNode->GetInt(1, &uMargin);
	}
}

CTextPanel::CTextPanel(void):
	m_pFont(NULL), m_uFontHeight(0), m_uTextFieldWidth(0), m_uTextFieldHeight(0), m_uTextHeight(0)
{
}

CTextPanel::~CTextPanel(void)
{
	for(size_t i=0; i<m_vctStrings.size(); i++)
	{
		SAFE_DELETE(m_vctStrings[i]);
	}
	m_vctStrings.clear();
}

void CTextPanel::RenderSelf(const CClipper& clipper)
{
	CPanel::RenderSelf(clipper);

	UINT uPosV=(m_uHeight-m_uTextFieldHeight)/2;
	UINT uPosH=(m_iRight-m_iLeft-m_uTextFieldWidth)/2;
	for(size_t i=0; i<m_vctStrings.size(); i++)
	{
		PStringItem pItem=m_vctStrings[i];
		for(size_t j=0; j<pItem->vctBreaks.size(); j++)
		{
			if (uPosV>m_uY && uPosV+m_uFontHeight-m_uY<(UINT)(m_iBottom-m_iTop))
			{
				char sBuffer[256];
				size_t iBegin=pItem->vctBreaks[j];
				size_t iEnd;
				if (j==pItem->vctBreaks.size()-1)
					iEnd=strlen(pItem->sText);
				else
					iEnd=pItem->vctBreaks[j+1];
				ZeroMemory(sBuffer, sizeof(sBuffer));
				memcpy(sBuffer, &pItem->sText[iBegin], iEnd-iBegin);

				RECT rct={0, 0, m_uTextFieldWidth, m_uFontHeight};
				OffsetRect(&rct, uPosH, uPosV-m_uY);
				CPoint2D pt(0, m_uY);
				pt=ClientToScreen(pt);
				OffsetRect(&rct, pt.x, pt.y);

				if (m_pGuiSystem)
				{
					m_pGuiSystem->RenderFont(m_pFont, sBuffer, &rct, m_fZ, DT_LEFT|DT_BOTTOM|DT_SINGLELINE);
				}
			}
			uPosV+=m_uFontHeight;
		}
	}
}

void CTextPanel::SetUIFont(CUIFont* pFont)
{
	m_pFont=pFont;
	if (m_pFont)
	{
		SIZE sz;
		GetTextExtentPoint32(m_pFont->GetFont()->GetDC(), "A", 1, &sz);
		m_uFontHeight=sz.cy;
	}
	else
		m_uFontHeight=0;
}

void CTextPanel::SetMargin(UINT uMargin)
{
	m_uTextFieldHeight=m_uHeight-2*uMargin;
	m_uTextFieldWidth=m_iRight-m_iLeft-2*uMargin;
}

void CTextPanel::AddString(const char* sText)
{
	PStringItem pItem=new CStringItem;
	DynamicCopy(&pItem->sText, sText);
	pItem->ComputeBreaks(m_pFont->GetFont()->GetDC(), m_uTextFieldWidth);
	m_vctStrings.push_back(pItem);
	m_uTextHeight+=((UINT)pItem->vctBreaks.size())*m_uFontHeight;
	while (m_uTextHeight>m_uTextFieldHeight)
	{
		std::vector<PStringItem>::iterator itr=m_vctStrings.begin();
		if (itr==m_vctStrings.end())
			break;
		PStringItem pTmpItem=*itr;
		m_uTextHeight-=((UINT)pTmpItem->vctBreaks.size())*m_uFontHeight;
		SAFE_DELETE(pTmpItem);
		m_vctStrings.erase(itr);
	}
}

int CTextPanel::ScrollVerticle(int iOffset)
{
	div_t dvResult=div(iOffset, m_uFontHeight);
	if (dvResult.rem>0)
		iOffset=(dvResult.quot+1)*m_uFontHeight;
	else if (dvResult.rem<0)
		iOffset=(dvResult.quot-1)*m_uFontHeight;

	return CPanel::ScrollVerticle(iOffset);
}

HRESULT CreateTextPanel(CTextPanel* pTextPanel, const TEXTPANEL_PARAMS* pTextPanelParams)
{
	HRESULT hr=S_OK;
	if (FAILED(CreatePanel(pTextPanel, pTextPanelParams)))
	{
		hr=E_FAIL;
		goto e_exit;
	}

	pTextPanel->SetMargin(pTextPanelParams->uMargin);
	CUIFont *pFont;
	if (FAILED(g_pUIFontManager->LoadFont(pTextPanelParams->iSize, pTextPanelParams->uWeight, 
		pTextPanelParams->bItalic, pTextPanelParams->strFacename, pTextPanelParams->dwFontColor, 
		&pFont)))
	{
		hr=E_FAIL;
		goto e_exit;
	}
	pTextPanel->SetUIFont(pFont);

e_exit:
	return hr;
}

HRESULT CreateTextPanelFromFile(CTextPanel* pTextPanel, CSectionNode* pSection, const char* sPath)
{
	TEXTPANEL_PARAMS* pTextPanelParams=new TEXTPANEL_PARAMS;
	pTextPanelParams->ReadFromSection(pSection, sPath);

	HRESULT hr=CreateTextPanel(pTextPanel, pTextPanelParams);
	SAFE_DELETE(pTextPanelParams);
	return hr;
}