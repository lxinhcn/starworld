#pragma once
#ifndef _NET_HELPER_H
#define _NET_HELPER_H
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <mswsock.h>
#if defined( _USE_WININET )
#include <WinInet.h>
#include <Shlwapi.h>
#endif

#ifdef EXPORT_API
#define EXPORT_LIB __declspec(dllexport)
#else
#define EXPORT_LIB __declspec(dllimport)
#endif

_lpctstr EXPORT_LIB get_ip( _lpctstr szAny );

//--------------------------------------------------------------------------------------------------------//
// D2Hex
// pData		:	数据首址
// dwDataSize	:	数据长度（按字符计）
// pOut			:	输出缓冲首址
// dwOutSize	:	缓冲区长度（按字符计）
// dwFlags		:	标志 （未定义）
// return		:	返回转换了多少字符
//--------------------------------------------------------------------------------------------------------//
#define HEX_WIDTH_8		0x00000000
#define HEX_WIDTH_16	0x00000001
#define HEX_WIDTH_32	0x00000002
#define HEX_WIDTH_64	0x00000003

DWORD EXPORT_LIB data2hex( LPCSTR pData, DWORD dwDataSize, LPTSTR pOut, DWORD dwOutSize, DWORD dwFlags = 0 );

//--------------------------------------------------------------------------------------------------------//
// 初始化网络
// init_extern_func	:	是否初始化扩展函数
// return			:	初始化是否成功
//--------------------------------------------------------------------------------------------------------//
bool EXPORT_LIB init_winsock( bool init_extend_func );

BOOL EXPORT_LIB acceptex(
	SOCKET sListenSocket,
	SOCKET sAcceptSocket,
	PVOID lpOutputBuffer,
	DWORD dwReceiveDataLength,
	DWORD dwLocalAddressLength,
	DWORD dwRemoteAddressLength,
	LPDWORD lpdwBytesReceived,
	LPOVERLAPPED lpOverlapped
	);

BOOL EXPORT_LIB transmitfile(
	SOCKET hSocket,
	HANDLE hFile,
	DWORD nNumberOfBytesToWrite,
	DWORD nNumberOfBytesPerSend,
	LPOVERLAPPED lpOverlapped,
	LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
	DWORD dwFlags
	);

void EXPORT_LIB get_acceptex_sockaddrs(
	PVOID lpOutputBuffer,
	DWORD dwReceiveDataLength,
	DWORD dwLocalAddressLength,
	DWORD dwRemoteAddressLength,
	LPSOCKADDR* LocalSockaddr,
	LPINT LocalSockaddrLength,
	LPSOCKADDR* RemoteSockaddr,
	LPINT RemoteSockaddrLength
	);

//--------------------------------------------------------------------------------------------------------//
#define INET_SERVICE_UNK        0x1000
//--------------------------------------------------------------------------------------------------------//
// inet 辅助函数
#if defined( _USE_WININET )

BOOL EXPORT_LIB ParseURLEx(_lpctstr pstrURL, DWORD& dwServiceType, _string& strServer, _string& strObject, INTERNET_PORT& nPort, _string& strUsername, _string& strPassword, DWORD dwFlags/* = 0*/);

BOOL EXPORT_LIB ParseURL(_lpctstr pstrURL, DWORD& dwServiceType, _string& strServer, _string& strObject, INTERNET_PORT& nPort);
#endif
//--------------------------------------------------------------------------------------------------------//
#endif // _NET_HELPER_H