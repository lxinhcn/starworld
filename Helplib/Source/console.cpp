#include "StdAfx.h"
#include "console.h"
#include <io.h>
#include <fcntl.h>

BOOL CConsole::IsExistent = FALSE;
CConsole::CConsole()
{
	if (IsExistent)
		return;
	AllocConsole();
	Attach(300, 80);
	IsExistent = TRUE;
}

CConsole::CConsole(LPCTSTR lpszTitle, SHORT ConsoleHeight, SHORT ConsoleWidth)
{
	if (IsExistent)
		return;
	AllocConsole();
	SetConsoleTitle(lpszTitle);
	Attach(ConsoleHeight, ConsoleWidth);
	IsExistent = TRUE;
}

void CConsole::Attach(SHORT ConsoleHeight, SHORT ConsoleWidth)
{
	HANDLE  hStd;
	int     fd;
	FILE    *file;

	// 重定向标准输入流句柄到新的控制台窗口
	hStd = GetStdHandle(STD_INPUT_HANDLE);
	fd = _open_osfhandle( reinterpret_cast<intptr_t>(hStd), _O_TEXT ); // 文本模式
	file = _fdopen(fd, "r");
	setvbuf(file, NULL, _IONBF, 0); // 无缓冲
	*stdin = *file;

	// 重定向标准输出流句柄到新的控制台窗口
	hStd = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD size;
	size.X = ConsoleWidth;
	size.Y = ConsoleHeight;
	SetConsoleScreenBufferSize(hStd, size);
	fd = _open_osfhandle(reinterpret_cast<intptr_t>(hStd), _O_TEXT); //文本模式
	file = _fdopen(fd, "w");
	setvbuf(file, NULL, _IONBF, 0); // 无缓冲
	*stdout = *file;

	// 重定向标准错误流句柄到新的控制台窗口
	hStd = GetStdHandle(STD_ERROR_HANDLE);
	fd = _open_osfhandle(reinterpret_cast<intptr_t>(hStd), _O_TEXT); // 文本模式
	file = _fdopen(fd, "w");
	setvbuf(file, NULL, _IONBF, 0); // 无缓冲
	*stderr = *file;

	SMALL_RECT rc = { 0, 0, ConsoleWidth -1, ConsoleHeight -1 };

	SetConsoleWindowInfo( hStd, TRUE, &rc );
}

CConsole::~CConsole()
{
	if (IsExistent)
	{
		FreeConsole();
		IsExistent = FALSE;
	}
}

void gotoxy( WORD x, WORD y )
{
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), c );
}

void cls( HANDLE hConsole )
{
	COORD coordScreen = { 0, 0 };    // home for the cursor 
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi; 
	DWORD dwConSize;

	// Get the number of character cells in the current buffer. 

	if( !GetConsoleScreenBufferInfo( hConsole, &csbi ))
		return;
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.

	if( !FillConsoleOutputCharacter( hConsole, (TCHAR) ' ',
		dwConSize, coordScreen, &cCharsWritten ))
		return;

	// Get the current text attribute.

	if( !GetConsoleScreenBufferInfo( hConsole, &csbi ))
		return;

	// Set the buffer's attributes accordingly.

	if( !FillConsoleOutputAttribute( hConsole, csbi.wAttributes,
		dwConSize, coordScreen, &cCharsWritten ))
		return;

	// Put the cursor at its home coordinates.

	SetConsoleCursorPosition( hConsole, coordScreen );
}
