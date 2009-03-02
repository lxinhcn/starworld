#pragma once
#include "Picture.h"
namespace UILib
{
	class CTexture;
	class CMultiPicture	:	public XObjTypeT< CPicture, TypeMultiPicture >
	{
	public:
		CMultiPicture(void);
		~CMultiPicture(void);

		virtual const char* GetLable(){ return "MULTIPICTURE"; }

		virtual void LoadProperty(TiXmlElement* pXMLElement );
		virtual void SaveProperty(TiXmlElement* pXMLElement );

		virtual bool SetProperty( const char* pszPropertyName, const char* pszValue );
		virtual bool GetProperty( const char* pszPropertyName, char* pValueBuffer, int size );

		// method
		void SetAlpha( float fAlpha );
		void SetIndex( int	nIndex );
		void SetGroup( int	nGroup );
		
		void SetAttrib( int nRow, int nCol ){ m_nRowCount = nRow; m_nColCount = nCol; }

	protected:
		void UpdatePicture();

	protected:
		std::string	m_strNameHead;
		int			m_nRowCount;	// 行总数
		int			m_nColCount;	// 列总数

		int			m_nCurGroup;	// 当前组索引	- 行
		int			m_nCurIndex;	// 当前图片索引	- 列
		int			m_nType;		// 类型 0 - 单张图片，UV设定
									// 类型 1 - 多张图片

		int			m_nUpdateTime;	// 图片更新时间，毫秒计
	};
}
