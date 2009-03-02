#pragma once
#ifndef _FILE_HELPER_H
#define _FILE_HELPER_H

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

namespace file_helper
{
	TOOLLIB_API bool get_datetime_filename( LPTSTR lpszBuffer, DWORD dwSize );

	class TOOLLIB_API CXFileFind
	{
	public:
		CXFileFind();
		virtual ~CXFileFind();

		// Attributes
	public:
		ULONGLONG GetLength() const;
		virtual BOOL GetFileName( _string& strFileName ) const;
		virtual BOOL GetFilePath( _string& strFilePath ) const;
		virtual BOOL GetFileTitle( _string& strFileTitle ) const;
		virtual BOOL GetFileURL( _string& strFileURl ) const;
		virtual BOOL GetRoot( _string& strRoot ) const;

		virtual BOOL GetLastWriteTime(FILETIME* pTimeStamp) const;
		virtual BOOL GetLastAccessTime(FILETIME* pTimeStamp) const;
		virtual BOOL GetCreationTime(FILETIME* pTimeStamp) const;

		virtual BOOL MatchesMask(DWORD dwMask) const;

		virtual BOOL IsDots() const;
		// these aren't virtual because they all use MatchesMask(), which is
		BOOL IsReadOnly() const;
		BOOL IsDirectory() const;
		BOOL IsCompressed() const;
		BOOL IsSystem() const;
		BOOL IsHidden() const;
		BOOL IsTemporary() const;
		BOOL IsNormal() const;
		BOOL IsArchived() const;

		// Operations
		void Close();
		virtual BOOL FindFile( LPCTSTR pstrName = NULL );
		virtual BOOL FindNextFile();

	protected:
		virtual void CloseContext();

		// Implementation
	protected:
		void* m_pFoundInfo;
		void* m_pNextInfo;
		HANDLE m_hContext;
		_string m_strRoot;
		TCHAR m_chDirSeparator[2];     // not '\\' for Internet classes
	};

	inline BOOL CXFileFind::IsReadOnly() const
	{ return MatchesMask(FILE_ATTRIBUTE_READONLY); }
	inline BOOL CXFileFind::IsDirectory() const
	{ return MatchesMask(FILE_ATTRIBUTE_DIRECTORY); }
	inline BOOL CXFileFind::IsCompressed() const
	{ return MatchesMask(FILE_ATTRIBUTE_COMPRESSED); }
	inline BOOL CXFileFind::IsSystem() const
	{ return MatchesMask(FILE_ATTRIBUTE_SYSTEM); }
	inline BOOL CXFileFind::IsHidden() const
	{ return MatchesMask(FILE_ATTRIBUTE_HIDDEN); }
	inline BOOL CXFileFind::IsTemporary() const
	{ return MatchesMask(FILE_ATTRIBUTE_TEMPORARY); }
	inline BOOL CXFileFind::IsNormal() const
	{ return MatchesMask(FILE_ATTRIBUTE_NORMAL); }
	inline BOOL CXFileFind::IsArchived() const
	{ return MatchesMask(FILE_ATTRIBUTE_ARCHIVE); }
}
#endif // _FILE_HELPER_H