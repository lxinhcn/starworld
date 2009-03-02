#include "header.h"
#include "multipicture.h"
#include "Texture.h"
#include "Canvas.h"

namespace UILib
{
	CMultiPicture::CMultiPicture(void)
	: m_nRowCount( 1 )
	, m_nColCount( 1 )
	, m_nCurGroup( 0 )
	, m_nCurIndex( 0 )
	, m_nType( 1 )
	{
	}

	CMultiPicture::~CMultiPicture(void)
	{
	}

	void CMultiPicture::LoadProperty(TiXmlElement* pXMLElement )
	{
		__super::LoadProperty( pXMLElement );
		m_nRowCount = pXMLElement->IntAttribute( "RowCount" );
		m_nColCount = pXMLElement->IntAttribute( "ColCount" );
		m_nCurGroup = pXMLElement->IntAttribute( "CurGroup" );
		m_nCurIndex = pXMLElement->IntAttribute( "CurIndex" );

		m_nType		= pXMLElement->IntAttribute( "Type" );
		m_strNameHead = pXMLElement->Attribute( "NameHead" );
		if( m_nType == 1 )
		{
			m_pBkTex->SetName( m_strNameHead.c_str() );
		}
		UpdatePicture();
	}

	void CMultiPicture::SaveProperty(TiXmlElement* pXMLElement )
	{
		__super::SaveProperty( pXMLElement );
		pXMLElement->SetAttribute( "RowCount", m_nRowCount );
		pXMLElement->SetAttribute( "ColCount", m_nColCount );
		pXMLElement->SetAttribute( "CurGroup", m_nCurGroup );
		pXMLElement->SetAttribute( "CurIndex", m_nCurIndex );

		pXMLElement->SetAttribute( "Type", m_nType );
		pXMLElement->SetAttribute( "NameHead", m_strNameHead.c_str() );
	}

	bool CMultiPicture::SetProperty( const char* pszPropertyName, const char* pszValue )
	{
		if( stricmp( pszPropertyName, "RowCount" ) == 0 )
		{
			m_nRowCount = atoi( pszValue );
			UpdatePicture();
		}
		else if( stricmp( pszPropertyName, "ColCount" ) == 0 )
		{
			m_nColCount = atoi( pszValue );
			UpdatePicture();
		}
		else if( stricmp( pszPropertyName, "Group" ) == 0 )
		{
			SetGroup( atoi( pszValue ) );
		}
		else if( stricmp( pszPropertyName, "Index" ) == 0 )
		{
			SetIndex( atoi( pszValue ) );
		}
		else if( stricmp( pszPropertyName, "Type" ) == 0 )
		{
			if( stricmp( pszValue, "More files" ) == 0 )	m_nType = 0;
			else if( stricmp( pszValue, "Single file" ) )	m_nType = 1;

			if( m_nType == 1 )
			{
				m_pBkTex->SetName( m_strNameHead.c_str() );
			}
			UpdatePicture();
		}
		else if( stricmp( pszPropertyName, "BackGround" ) == 0 )
		{
			m_strNameHead = pszValue;
			if( m_nType == 1 )
			{
				m_pBkTex->SetName( m_strNameHead.c_str() );
			}
			return __super::SetProperty( pszPropertyName, pszValue );
		}
		else 
		{
			return __super::SetProperty( pszPropertyName, pszValue );
		}
		return true;
	}

	bool CMultiPicture::GetProperty( const char* pszPropertyName, char* pValueBuffer, int size )
	{
		if( stricmp( pszPropertyName, "RowCount" ) == 0 )
		{
			itoa( m_nRowCount, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "ColCount" ) == 0 )
		{
			itoa( m_nColCount, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "Group" ) == 0 )
		{
			itoa( m_nCurGroup, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "Index" ) == 0 )
		{
			itoa( m_nCurIndex, pValueBuffer, 10 );
		}
		else if( stricmp( pszPropertyName, "Type" ) == 0 )
		{
			strncpy( pValueBuffer, m_nType?"Single file":"More files", size );
		}
		else if( stricmp( pszPropertyName, "Texture" ) == 0 )
		{
			strncpy( pValueBuffer, m_strNameHead.c_str(), size );
		}
		else
		{
			return __super::GetProperty( pszPropertyName, pValueBuffer, size );
		}
		return true;
	}

	void CMultiPicture::SetGroup( int nGroup )
	{
		m_nCurGroup = nGroup;
		UpdatePicture();
	}

	void CMultiPicture::SetIndex( int nIndex )
	{
		m_nCurIndex = nIndex;
		UpdatePicture();
	}

	void CMultiPicture::UpdatePicture()
	{
		switch( m_nType )
		{
		case 0:	// multi picture file
			{
				if( m_strNameHead.empty() )	return;
				char szFullName[_MAX_PATH];
				char szFilename[_MAX_PATH];
				char szExt[_MAX_PATH];

				const char* pstr = m_strNameHead.c_str();
				size_t i = 0;
				while( *pstr != '.')
				{
					if( *pstr == 0 )	return;
					szFilename[i++] = *pstr++;
					if( i >= sizeof( szFilename ) ) return;
				}
				pstr++;
				szFilename[i] = 0;
				i = 0;
				while( *pstr != 0 )
				{
					szExt[i++] = *pstr++;
					if( i >= sizeof( szExt ) ) return;
				}
				szExt[i] = 0;
				_snprintf( szFullName, sizeof( szFilename ), "%s%d.%s", szFilename, m_nCurGroup*m_nCurIndex, szExt );
				m_pBkTex->SetName( szFullName );
				m_pBkTex->SetUV( 0.0f, 0.0f, 1.0f, 1.0f );
			}
			break;
		case 1:	// signel picture file
			{
				if( m_nRowCount && m_nColCount )
				{
					float V0 = ( float )m_nCurGroup/m_nRowCount;
					float V1 = ( m_nCurGroup + 1.0f )/m_nRowCount;

					float U0 = ( float )m_nCurIndex/m_nColCount;
					float U1 = ( m_nCurIndex + 1.0f )/m_nColCount;
					m_pBkTex->SetUV( U0, V0, U1, V1 );
				}
			}
		}
	}

	//void CMultiPicture::RenderSelf( const CUIRect& clipper )
	//{

	//}
}
