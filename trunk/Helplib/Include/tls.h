#pragma once

#ifndef _TLS_H
#define _TLS_H

#ifdef TOOLLIB_EXPORTS
#define TOOLLIB_API __declspec(dllexport)
#else
#define TOOLLIB_API __declspec(dllimport)
#endif	//TOOLLIB_EXPORTS

class TOOLLIB_API CTls
{
public:
	CTls();
	~CTls();

	PVOID	Get();
	BOOL	Set( PVOID pValue );
	DWORD	Index()const { return m_dwTlsIndex; }

private:
	DWORD m_dwTlsIndex;
};

#endif // _TLS_H