#pragma once
#include "Picture.h"
namespace UILib
{
	class CStatic	:	public	XObjTypeT< CPicture, TypeStatic >
	{
	public:
		CStatic(void);
		~CStatic(void);

		virtual LPCSTR GetLable(){ return "STATIC"; }
		
		virtual void LoadProperty(TiXmlElement* pXMLElement );
		virtual void SaveProperty(TiXmlElement* pXMLElement );

		virtual bool SetProperty( const char* pszPropertyName, const char* pszValue );
		virtual bool GetProperty( const char* pszPropertyName, char* pValueBuffer, int size );

		void SetText( LPCSTR lpszText ){ m_strText = lpszText; }
		void SetTextColor( DWORD dwColor ){ m_dwColor = dwColor; }

		LPCSTR	GetText(){ return m_strText.c_str(); }
		DWORD	GetTextColor(){ return m_dwColor; }

	protected:
		virtual void RenderSelf(const CUIRect& clipper );

		std::string m_strText;
		DWORD		m_dwColor;
		DWORD		m_dwFormat;
	};
}
