#pragma once

class CConsoleBuffer
{
public:
	CConsoleBuffer( HANDLE hBufferHandle );
	CConsoleBuffer( short nBufferWidth, short nBufferHeight );
	CConsoleBuffer(void);
	~CConsoleBuffer(void);

	CConsoleBuffer& operator =( CConsoleBuffer& rsh ){ m_hBuffer = rsh.m_hBuffer; return *this; }
	CConsoleBuffer& operator =( HANDLE hScreenBuffer ){ m_hBuffer = hScreenBuffer; return *this; }
	operator HANDLE(){ return m_hBuffer; }
public:
	HANDLE active();

	int	 printf( LPCTSTR lpszFmt, ... );
	int	 cprintf( WORD wAttributes, LPCTSTR lpszFmt, ... );
	int	 scanf( LPCTSTR lpszFmt, ... );
	void create( short nBufferWidth = 79, short nBufferHeight = 21 );
	void destroy();
	void gotoxy( short x, short y );

protected:
	bool NewLine();
	void Scroll( int x );

protected:
	HANDLE m_hBuffer;
};

class CConsoleBufferArea
{
public:
	CConsoleBufferArea( HANDLE hScreenBuffer, SMALL_RECT rcArea );
	CConsoleBufferArea( HANDLE hScreenBuffer, SHORT nLeft, SHORT nTop, SHORT nRight, SHORT nBottom );
	~CConsoleBufferArea();

	int	 printf( LPCTSTR lpszFmt, ... );
	int	 cprintf( WORD wAttributes, LPCTSTR lpszFmt, ... );
	void gotoxy( short x, short y );

private:
	SMALL_RECT	m_rcArea;
	HANDLE		m_hScreenBuffer;
};

/*
void main(void) 
{
	_tprintf( 
		_T("asdfasdf\n")
		_T("s111\n")
		_T("s222\n")
		_T("s333\n")
		_T("s444\n") );
	CConsoleBuffer screen[2];
	screen[0].create( 80, 5 );
	screen[1] = screen[0].active();
	screen[0].printf( _T("asdfasdfasdf\n1\n2\n3\n4\n5\n6\n") );
	int i = 0;
	while( _getch() != 13 )
	{
		screen[i++%2].active();
	}
}
*/