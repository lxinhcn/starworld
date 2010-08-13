#pragma once
#include "Picture.h"
namespace UILib
{
	class CRenderTarget		:	public XObjTypeT< CPicture, TypeRenderTarget >
	{
	public:
		CRenderTarget(void);
		~CRenderTarget(void);

		virtual LPCSTR GetLable(){ return "RENDERTARGET"; }

		virtual void LoadProperty(TiXmlElement* pXMLElement );
		virtual void SaveProperty(TiXmlElement* pXMLElement );

		virtual bool SetProperty( const char* pszPropertyName, const char* pszValue );
		virtual bool GetProperty( const char* pszPropertyName, char* pValueBuffer, int size );

		void	SetModelName( LPCSTR lpszModelName );
		LPCSTR	GetModelName();

	protected:
		void RenderSelf(const CUIRect& clipper);

		uimsg BOOL OnCreate( WPARAM wParam, LPARAM lParam );
		DECLARE_UIMSG_MAP()

	protected:
		std::string m_strModelName;
	};
}
