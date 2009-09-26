#include "StdAfx.h"
#include <atlconv.h>
#include "net_helper.h"
#include "utility.h"
#pragma comment( lib, "shlwapi.lib" )
#pragma comment( lib, "Wininet.lib" )
namespace net_helper
{
	char* get_ip( const char *pszAny )
	{
		struct hostent *host;
		if ( pszAny == NULL) return NULL;

		size_t size = 0;
		host = gethostbyname( pszAny );
		if( host == NULL )
		{
			return NULL;
		}
		return (char*)inet_ntoa(*(struct in_addr *)(host->h_addr));
	}

	//--------------------------------------------------------------------------------------------------------//
	// D2Hex
	// pData		:	数据首址
	// dwDataSize	:	数据长度（按字符计）
	// pOut			:	输出缓冲首址
	// dwOutSize	:	缓冲区长度（按字符计）
	// dwFlags		:	标志 （未定义）
	// return		:	返回转换了多少字符
	//--------------------------------------------------------------------------------------------------------//
	DWORD data2hex( LPCSTR pData, DWORD dwDataSize, LPTSTR pOut, DWORD dwOutSize, DWORD dwFlags )
	{
		ASSERT( pData );
		ASSERT( pOut );
		ASSERT( debug_helper::IsValidAddress( pData, dwDataSize*sizeof(*pData), FALSE ) );
		ASSERT( debug_helper::IsValidAddress( pOut, dwOutSize*sizeof(*pOut) ) );
		//ASSERT( pData != pOut );

		int width = ((dwFlags&0x00000003)+1)*8;
		TCHAR* ascii = (TCHAR*)_alloca( (width + 1)*sizeof( TCHAR ) );
		TCHAR* p = pOut;
		TCHAR* end = p + dwOutSize;

		int n = _sntprintf( p, (size_t)(end - p), _T("\r\n") );
		p += n;
		DWORD i = 0;
		for( ; i < dwDataSize; ++i )
		{
			if( p >= end ) break;
			unsigned char uch = *(pData + i);
			n = _sntprintf( p, (size_t)(end - p), _T("%02x "), uch );
			if( n < 0 )
			{
				*(p-1) = 0;
				break;
			}
			if( _istprint( uch ) )
			{
				*ascii++ = uch;
			}
			else
			{
				*ascii++ = '.';
			}
			p += n;

			if( (i+1)%width == 0 )
			{
				*ascii++ = 0;
				ascii -= width+1;
				n = _sntprintf( p, (size_t)(end - p), _T("%s\r\n"), ascii );
				if( n < 0 )
				{
					*(p-1) = 0;
					break;
				}
				p += n;
			}
		}

		TCHAR szFmt[64];
		_sntprintf( szFmt, _countof( szFmt ), _T("%%%ds"), ( width - i%width ) * 3 );
		n = _sntprintf( p, (size_t)(end - p), szFmt, _T("") );
		if( n < 0 )
		{
			*(p-1) = 0;
		}
		else
		{
			p += n;
			*ascii++ = 0;
			ascii -= (i+1)%width;
			n = _sntprintf( p, (size_t)(end - p), _T("%s\r\n"), ascii );
			if( n < 0 )
			{
				*(p-1) = 0;
			}
			else
			{
				p += n;
			}
		}
		return DWORD(p - pOut);
	}

	//--------------------------------------------------------------------------------------------------------//
	// 初始化网络
	// init_extern_func	:	是否初始化扩展函数
	// return			:	初始化是否成功
	//--------------------------------------------------------------------------------------------------------//
	static GUID g_GUIDAcceptEx				= WSAID_ACCEPTEX;
	static GUID g_GUIDTransmitFile			= WSAID_TRANSMITFILE;
	static GUID g_GUIDGetAcceptSockaddrs	= WSAID_GETACCEPTEXSOCKADDRS;

	LPFN_ACCEPTEX				lpAcceptEx				= NULL;
	LPFN_TRANSMITFILE			lpTransmitFile			= NULL;
	LPFN_GETACCEPTEXSOCKADDRS	lpGetAcceptExSockaddrs	= NULL;

	bool init_winsock( bool init_extend_func )
	{
		WSADATA wsd;
		// 加载winsock库
		int nResult = WSAStartup(MAKEWORD(2,2), &wsd);
		if (0 != nResult)		return false;

		// 检查版本
		if ( LOBYTE( wsd.wVersion ) != 2 || HIBYTE( wsd.wVersion ) != 2 )
		{
			/* Tell the user that we could not find a usable */
			/* WinSock DLL.                                  */
			WSACleanup();
			return false; 
		}

		if( init_extend_func == true )
		{
			DWORD dwResult;
			SOCKET socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, 
				NULL, 0, WSA_FLAG_OVERLAPPED); 

			//
			//获取微软SOCKET扩展函数指针
			//
			nResult = WSAIoctl( 
				socket,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&g_GUIDAcceptEx,
				sizeof(g_GUIDAcceptEx),
				&lpAcceptEx,
				sizeof(lpAcceptEx),
				&dwResult,
				NULL,
				NULL
				);

			ASSERT( lpAcceptEx );

			if (SOCKET_ERROR == nResult)
			{
				closesocket(socket);
				return FALSE;
			}

			nResult = WSAIoctl(	
				socket,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&g_GUIDTransmitFile,
				sizeof(g_GUIDTransmitFile),
				&lpTransmitFile,
				sizeof(lpTransmitFile),
				&dwResult,
				NULL,
				NULL
				);

			if (SOCKET_ERROR == nResult)
			{
				closesocket(socket);
				return FALSE;
			}

			nResult = WSAIoctl(
				socket,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&g_GUIDGetAcceptSockaddrs,
				sizeof( g_GUIDGetAcceptSockaddrs ),
				&lpGetAcceptExSockaddrs,
				sizeof( lpGetAcceptExSockaddrs ),
				&dwResult,
				NULL,
				NULL
				);
			closesocket( socket );
			if (SOCKET_ERROR == nResult)
			{
				closesocket(socket);
				return FALSE;
			}
		}
		return TRUE;
	}

	BOOL AcceptEx(
		SOCKET sListenSocket,
		SOCKET sAcceptSocket,
		PVOID lpOutputBuffer,
		DWORD dwReceiveDataLength,
		DWORD dwLocalAddressLength,
		DWORD dwRemoteAddressLength,
		LPDWORD lpdwBytesReceived,
		LPOVERLAPPED lpOverlapped
		)
	{
		if( lpAcceptEx )
			return lpAcceptEx( sListenSocket, sAcceptSocket, lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength, lpdwBytesReceived, lpOverlapped );
		else
			return FALSE;
	}

	BOOL TransmitFile(
		SOCKET hSocket,
		HANDLE hFile,
		DWORD nNumberOfBytesToWrite,
		DWORD nNumberOfBytesPerSend,
		LPOVERLAPPED lpOverlapped,
		LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
		DWORD dwFlags
		)
	{
		if( lpTransmitFile )
			return lpTransmitFile( hSocket, hFile, nNumberOfBytesToWrite, nNumberOfBytesPerSend, lpOverlapped, lpTransmitBuffers, dwFlags );
		else
			return FALSE;
	}

	void GetAcceptExSockaddrs(
		PVOID lpOutputBuffer,
		DWORD dwReceiveDataLength,
		DWORD dwLocalAddressLength,
		DWORD dwRemoteAddressLength,
		LPSOCKADDR* LocalSockaddr,
		LPINT LocalSockaddrLength,
		LPSOCKADDR* RemoteSockaddr,
		LPINT RemoteSockaddrLength
		)
	{
		if( lpGetAcceptExSockaddrs )
			lpGetAcceptExSockaddrs( lpOutputBuffer, dwReceiveDataLength, dwLocalAddressLength, dwRemoteAddressLength, LocalSockaddr, LocalSockaddrLength, RemoteSockaddr, RemoteSockaddrLength );
	}

	//--------------------------------------------------------------------------------------------------------//
	BOOL ParseURLWorker(LPCTSTR pstrURL, LPURL_COMPONENTS lpComponents, DWORD& dwServiceType, WORD& nPort, DWORD dwFlags)
	{
		// this function will return bogus stuff if lpComponents
		// isn't set up to copy the components

		ASSERT(lpComponents != NULL && pstrURL != NULL);
		if (lpComponents == NULL || pstrURL == NULL)
			return FALSE;
		ASSERT(lpComponents->dwHostNameLength == 0 ||
			lpComponents->lpszHostName != NULL);
		ASSERT(lpComponents->dwUrlPathLength == 0 ||
			lpComponents->lpszUrlPath != NULL);
		ASSERT(lpComponents->dwUserNameLength == 0 ||
			lpComponents->lpszUserName != NULL);
		ASSERT(lpComponents->dwPasswordLength == 0 ||
			lpComponents->lpszPassword != NULL);

		ASSERT( debug_helper::IsValidAddress(lpComponents, sizeof(URL_COMPONENTS), TRUE));

		LPTSTR pstrCanonicalizedURL;
		TCHAR szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
		DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
		BOOL bRetVal;
		BOOL bMustFree = FALSE;

		// Decoding is done in InternetCrackUrl/UrlUnescape 
		// so we don't need the ICU_DECODE flag here.

		DWORD dwCanonicalizeFlags = dwFlags &
			(ICU_NO_ENCODE | ICU_NO_META |
			ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE);

		DWORD dwCrackFlags = dwFlags & ICU_USERNAME;

		BOOL bUnescape = FALSE;

		if((dwFlags & (ICU_ESCAPE | ICU_DECODE)) && (lpComponents->dwUrlPathLength != 0) )
		{

			// We use only the ICU_ESCAPE flag for decoding even if
			// ICU_DECODE is passed.

			// Also, if ICU_BROWSER_MODE is passed we do the unescaping
			// manually because InternetCrackUrl doesn't do
			// Browser mode unescaping

			if (dwFlags & ICU_BROWSER_MODE)
				bUnescape = TRUE;
			else
				dwCrackFlags |= ICU_ESCAPE;
		}

		bRetVal = InternetCanonicalizeUrl(pstrURL, szCanonicalizedURL,
			&dwNeededLength, dwCanonicalizeFlags);

		if (!bRetVal)
		{
			if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
				return FALSE;

			pstrCanonicalizedURL = new TCHAR[dwNeededLength];
			if (pstrCanonicalizedURL == NULL)
				return FALSE;

			bMustFree = TRUE;
			bRetVal = InternetCanonicalizeUrl(pstrURL, pstrCanonicalizedURL,
				&dwNeededLength, dwCanonicalizeFlags);
			if (!bRetVal)
			{
				delete [] pstrCanonicalizedURL;
				return FALSE;
			}
		}
		else
			pstrCanonicalizedURL = szCanonicalizedURL;

		// now that it's safely canonicalized, crack it

		bRetVal = InternetCrackUrl(pstrCanonicalizedURL, 0,
			dwCrackFlags, lpComponents);

		if(bUnescape)
		{
			if(FAILED(UrlUnescape(lpComponents->lpszUrlPath,NULL,NULL,URL_UNESCAPE_INPLACE | URL_DONT_UNESCAPE_EXTRA_INFO)))
			{
				if (bMustFree)
					delete [] pstrCanonicalizedURL;

				return FALSE;
			}

			lpComponents->dwUrlPathLength = lstrlen(lpComponents->lpszUrlPath);
		}

		if (bMustFree)
			delete [] pstrCanonicalizedURL;

		// convert to MFC-style service ID

		if (!bRetVal)
			dwServiceType = INET_SERVICE_UNK;
		else
		{
			nPort = lpComponents->nPort;
			dwServiceType = lpComponents->nScheme;
			//switch (lpComponents->nScheme)
			//{
			//case INTERNET_SCHEME_FTP:
			//	dwServiceType = AFX_INET_SERVICE_FTP;
			//	break;

			//case INTERNET_SCHEME_GOPHER:
			//	dwServiceType = AFX_INET_SERVICE_GOPHER;
			//	break;

			//case INTERNET_SCHEME_HTTP:
			//	dwServiceType = AFX_INET_SERVICE_HTTP;
			//	break;

			//case INTERNET_SCHEME_HTTPS:
			//	dwServiceType = AFX_INET_SERVICE_HTTPS;
			//	break;

			//case INTERNET_SCHEME_FILE:
			//	dwServiceType = AFX_INET_SERVICE_FILE;
			//	break;

			//case INTERNET_SCHEME_NEWS:
			//	dwServiceType = AFX_INET_SERVICE_NNTP;
			//	break;

			//case INTERNET_SCHEME_MAILTO:
			//	dwServiceType = AFX_INET_SERVICE_MAILTO;
			//	break;

			//default:
			//	dwServiceType = AFX_INET_SERVICE_UNK;
			//}
		}

		return bRetVal;
	}

	BOOL ParseURLEx(LPCTSTR pstrURL, DWORD& dwServiceType,
		_string& strServer, _string& strObject, INTERNET_PORT& nPort,
		_string& strUsername, _string& strPassword, DWORD dwFlags/* = 0*/)
	{
		dwServiceType = INET_SERVICE_UNK;

		ASSERT(pstrURL != NULL);
		if (pstrURL == NULL)
			return FALSE;

		URL_COMPONENTS urlComponents;
		memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
		urlComponents.dwStructSize = sizeof(URL_COMPONENTS);

		TCHAR szServer[INTERNET_MAX_HOST_NAME_LENGTH+1];
		TCHAR szObject[INTERNET_MAX_PATH_LENGTH+1];
		TCHAR szUsername[INTERNET_MAX_USER_NAME_LENGTH+1];
		TCHAR szPassword[INTERNET_MAX_PASSWORD_LENGTH+1];
		_tcsncpy( szServer, strServer.c_str(), _countof( szServer ) );
		_tcsncpy( szObject, strObject.c_str(), _countof( szObject ) );

		urlComponents.dwHostNameLength = INTERNET_MAX_HOST_NAME_LENGTH;
		urlComponents.lpszHostName = szServer;
		urlComponents.dwUrlPathLength = INTERNET_MAX_PATH_LENGTH;
		urlComponents.lpszUrlPath = szObject;
		urlComponents.dwUserNameLength = INTERNET_MAX_USER_NAME_LENGTH;
		urlComponents.lpszUserName = szUsername;
		urlComponents.dwPasswordLength = INTERNET_MAX_PASSWORD_LENGTH;
		urlComponents.lpszPassword = szPassword;

		BOOL bRetVal = ParseURLWorker(pstrURL, &urlComponents,
			dwServiceType, nPort, dwFlags);

		strServer = szServer;
		strObject = szObject;
		strUsername = szUsername;
		strPassword = szPassword;
		return bRetVal;
	}

	BOOL ParseURL(LPCTSTR pstrURL, DWORD& dwServiceType, _string& strServer, _string& strObject, INTERNET_PORT& nPort)
	{
		dwServiceType = INET_SERVICE_UNK;

		ASSERT(pstrURL != NULL);
		if (pstrURL == NULL)
			return FALSE;

		URL_COMPONENTS urlComponents;
		memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
		urlComponents.dwStructSize = sizeof(URL_COMPONENTS);

		TCHAR szServer[INTERNET_MAX_HOST_NAME_LENGTH+1];
		TCHAR szObject[INTERNET_MAX_PATH_LENGTH+1];
		_tcsncpy( szServer, strServer.c_str(), _countof( szServer ) );
		_tcsncpy( szObject, strObject.c_str(), _countof( szObject ) );

		urlComponents.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
		urlComponents.lpszHostName = szServer;
		urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
		urlComponents.lpszUrlPath = szObject;

		BOOL bRetVal = ParseURLWorker( pstrURL, &urlComponents, dwServiceType, nPort, ICU_BROWSER_MODE );

		strServer = szServer;
		strObject = szObject;
		return bRetVal;
	}
};