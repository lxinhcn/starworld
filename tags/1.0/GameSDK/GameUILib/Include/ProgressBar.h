#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#pragma once
#include "UIElement.h"

namespace UILib
{
	struct NMHDR_PROCESS
	{
		NMUIHDR hdr;
		int		pos;
	};

	class CTexture;
	class CProgressBar :	public XObjTypeT< CUIElement, TypeProgress >
	{
	public:
		CProgressBar();
		~CProgressBar();
	protected:
		int m_nMax;
		int m_nMin;
		int m_nValue;
		int	m_nType;

		CTexture*	m_pTexture;

		virtual void RenderSelf( CONST CUIRect& clipper );
		virtual void OnMove( CUIRect& OldRect ){ SetPos( m_nValue ); }
	public:
		void SetMax(int nValue);
		void SetMin(int nValue);
		void SetType( int nType ){ m_nType = nType; SetPos( m_nValue ); }

		int GetMax() {return m_nMax;}
		int GetMin() {return m_nMin;}
		int GetPos() {return m_nValue;}

		void SetPos( int nValue );
		void Step( int nDelta ){ SetPos( m_nValue + nDelta ); }
		void Reset(){ m_nValue = m_nMin; }


		virtual LPCSTR GetLable(){ return "PROGRESSBAR"; }
		
		// 设置控件属性
		virtual bool SetProperty( const char* pszPropertyName, const char* pszValue );

		virtual bool GetProperty( const char* pszPropertyName, char* pValueBuffer, int size );

		virtual void LoadProperty( TiXmlElement* pXMLElement );
		//存储属性
		virtual void SaveProperty( TiXmlElement* pXMLElement );

	private:
		float m_fTextureLen;
	};
}
#endif
