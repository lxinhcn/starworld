#include "StdAfx.h"
#include "console_buffer.h"
#include <stdlib.h>
#include <malloc.h>

CConsoleBuffer::CConsoleBuffer( HANDLE hBufferHandle )
{
	m_hBuffer = hBufferHandle;
}

CConsoleBuffer::CConsoleBuffer( short nBufferWidth, short nBufferHeight )
{
	create( nBufferWidth, nBufferHeight );
}

CConsoleBuffer::CConsoleBuffer(void)
{
	create();
}

CConsoleBuffer::~CConsoleBuffer(void)
{
	destroy();
}

HANDLE CConsoleBuffer::active()
{
	HANDLE hStdHandle = GetStdHandle( STD_OUTPUT_HANDLE );
	SetConsoleActiveScreenBuffer( m_hBuffer );
	return hStdHandle;
}

bool CConsoleBuffer::NewLine(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
	if (! GetConsoleScreenBufferInfo( m_hBuffer, &csbiInfo ) ) 
	{
		return false;
	}

	csbiInfo.dwCursorPosition.X = 0; 

	// If it is the last line in the screen buffer, scroll 
	// the buffer up. 

	if ((csbiInfo.dwSize.Y-1) == csbiInfo.dwCursorPosition.Y) 
	{ 
		Scroll( 1 );
	} 

	// Otherwise, advance the cursor to the next line. 

	else 
		csbiInfo.dwCursorPosition.Y += 1; 

	if(! SetConsoleCursorPosition( m_hBuffer, csbiInfo.dwCursorPosition ) )
	{
		return false;
	}
	return true;
} 

void CConsoleBuffer::Scroll( int x )
{
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
	if (! GetConsoleScreenBufferInfo( m_hBuffer, &csbiInfo ) ) 
	{
		return;
	}

	SMALL_RECT srctScrollRect, srctClipRect;
	CHAR_INFO chiFill;
	COORD coordDest;

	srctScrollRect.Left = 0;
	srctScrollRect.Top = 1;
	srctScrollRect.Right = csbiInfo.dwSize.X - x; 
	srctScrollRect.Bottom = csbiInfo.dwSize.Y - x; 

	// The destination for the scroll rectangle is one row up. 

	coordDest.X = 0; 
	coordDest.Y = 0; 

	// The clipping rectangle is the same as the scrolling rectangle. 
	// The destination row is left unchanged. 

	srctClipRect = srctScrollRect; 

	// Set the fill character and attributes. 

	chiFill.Attributes = FOREGROUND_RED|FOREGROUND_INTENSITY; 
	chiFill.Char.AsciiChar = (char)' '; 

	// Scroll up one line. 

	ScrollConsoleScreenBuffer( 
		m_hBuffer,       // screen buffer handle 
		&srctScrollRect, // scrolling rectangle 
		&srctClipRect,   // clipping rectangle 
		coordDest,       // top left destination cell 
		&chiFill);       // fill character and color 
}

int	 CConsoleBuffer::printf( LPCTSTR lpszFmt, ... )
{
	va_list args;
	va_start(args, lpszFmt);

	TCHAR szOutput[64*1024];
	int i = _stprintf_s( szOutput, _countof( szOutput ), lpszFmt, args );
	if( i < 0 ) return -1;

	DWORD dwWriteBytes = 0;
	BOOL ret = WriteConsole( m_hBuffer, szOutput, i, &dwWriteBytes, NULL );
	if( !ret ) return -1;

	return dwWriteBytes;
}

/*
wAttributes values:
//////////////////////////////////////////////////////////////////////////
	FOREGROUND_BLUE			Text color contains blue. 
	FOREGROUND_GREEN		Text color contains green. 
	FOREGROUND_RED			Text color contains red. 
	FOREGROUND_INTENSITY	Text color is intensified. 
	BACKGROUND_BLUE			Background color contains blue. 
	BACKGROUND_GREEN		Background color contains green. 
	BACKGROUND_RED			Background color contains red. 
	BACKGROUND_INTENSITY	Background color is intensified. 
	COMMON_LVB_LEADING_BYTE		Leading byte. 
	COMMON_LVB_TRAILING_BYTE	Trailing byte. 
	COMMON_LVB_GRID_HORIZONTAL	Top horizontal. 
	COMMON_LVB_GRID_LVERTICAL	Left vertical. 
	COMMON_LVB_GRID_RVERTICAL	Right vertical. 
	COMMON_LVB_REVERSE_VIDEO	Reverse foreground and background attributes. 
	COMMON_LVB_UNDERSCORE		Underscore. 
*/

int	CConsoleBuffer::cprintf( WORD wAttributes, LPCTSTR lpszFmt, ... )
{
	SetConsoleTextAttribute( m_hBuffer, wAttributes );
	
	va_list args;
	va_start(args, lpszFmt);

	TCHAR szOutput[64*1024];
	int i = _stprintf_s( szOutput, _countof( szOutput ), lpszFmt, args );
	if( i < 0 ) return -1;

	DWORD dwWriteBytes = 0;
	BOOL ret = WriteConsole( m_hBuffer, szOutput, i, &dwWriteBytes, NULL );
	if( !ret ) return -1;

	return dwWriteBytes;
}

int	 CConsoleBuffer::scanf( LPCTSTR lpszFmt, ... )
{
	va_list args;
	va_start(args, lpszFmt);

	TCHAR szOutput[64*1024];
	DWORD dwReadBytes = 0;
	BOOL ret = ReadConsole( m_hBuffer, szOutput, _countof(szOutput), &dwReadBytes, NULL );
	if( !ret ) return -1;

	int i = _stscanf_s( szOutput, lpszFmt, args );
	return i;
}

void CConsoleBuffer::create( short nBufferWidth, short nBufferHeight )
{
	m_hBuffer = CreateConsoleScreenBuffer( 	
		GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
		);

	if( m_hBuffer )
	{
		COORD c = { nBufferWidth, nBufferHeight };
		SetConsoleScreenBufferSize( m_hBuffer, c );
	}
}

void CConsoleBuffer::destroy()
{
	CloseHandle( m_hBuffer );
}

void CConsoleBuffer::gotoxy( short x, short y )
{
	COORD c = { x, y };
	SetConsoleCursorPosition( m_hBuffer, c );
}

//////////////////////////////////////////////////////////////////////////
// CConsoleBufferArea
//////////////////////////////////////////////////////////////////////////
CConsoleBufferArea::CConsoleBufferArea( HANDLE hScreenBuffer, SMALL_RECT rcArea )
: m_hScreenBuffer( hScreenBuffer )
, m_rcArea( rcArea )
{
	
}

CConsoleBufferArea::CConsoleBufferArea( HANDLE hScreenBuffer, SHORT nLeft, SHORT nTop, SHORT nRight, SHORT nBottom )
: m_hScreenBuffer( hScreenBuffer )
{
	m_rcArea.Left	= nLeft;
	m_rcArea.Top	= nTop;
	m_rcArea.Right	= nRight;
	m_rcArea.Bottom = nBottom;
}

CConsoleBufferArea::~CConsoleBufferArea()
{

}

int	 CConsoleBufferArea::printf( LPCTSTR lpszFmt, ... )
{
	va_list args;
	va_start(args, lpszFmt);

	return cprintf( FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED, lpszFmt, args );
}

/*
wAttributes values:
//////////////////////////////////////////////////////////////////////////
	FOREGROUND_BLUE			Text color contains blue. 
	FOREGROUND_GREEN		Text color contains green. 
	FOREGROUND_RED			Text color contains red. 
	FOREGROUND_INTENSITY	Text color is intensified. 
	BACKGROUND_BLUE			Background color contains blue. 
	BACKGROUND_GREEN		Background color contains green. 
	BACKGROUND_RED			Background color contains red. 
	BACKGROUND_INTENSITY	Background color is intensified. 
	COMMON_LVB_LEADING_BYTE		Leading byte. 
	COMMON_LVB_TRAILING_BYTE	Trailing byte. 
	COMMON_LVB_GRID_HORIZONTAL	Top horizontal. 
	COMMON_LVB_GRID_LVERTICAL	Left vertical. 
	COMMON_LVB_GRID_RVERTICAL	Right vertical. 
	COMMON_LVB_REVERSE_VIDEO	Reverse foreground and background attributes. 
	COMMON_LVB_UNDERSCORE		Underscore. 
*/

int	 CConsoleBufferArea::cprintf( WORD wAttributes, LPCTSTR lpszFmt, ... )
{
	va_list args;
	va_start(args, lpszFmt);

	TCHAR szOutput[64*1024];
	int i = _vsntprintf( szOutput, _countof( szOutput ), lpszFmt, args );
	if( i < 0 ) return -1;

	DWORD dwWriteBytes = 0;
	COORD cSize = { m_rcArea.Right - m_rcArea.Left, m_rcArea.Bottom - m_rcArea.Top };
	COORD cPos	= { 0, 0 };
	PCHAR_INFO lpBuffer = (PCHAR_INFO)_alloca( i*sizeof(CHAR_INFO) );

	int s = 0;
	TCHAR ch = 0;
	for( int j = 0; j < i; ++j )
	{
		lpBuffer[j].Attributes	= wAttributes;
		ch = ( szOutput[s] == _T('\n') )?j%cSize.X == 0?szOutput[++s]:_T(' '):szOutput[s++];
#ifndef _UNICODE
		lpBuffer[j].Char.AsciiChar	= ch;
#else
		lpBuffer[j].Char.UnicodeChar= ch;
#endif
	}

	//BOOL ret = WriteConsole( m_hBuffer, szOutput, i, &dwWriteBytes, NULL );
	BOOL bRet = WriteConsoleOutput( m_hScreenBuffer, lpBuffer, cSize, cPos, &m_rcArea );
	if( !bRet ) return -1;

	return dwWriteBytes;
}

void CConsoleBufferArea::gotoxy( short x, short y )
{
	COORD c = { m_rcArea.Left + x, m_rcArea.Top + y };
	SetConsoleCursorPosition( m_hScreenBuffer, c );
}
