#include "stdafx.h"
#include "file_helper.h"
////////////////////////////////////////////////////////////////////////////
// CXFileFind implementation
namespace file_helper
{
	bool get_datetime_filename( LPTSTR lpszBuffer, DWORD dwSize )
	{
		TCHAR szDate[128];
		TCHAR szTime[128];
		_tstrdate( szDate );
		_tstrtime( szTime );
		TCHAR* p = szDate;
		while( p = _tcschr( p, '/' ) )
		{
			*p = '-';
		}

		p = szTime;
		while( p = _tcschr( p, ':' ) )
		{
			*p = '-';
		}

		_sntprintf( lpszBuffer, dwSize, _TEXT( "%s_%s"), szDate, szTime );

		return true;
	}

	CXFileFind::CXFileFind()
	{
		m_pFoundInfo = NULL;
		m_pNextInfo = NULL;
		m_hContext = NULL;
		m_chDirSeparator[0] = '\\';
		m_chDirSeparator[1] = 0;
	}

	CXFileFind::~CXFileFind()
	{
		Close();
	}

	void CXFileFind::Close()
	{
		if (m_pFoundInfo != NULL)
		{
			delete m_pFoundInfo;
			m_pFoundInfo = NULL;
		}

		if (m_pNextInfo != NULL)
		{
			delete m_pNextInfo;
			m_pNextInfo = NULL;
		}

		if (m_hContext != NULL && m_hContext != INVALID_HANDLE_VALUE)
		{
			CloseContext();
			m_hContext = NULL;
		}
	}

	void CXFileFind::CloseContext()
	{
		::FindClose(m_hContext);
		return;
	}

	BOOL CXFileFind::FindFile( LPCTSTR pstrName )
	{
		Close();

		if (pstrName == NULL)	pstrName = _T("*.*");
		else if ( _tcslen( pstrName ) >= (_countof(((WIN32_FIND_DATA*) m_pNextInfo)->cFileName)))
		{
			::SetLastError(ERROR_BAD_ARGUMENTS);
			return FALSE;		
		}

		m_pNextInfo = new WIN32_FIND_DATA;

		WIN32_FIND_DATA *pFindData = (WIN32_FIND_DATA *)m_pNextInfo;

		_tcscpy(pFindData->cFileName, pstrName);

		m_hContext = ::FindFirstFile( pstrName, (WIN32_FIND_DATA*) m_pNextInfo );

		if( m_hContext == INVALID_HANDLE_VALUE )
		{
			DWORD dwTemp = ::GetLastError();
			Close();
			::SetLastError( dwTemp );
			return FALSE;
		}

		TCHAR pstrRoot[_MAX_PATH] = {0};

		_tcsncpy( pstrRoot, m_strRoot.c_str(), _countof( pstrRoot ) );
		LPCTSTR pstr = _tfullpath(pstrRoot, pstrName, _MAX_PATH);

		// passed name isn't a valid path but was found by the API
		ASSERT(pstr != NULL);
		if (pstr == NULL)
		{
			m_strRoot.clear();
			Close();
			::SetLastError(ERROR_INVALID_NAME);
			return FALSE;
		}
		else
		{
			// find the last forward or backward whack
			LPTSTR pstrBack  = _tcschr(pstrRoot, _T('\\'));
			LPTSTR pstrFront = _tcschr(pstrRoot, _T('/'));

			if (pstrFront != NULL || pstrBack != NULL)
			{
				if (pstrFront == NULL)
					pstrFront = pstrRoot;
				if (pstrBack == NULL)
					pstrBack = pstrRoot;

				// from the start to the last whack is the root

				if (pstrFront >= pstrBack)
					*pstrFront = _T('\0');
				else
					*pstrBack = _T('\0');
			}
			m_strRoot.clear();
		}

		return TRUE;
	}

	BOOL CXFileFind::MatchesMask(DWORD dwMask) const
	{
		if (m_pFoundInfo != NULL)
			return (!!(((LPWIN32_FIND_DATA) m_pFoundInfo)->dwFileAttributes & dwMask));
		else
			return FALSE;
	}

	BOOL CXFileFind::GetLastAccessTime(FILETIME* pTimeStamp) const
	{
		if (m_pFoundInfo != NULL && pTimeStamp != NULL)
		{
			*pTimeStamp = ((LPWIN32_FIND_DATA) m_pFoundInfo)->ftLastAccessTime;
			return TRUE;
		}
		else
			return FALSE;
	}

	BOOL CXFileFind::GetLastWriteTime(FILETIME* pTimeStamp) const
	{
		if (m_pFoundInfo != NULL && pTimeStamp != NULL)
		{
			*pTimeStamp = ((LPWIN32_FIND_DATA) m_pFoundInfo)->ftLastWriteTime;
			return TRUE;
		}
		else
			return FALSE;
	}

	BOOL CXFileFind::GetCreationTime(FILETIME* pTimeStamp) const
	{
		if (m_pFoundInfo != NULL && pTimeStamp != NULL)
		{
			*pTimeStamp = ((LPWIN32_FIND_DATA) m_pFoundInfo)->ftCreationTime;
			return TRUE;
		}
		else
			return FALSE;
	}

	BOOL CXFileFind::IsDots() const
	{
		// return TRUE if the file name is "." or ".." and
		// the file is a directory

		BOOL bResult = FALSE;
		if (m_pFoundInfo != NULL && IsDirectory())
		{
			LPWIN32_FIND_DATA pFindData = (LPWIN32_FIND_DATA) m_pFoundInfo;
			if (pFindData->cFileName[0] == '.')
			{
				if (pFindData->cFileName[1] == '\0' ||
					(pFindData->cFileName[1] == '.' &&
					pFindData->cFileName[2] == '\0'))
				{
					bResult = TRUE;
				}
			}
		}

		return bResult;
	}

	BOOL CXFileFind::FindNextFile()
	{
		if (m_hContext == NULL)
			return FALSE;
		if (m_pFoundInfo == NULL)
			m_pFoundInfo = new WIN32_FIND_DATA;

		void* pTemp = m_pFoundInfo;
		m_pFoundInfo = m_pNextInfo;
		m_pNextInfo = pTemp;

		return ::FindNextFile(m_hContext, (LPWIN32_FIND_DATA) m_pNextInfo);
	}

	BOOL CXFileFind::GetFileURL( _string& strFileURL ) const
	{
		strFileURL += _T("file://");
		_string strFilePath;
		GetFilePath( strFilePath );
		strFileURL += strFilePath;
		return TRUE;
	}

	BOOL CXFileFind::GetRoot( _string& strRoot ) const
	{
		strRoot = m_strRoot;
		return TRUE;
	}

	BOOL CXFileFind::GetFilePath( _string& strFilePath ) const
	{
		strFilePath = m_strRoot;
		LPCTSTR pszResult;
		LPCTSTR pchLast;
		_string strFileName;
		GetFileName( strFileName );
		strFilePath += strFileName.c_str();

		pszResult = ( LPCTSTR )strFilePath.c_str();
		pchLast = pszResult+strFilePath.length();
		if ( (*pchLast != (_T('\\'))) && (*pchLast != (_T('/')) ) )
			strFilePath += m_chDirSeparator;
		return TRUE;
	}

	BOOL CXFileFind::GetFileTitle( _string& strFileTitle ) const
	{
		_string strFullName;
		GetFileName( strFullName );
		TCHAR strResult[_MAX_PATH];

		_tsplitpath(strFullName.c_str(), NULL, NULL, strResult, NULL);
		strFileTitle = strResult;
		return TRUE;
	}

	BOOL CXFileFind::GetFileName( _string& strFileName ) const
	{
		if (m_pFoundInfo != NULL)
		{
			strFileName = (LPCTSTR)((LPWIN32_FIND_DATA) m_pFoundInfo)->cFileName;
			return TRUE;
		}
		return FALSE;
	}

	ULONGLONG CXFileFind::GetLength() const
	{
		ULARGE_INTEGER nFileSize;

		if (m_pFoundInfo != NULL)
		{
			nFileSize.LowPart = ((LPWIN32_FIND_DATA) m_pFoundInfo)->nFileSizeLow;
			nFileSize.HighPart = ((LPWIN32_FIND_DATA) m_pFoundInfo)->nFileSizeHigh;
		}
		else
		{
			nFileSize.QuadPart = 0;
		}

		return nFileSize.QuadPart;
	}
}
